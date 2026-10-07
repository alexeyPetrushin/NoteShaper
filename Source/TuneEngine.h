#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

// A monophonic pitch-synchronous overlap/add tuner. Stereo audio shares the
// detector and pitch marks, so its image is preserved. No allocation in process.
namespace notefollow {
struct Settings {
    int root = 0;
    int chord = 0; // note, major, minor, dom7, maj7, min7, sus2, sus4, power, chromatic, custom
    unsigned customMask = 0x0ab5;
    float amount = 1.0f;
    float speedMs = 0.0f;
    float toleranceCents = 0.0f;
    int scale = 0; // off, major, natural minor, harmonic minor, major/minor pentatonic
    bool natural = false;
};

inline unsigned noteMask(const Settings& s) {
    constexpr unsigned relative[] = {1, 0x091, 0x089, 0x491, 0x891, 0x489,
                                      0x085, 0x0a1, 0x081, 0xfff};
    constexpr unsigned scales[] = {0, 0xab5, 0x5ad, 0x9ad, 0x295, 0x4a9};
    if (s.scale == 0 && s.chord >= 10) return s.customMask & 0xfff;
    unsigned m = s.scale > 0 ? scales[std::clamp(s.scale, 1, 5)]
                             : relative[std::clamp(s.chord, 0, 9)];
    int shift = ((s.root % 12) + 12) % 12;
    return ((m << shift) | (m >> (12 - shift))) & 0xfff;
}

inline double nearestNote(double midi, unsigned mask, double previous = -1000.0,
                          double hysteresis = 0.035) {
    if (!mask) return midi;
    double best = std::round(midi), distance = 1000.0;
    int centre = static_cast<int>(std::floor(midi));
    for (int note = centre - 12; note <= centre + 12; ++note) {
        int pc = ((note % 12) + 12) % 12;
        if ((mask & (1u << pc)) && std::abs(note - midi) < distance) {
            best = note; distance = std::abs(note - midi);
        }
    }
    int old = static_cast<int>(std::round(previous));
    if (previous > -100 && (mask & (1u << (((old % 12) + 12) % 12)))
        && std::abs(previous - midi) < distance + hysteresis) return previous;
    return best;
}

class TuneEngine {
public:
    void prepare(double rate) {
        sampleRate = rate;
        decimation = std::max(1, static_cast<int>(std::round(rate / 12000.0)));
        analysisRate = rate / decimation;
        hop = std::max(32, static_cast<int>(rate * 0.005));
        latencySamples = static_cast<int>(std::ceil(rate * 0.064));
        lead = static_cast<int>(std::ceil(rate / 65.0 * 1.6 + 16));
        size = 1;
        while (size < static_cast<int>(rate * 0.5)) size *= 2;
        ringMask = size - 1;
        for (auto& r : input) r.assign(size, 0.0f);
        for (auto& r : output) r.assign(size, 0.0f);
        weights.assign(size, 0.0f);
        blendHistory.assign(size, 0.0f);
        reset();
    }
    void reset() {
        for (auto& r : input) std::fill(r.begin(), r.end(), 0.0f);
        for (auto& r : output) std::fill(r.begin(), r.end(), 0.0f);
        std::fill(weights.begin(), weights.end(), 0.0f);
        std::fill(blendHistory.begin(), blendHistory.end(), 0.0f);
        time = 0; nextCentre = lead; lastEpoch = -1; currentPeriod = sampleRate / 220.0;
        correction = 0; targetMidi = -1000; inputMidi = -1000;
        confidence = 0; voiced = false; blend = 0; historyCount = 0; historyIndex = 0;
        unvoicedHops = 0; hasCorrection = false;
        pitchCentre = -1000; pendingTarget = -1000; pendingHops = 0; detectorChannel = 0;
    }
    int latency() const { return latencySamples; }
    double pitch() const { return inputMidi; }
    double target() const { return targetMidi; }
    double correctionCents() const { return correction; }
    double pitchConfidence() const { return confidence; }

    void process(float* left, float* right, int count, const Settings& settings) {
        const double fade = 1.0 - std::exp(-1.0 / (sampleRate * 0.008));
        for (int n = 0; n < count; ++n, ++time) {
            const int write = index(time);
            input[0][write] = std::isfinite(left[n]) ? left[n] : 0.0f;
            input[1][write] = right ? (std::isfinite(right[n]) ? right[n] : 0.0f) : input[0][write];
            if (time % hop == 0) analyse(time - latencySamples + lead, settings);

            const bool active = voiced && settings.amount > 0.00001f && noteMask(settings) != 0
                                && std::abs(correction) > 0.05;
            blend += fade * ((active ? 1.0 : 0.0) - blend);
            // Detector reads ahead by lead samples; align its voiced decision to audio.
            blendHistory[index(time + lead)] = static_cast<float>(blend);
            while (nextCentre <= static_cast<double>(time + lead)) {
                double ratio = std::pow(2.0, correction / 1200.0);
                ratio = std::clamp(ratio, 0.5, 2.0);
                const double period = std::clamp(currentPeriod, sampleRate / 1000.0, sampleRate / 65.0);
                addGrain(nextCentre, period, ratio);
                nextCentre += period / ratio;
            }

            const int dryIndex = index(time - latencySamples);
            const float norm = weights[write];
            const float mix = settings.amount <= 0.00001f || noteMask(settings) == 0
                                ? 0.0f : blendHistory[write];
            for (int ch = 0; ch < (right ? 2 : 1); ++ch) {
                const float dry = time >= latencySamples ? input[ch][dryIndex] : 0.0f;
                const float wet = norm > 0.0001f ? output[ch][write] / norm : dry;
                float result = dry + mix * (wet - dry);
                if (!std::isfinite(result)) result = dry;
                (ch == 0 ? left : right)[n] = result;
            }
            output[0][write] = output[1][write] = 0.0f;
            weights[write] = 0.0f;
            blendHistory[write] = 0.0f;
        }
    }

private:
    static constexpr double pi = 3.14159265358979323846;
    int index(std::int64_t t) const { return static_cast<int>(static_cast<std::uint64_t>(t) & ringMask); }
    float sample(int ch, double position) const {
        if (position < 0.0 || position + 1.0 > time) return 0.0f;
        auto p = static_cast<std::int64_t>(std::floor(position));
        float f = static_cast<float>(position - p);
        return input[ch][index(p)] * (1.0f - f) + input[ch][index(p + 1)] * f;
    }
    double mono(std::int64_t p) const {
        if (detectorChannel != 0) return input[detectorChannel - 1][index(p)];
        return 0.5 * (input[0][index(p)] + input[1][index(p)]);
    }
    void analyse(std::int64_t centre, const Settings& s) {
        constexpr int length = 512, comparison = 256;
        const auto begin = centre - static_cast<std::int64_t>(length / 2 * decimation);
        if (begin < 0 || begin + length * decimation >= time) { voiced = false; return; }
        // A phase-inverted stereo vocal can disappear in L+R. Detect from the
        // stronger channel in that case; both outputs still share pitch marks.
        double midEnergy = 0, leftEnergy = 0, rightEnergy = 0;
        for (int i = 0; i < length * decimation; ++i) {
            const double l = input[0][index(begin + i)], r = input[1][index(begin + i)];
            midEnergy += (l + r) * (l + r) * 0.25;
            leftEnergy += l * l; rightEnergy += r * r;
        }
        detectorChannel = midEnergy < 0.05 * std::max(leftEnergy, rightEnergy)
                            ? (leftEnergy >= rightEnergy ? 1 : 2) : 0;
        double mean = 0.0;
        for (int i = 0; i < length; ++i) {
            double sum = 0;
            for (int k = 0; k < decimation; ++k) sum += mono(begin + i * decimation + k);
            frame[i] = sum / decimation;
            mean += frame[i];
        }
        mean /= length;
        double energy = 0;
        for (double& x : frame) { x -= mean; energy += x * x; }
        if (energy / length < 0.0000063) {
            losePitch(); confidence = 0;
            return;
        }
        const int minTau = std::max(2, static_cast<int>(analysisRate / 1000.0));
        const int maxTau = std::min(250, static_cast<int>(analysisRate / 65.0));
        difference[0] = 1.0;
        double running = 0;
        for (int tau = 1; tau <= maxTau; ++tau) {
            double d = 0;
            for (int i = 0; i < comparison; ++i) {
                const double delta = frame[i] - frame[i + tau];
                d += delta * delta;
            }
            running += d;
            difference[tau] = running > 1e-20 ? d * tau / running : 1.0;
        }
        int candidate = -1;
        for (int tau = minTau; tau < maxTau; ++tau) {
            if (difference[tau] < 0.16) {
                while (tau + 1 <= maxTau && difference[tau + 1] < difference[tau]) ++tau;
                candidate = tau; break;
            }
        }
        if (candidate < 0) {
            candidate = minTau;
            for (int tau = minTau + 1; tau <= maxTau; ++tau)
                if (difference[tau] < difference[candidate]) candidate = tau;
        }
        confidence = std::clamp(1.0 - difference[candidate], 0.0, 1.0);
        if (confidence < 0.72) {
            losePitch();
            return;
        }
        double tau = candidate;
        if (candidate > 1 && candidate < maxTau) {
            const double a = difference[candidate - 1], b = difference[candidate], c = difference[candidate + 1];
            const double denominator = a - 2.0 * b + c;
            if (std::abs(denominator) > 1e-12) tau += std::clamp(0.5 * (a - c) / denominator, -0.5, 0.5);
        }
        const double midi = 69.0 + 12.0 * std::log2((analysisRate / tau) / 440.0);
        if (historyCount == 1 && std::abs(midi - pitchHistory[0]) > 0.8) {
            pitchHistory.fill(midi); historyIndex = 1; voiced = false; inputMidi = -1000;
            return;
        }
        pitchHistory[historyIndex++ % 3] = midi;
        historyCount = std::min(3, historyCount + 1);
        if (historyCount < 2) { voiced = false; inputMidi = -1000; return; }
        unvoicedHops = 0;
        double filtered = midi;
        if (historyCount == 3) {
            auto sorted = pitchHistory; std::sort(sorted.begin(), sorted.end()); filtered = sorted[1];
        }
        inputMidi = filtered;
        const double frequency = 440.0 * std::pow(2.0, (filtered - 69.0) / 12.0);
        currentPeriod = sampleRate / frequency;
        const unsigned mask = noteMask(s);
        const double nextTarget = nearestNote(filtered, mask, targetMidi,
                                              s.natural ? 0.24 : s.speedMs < 5 ? 0.025 : 0.065);
        const int oldPitchClass = ((static_cast<int>(std::round(targetMidi)) % 12) + 12) % 12;
        const bool oldAllowed = targetMidi > -100 && (mask & (1u << oldPitchClass));
        if (s.natural && oldAllowed && nextTarget != targetMidi
            && std::abs(filtered - targetMidi) - std::abs(filtered - nextTarget) < 0.7) {
            if (pendingTarget != nextTarget) { pendingTarget = nextTarget; pendingHops = 0; }
            if (++pendingHops * hop >= sampleRate * 0.04) {
                targetMidi = nextTarget; pendingHops = 0;
            }
        } else { targetMidi = nextTarget; pendingTarget = -1000; pendingHops = 0; }
        // Correct the slowly moving pitch centre in natural mode. Rapid small
        // movements remain in the performance; large note changes reset it.
        if (!hasCorrection || pitchCentre < -100 || std::abs(filtered - pitchCentre) > 0.8)
            pitchCentre = filtered;
        else pitchCentre += (1.0 - std::exp(-hop / (sampleRate * 0.07))) * (filtered - pitchCentre);
        double desired = (targetMidi - (s.natural ? pitchCentre : filtered)) * 100.0;
        // Soft threshold keeps small deviations/vibrato when desired.
        desired = std::copysign(std::max(0.0, std::abs(desired) - s.toleranceCents), desired);
        desired *= std::clamp(s.amount, 0.0f, 1.0f);
        const double coefficient = s.speedMs <= 0.01f ? 1.0
            : 1.0 - std::exp(-hop / (sampleRate * s.speedMs * 0.001));
        // Respect the requested retune time on the first voiced frame too.
        if (!hasCorrection) correction = 0;
        correction += coefficient * (desired - correction);
        hasCorrection = true;
        correction = std::clamp(correction, -1200.0, 1200.0);
        voiced = true;
    }
    void losePitch() {
        voiced = false; inputMidi = -1000;
        // Keep the median history across short note-transition ambiguities.
        // Otherwise one spurious low-frequency estimate can cause a large jump.
        if (++unvoicedHops >= 8) {
            historyCount = 0; historyIndex = 0; hasCorrection = false;
            targetMidi = -1000; confidence = 0; correction = 0;
            pendingHops = 0; pendingTarget = pitchCentre = -1000;
        }
    }
    void addGrain(double destination, double period, double ratio) {
        const double nominal = destination - latencySamples;
        const double radius = std::min(static_cast<double>(lead - 8), 1.1 * std::max(period, period / ratio));
        if (nominal - radius < 0 || nominal + radius + period * 0.5 + 2 > time) return;
        double predicted = nominal;
        bool continuity = lastEpoch >= 0 && std::abs(nominal - lastEpoch) < sampleRate * 0.1;
        if (continuity) predicted = lastEpoch + std::round((nominal - lastEpoch) / period) * period;
        const double search = period * (continuity ? 0.24 : 0.5);
        auto first = static_cast<std::int64_t>(std::ceil(predicted - search));
        auto last = static_cast<std::int64_t>(std::floor(predicted + search));
        auto best = first;
        double score = -1e30;
        for (auto p = first; p <= last; ++p) {
            double value = mono(p);
            if (value > score) { score = value; best = p; }
        }
        double epoch = static_cast<double>(best);
        double a = mono(best - 1), b = mono(best), c = mono(best + 1);
        double denominator = a - 2 * b + c;
        if (std::abs(denominator) > 1e-12)
            epoch += std::clamp(0.5 * (a - c) / denominator, -0.5, 0.5);
        lastEpoch = epoch;
        auto start = std::max(time, static_cast<std::int64_t>(std::ceil(destination - radius)));
        auto end = static_cast<std::int64_t>(std::floor(destination + radius));
        for (auto p = start; p <= end; ++p) {
            const double offset = p - destination;
            const float w = static_cast<float>(0.5 + 0.5 * std::cos(pi * offset / radius));
            const int out = index(p);
            weights[out] += w;
            output[0][out] += w * sample(0, epoch + offset);
            output[1][out] += w * sample(1, epoch + offset);
        }
    }

    double sampleRate = 48000, analysisRate = 12000;
    int decimation = 4, hop = 240, latencySamples = 3072, lead = 1200, size = 32768, ringMask = 32767;
    std::array<std::vector<float>, 2> input, output;
    std::vector<float> weights, blendHistory;
    std::array<double, 512> frame{};
    std::array<double, 256> difference{};
    std::array<double, 3> pitchHistory{};
    std::int64_t time = 0;
    double nextCentre = 0, lastEpoch = -1, currentPeriod = 200;
    double correction = 0, targetMidi = -1000, inputMidi = -1000, confidence = 0, blend = 0;
    int historyCount = 0, unvoicedHops = 0;
    int detectorChannel = 0, pendingHops = 0;
    double pitchCentre = -1000, pendingTarget = -1000;
    std::uint32_t historyIndex = 0;
    bool voiced = false, hasCorrection = false;
};
} // namespace notefollow

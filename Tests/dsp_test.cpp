#include "TuneEngine.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

using namespace notefollow;
constexpr double pi = 3.14159265358979323846;
static void require(bool condition, const std::string& message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
static std::vector<float> signal(double sr, double hz, double seconds, bool vowel = false) {
    std::vector<float> result(static_cast<int>(seconds * sr));
    for (int n = 0; n < static_cast<int>(result.size()); ++n) {
        double phase = 2 * pi * hz * n / sr;
        double value = 0.22 * std::sin(phase);
        if (vowel) {
            value = 0;
            for (int h = 1; h <= 20; ++h) {
                double f = h * hz;
                double env = 0.18 / h + 0.08 * std::exp(-std::pow((f - 700.0) / 230.0, 2))
                    + 0.025 * std::exp(-std::pow((f - 1200.0) / 260.0, 2));
                value += env * std::sin(h * phase + 0.15 * h);
            }
        }
        result[n] = static_cast<float>(value);
    }
    return result;
}
static double pitchHz(const std::vector<float>& signal, double sr, int start, double seconds = 0.12) {
    const int window = static_cast<int>(sr * seconds);
    const int low = static_cast<int>(sr / 600), high = static_cast<int>(sr / 65);
    double best = 1e30; int tauBest = low;
    std::vector<double> differences(high + 1);
    double running = 0;
    for (int tau = 1; tau <= high; ++tau) {
        double value = 0;
        for (int i = 0; i < window; ++i) {
            double d = signal[start + i] - signal[start + i + tau]; value += d * d;
        }
        running += value;
        differences[tau] = running > 1e-20 ? value * tau / running : 1;
    }
    for (int tau = low; tau < high; ++tau) {
        if (differences[tau] < 0.1) {
            while (tau + 1 <= high && differences[tau + 1] < differences[tau]) ++tau;
            tauBest = tau; best = differences[tau]; break;
        }
        if (differences[tau] < best) { best = differences[tau]; tauBest = tau; }
    }
    double a = differences[tauBest - 1], b = differences[tauBest], c = differences[tauBest + 1];
    double sub = std::abs(a - 2 * b + c) > 1e-12 ? 0.5 * (a - c) / (a - 2 * b + c) : 0;
    return sr / (tauBest + std::clamp(sub, -0.5, 0.5));
}
static void render(TuneEngine& engine, std::vector<float>& audio, Settings s, int block = 256) {
    for (int i = 0; i < static_cast<int>(audio.size()); i += block)
        engine.process(audio.data() + i, nullptr, std::min(block, static_cast<int>(audio.size()) - i), s);
}
static std::vector<float> modulated(double centre, double depth, double rate, double seconds = 1.6) {
    std::vector<float> audio(static_cast<int>(seconds * 48000)); double phase = 0;
    for (int i = 0; i < static_cast<int>(audio.size()); ++i) {
        const double cents = centre + depth * std::sin(2 * pi * rate * i / 48000.0);
        phase += 2 * pi * 220 * std::pow(2.0, cents / 1200.0) / 48000.0;
        audio[i] = static_cast<float>(0.22 * std::sin(phase));
    }
    return audio;
}
// Measure actual output cycles, rather than checking the correction formula.
static std::pair<double, double> contour(const std::vector<float>& audio) {
    double last = -1, sum = 0, sine = 0, cosine = 0; int count = 0;
    for (int i = 24000; i < static_cast<int>(audio.size()); ++i) {
        if (audio[i - 1] <= 0 && audio[i] > 0) {
            const double crossing = i - 1 - audio[i - 1] / static_cast<double>(audio[i] - audio[i - 1]);
            if (last > 0) {
                const double t = (crossing + last) * 0.5 / 48000.0;
                const double cents = 1200 * std::log2(48000.0 / ((crossing - last) * 220.0));
                sum += cents; sine += cents * std::sin(2 * pi * 6 * t); cosine += cents * std::cos(2 * pi * 6 * t); ++count;
            }
            last = crossing;
        }
    }
    require(count > 100, "too few output cycles");
    return {sum / count, 2 * std::hypot(sine, cosine) / count};
}
int main(int argc, char** argv) {
    const auto begin = std::chrono::steady_clock::now();
    for (double sr : {44100.0, 48000.0, 96000.0, 192000.0}) {
        TuneEngine engine; engine.prepare(sr);
        Settings s; s.root = 9; s.chord = 0; s.amount = 1; s.speedMs = 0;
        for (double inputHz : {208.0, 234.0, 415.0, 466.0, 110.7, 83.0}) {
            engine.reset(); auto audio = signal(sr, inputHz, 0.9, true);
            render(engine, audio, s, 127);
            double target = 440 * std::pow(2.0, (nearestNote(69 + 12 * std::log2(inputHz / 440.0), 1u << 9) - 69) / 12);
            double measured = pitchHz(audio, sr, static_cast<int>(sr * 0.45));
            double error = 1200 * std::log2(measured / target);
            std::cout << "sr=" << sr << " input=" << inputHz << " target=" << target
                      << " output=" << measured << " cents=" << error << '\n';
            require(std::abs(error) < 12, "full correction misses chosen note");
            for (auto sample : audio) require(std::isfinite(sample) && std::abs(sample) < 2, "invalid output");
        }
        engine.reset(); auto audio = signal(sr, 234, 0.7);
        auto original = audio; s.amount = 0; render(engine, audio, s);
        for (int i = 0; i < static_cast<int>(audio.size()); ++i)
            require(audio[i] == (i >= engine.latency() ? original[i - engine.latency()] : 0.0f), "0% changes dry signal");
        engine.reset(); audio = signal(sr, 234, 0.9); s.amount = 0.5f; render(engine, audio, s);
        const double partial = pitchHz(audio, sr, static_cast<int>(sr * 0.45));
        require(std::abs(1200 * std::log2(partial / std::sqrt(234.0 * 220.0))) < 12, "strength is not partial pitch correction");
        engine.reset(); audio = signal(sr, 234, 0.7); original = audio; s.amount = 1; s.chord = 10; s.customMask = 0;
        render(engine, audio, s);
        for (int i = 0; i < static_cast<int>(audio.size()); ++i)
            require(audio[i] == (i >= engine.latency() ? original[i - engine.latency()] : 0.0f), "empty selection changes dry signal");
    }
    Settings s; s.root = 0; s.chord = 1;
    require(noteMask(s) == ((1u << 0) | (1u << 4) | (1u << 7)), "major chord mask");
    s.root = 11; s.chord = 2;
    require(noteMask(s) == ((1u << 11) | (1u << 2) | (1u << 6)), "wrapped minor chord mask");
    require(nearestNote(64.3, 0) == 64.3, "empty note selection");
    s.root = 0; s.scale = 1; require(noteMask(s) == 0xab5, "major scale");
    s.scale = 2; require(noteMask(s) == 0x5ad, "natural minor scale");
    s.scale = 3; require(noteMask(s) == 0x9ad, "harmonic minor scale");
    s.scale = 4; require(noteMask(s) == 0x295, "major pentatonic scale");
    s.scale = 5; require(noteMask(s) == 0x4a9, "minor pentatonic scale");
    s.scale = 2; s.root = 9; require(noteMask(s) == 0xab5, "A minor transposition");
    s.scale = 0;
    TuneEngine engine; engine.prepare(48000);
    std::vector<float> noise(48000); std::mt19937 rng(1974); std::uniform_real_distribution<float> random(-0.08f, 0.08f);
    for (auto& x : noise) x = random(rng);
    auto noiseOriginal = noise; s.root = 9; s.chord = 0; s.amount = 1; render(engine, noise, s);
    require(engine.pitch() < 0, "noise classified as a note");
    for (int i = engine.latency(); i < 48000; ++i)
        require(std::abs(noise[i] - noiseOriginal[i - engine.latency()]) < 0.0001f, "noise is shifted");
    engine.reset(); std::vector<float> silence(48000, 0); render(engine, silence, s);
    for (float x : silence) require(x == 0, "silence produces sound");
    // Identical audio must be independent of the host's buffer size.
    auto smallBlocks = signal(48000, 234, 1.2, true), largeBlocks = smallBlocks;
    TuneEngine second; second.prepare(48000); engine.reset();
    render(engine, smallBlocks, s, 17); render(second, largeBlocks, s, 1024);
    require(smallBlocks == largeBlocks, "host block size changes the sound");
    // Smooth retuning must take measurably longer after a note transition.
    auto firstNote = signal(48000, 208, 0.6, true);
    auto nextNote = signal(48000, 234, 0.6, true);
    auto stepped = firstNote; stepped.insert(stepped.end(), nextNote.begin(), nextNote.end());
    auto smooth = stepped;
    engine.reset(); second.reset(); s.speedMs = 0; render(engine, stepped, s);
    s.speedMs = 160; render(second, smooth, s);
    const double hardEarly = pitchHz(stepped, 48000, static_cast<int>(48000 * 0.72), 0.03);
    const double softEarly = pitchHz(smooth, 48000, static_cast<int>(48000 * 0.72), 0.03);
    require(std::abs(hardEarly - 220) < 1.5, "hard mode does not lock after transition");
    require(softEarly > hardEarly + 3.0 && softEarly < 249.0, "retune-time control produces wrong transition");
    std::cout << "Transition: hard=" << hardEarly << " smooth=" << softEarly << " Hz\n";
    // A small detuning within the freedom threshold should retain its pitch.
    engine.reset(); s.speedMs = 0; s.toleranceCents = 15;
    auto freePitch = signal(48000, 221, 0.9, true); render(engine, freePitch, s);
    require(std::abs(pitchHz(freePitch, 48000, 22000) - 221) < 0.1, "freedom threshold flattens allowed detuning");
    s.toleranceCents = 0;
    // Slow tuning must start slowly, including after a pause in a phrase.
    engine.reset(); s.speedMs = 160;
    auto attack = signal(48000, 234, 0.5);
    auto firstCorrection = [&](std::vector<float>& input) {
        for (int i = 0; i < static_cast<int>(input.size()); i += 240) {
            engine.process(input.data() + i, nullptr, std::min(240, static_cast<int>(input.size()) - i), s);
            if (engine.pitch() > 0) return std::abs(engine.correctionCents());
        }
        return 1e6;
    };
    require(firstCorrection(attack) < 10, "slow retune jumps at voice onset");
    std::vector<float> gap(24000, 0); render(engine, gap, s);
    require(engine.target() < 0 && engine.pitchConfidence() == 0, "stale pitch after silence");
    attack = signal(48000, 234, 0.5);
    require(firstCorrection(attack) < 10, "slow retune jumps after silence");
    // Natural mode retains rapid movement while correcting its average centre.
    s.speedMs = 0; s.natural = false; engine.reset(); second.reset();
    auto robotic = modulated(35, 30, 6), natural = robotic;
    render(engine, robotic, s); s.natural = true; render(second, natural, s);
    const auto hardContour = contour(robotic), softContour = contour(natural);
    std::cout << "Vibrato: hard centre=" << hardContour.first << " depth=" << hardContour.second
              << "; natural centre=" << softContour.first << " depth=" << softContour.second << " cents\n";
    require(std::abs(hardContour.first) < 3 && hardContour.second < 5, "hard mode does not flatten vibrato");
    require(std::abs(softContour.first) < 4 && softContour.second > 20 && softContour.second < 42,
            "natural mode does not retain vibrato around corrected centre");
    // Tiny rapid movements across a chromatic boundary must not flip natural targets.
    auto changes = [&](bool expressive) {
        engine.reset(); auto audio = modulated(47, 10, 10);
        Settings selection; selection.chord = 9; selection.natural = expressive;
        double previous = -1000; int count = 0;
        for (int i = 0; i < static_cast<int>(audio.size()); i += 240) {
            engine.process(audio.data() + i, nullptr, std::min(240, static_cast<int>(audio.size()) - i), selection);
            if (i > 15000 && engine.pitch() > 0) {
                if (previous > 0 && engine.target() != previous) ++count;
                previous = engine.target();
            }
        }
        return count;
    };
    const int hardChanges = changes(false), naturalChanges = changes(true);
    std::cout << "Boundary target changes: hard=" << hardChanges << " natural=" << naturalChanges << '\n';
    require(hardChanges >= 5 && naturalChanges == 0, "natural target flickers at note boundary");
    // Detection must also survive opposite-polarity stereo, preserving that relationship.
    engine.reset(); auto left = signal(48000, 234, 1), right = left;
    for (auto& x : right) x = -x;
    s.natural = false;
    for (int i = 0; i < 48000; i += 256)
        engine.process(left.data() + i, right.data() + i, std::min(256, 48000 - i), s);
    require(engine.pitch() > 0 && std::abs(pitchHz(left, 48000, 24000) - 220) < 0.2, "opposite stereo polarity defeats tuning");
    for (int i = 0; i < 48000; ++i) require(std::abs(left[i] + right[i]) < 1e-6, "stereo polarity changed");
    // A melody note outside the triad must be a valid target in the full scale.
    engine.reset(); s.root = 0; s.chord = 1; s.scale = 1;
    auto melody = signal(48000, 300, 0.9, true); render(engine, melody, s);
    require(std::abs(pitchHz(melody, 48000, 24000) - 293.664768) < 0.2, "major scale does not tune to D");
    // Anti-flicker protection must still accept a deliberate change to B.
    engine.reset(); s.scale = 0; s.chord = 9; s.natural = true;
    auto from = signal(48000, 220 * std::pow(2.0, 35.0 / 1200), 0.6);
    auto to = signal(48000, 246.941651 * std::pow(2.0, 35.0 / 1200), 0.6);
    from.insert(from.end(), to.begin(), to.end()); render(engine, from, s);
    std::cout << "Natural transition: target=" << engine.target() << " output=" << pitchHz(from, 48000, 45000) << " Hz\n";
    require(engine.target() == 59 && std::abs(pitchHz(from, 48000, 45000) - 246.941651) < 0.2,
            "natural mode refuses deliberate melody transition");
    auto naturalSmall = modulated(35, 30, 6), naturalLarge = naturalSmall;
    engine.reset(); second.reset(); render(engine, naturalSmall, s, 17); render(second, naturalLarge, s, 1024);
    require(naturalSmall == naturalLarge, "natural mode depends on host block size");
    s.natural = false; s.root = 9; s.chord = 0;
    const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - begin).count();
    std::cout << "PASS: pitch accuracy, four sample rates, dry, empty targets, chords/scales, noise, silence, block sizes, retune attack/re-attack, freedom, vibrato, stable boundaries, melody changes, stereo polarity (" << elapsed << " s)\n";
    if (argc > 1) {
        engine.reset(); auto before = signal(48000, 234, 2, true), after = before; render(engine, after, s);
        std::ofstream raw(std::string(argv[1]) + "-input.f32", std::ios::binary);
        raw.write(reinterpret_cast<const char*>(before.data()), before.size() * sizeof(float));
        std::ofstream processed(std::string(argv[1]) + "-hard.f32", std::ios::binary);
        processed.write(reinterpret_cast<const char*>(after.data()), after.size() * sizeof(float));
    }
}

#pragma once
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

namespace santa {
namespace fs = std::filesystem;

// target[i] = who person i buys for, picked uniformly among all valid draws:
// shuffle and retry until nobody has themselves (and, if asked, no A<->B pairs).
// Needs n >= 2, and n >= 3 when no_mutual is set.
inline std::vector<size_t> draw(size_t n, bool no_mutual) {
    std::random_device rng;  // the OS's secure generator on Windows, macOS and Linux
    std::vector<size_t> t(n);
    for (bool ok = false; !ok;) {
        std::iota(t.begin(), t.end(), 0);
        std::shuffle(t.begin(), t.end(), rng);
        ok = true;
        for (size_t i = 0; i < n && ok; ++i) ok = t[i] != i && !(no_mutual && t[t[i]] == i);
    }
    return t;
}

// Names are UTF-8; going through u8string keeps them intact on Windows too.
inline fs::path u8path(const std::string& s) { return std::u8string(s.begin(), s.end()); }
inline std::string u8str(const fs::path& p) { auto s = p.u8string(); return {s.begin(), s.end()}; }

inline fs::path envelope(const fs::path& folder, const std::string& name) {
    return folder / u8path(name) / u8path(name + ".txt");
}

// Writes <folder>/<name>/<name>.txt for everyone. The BOM makes Notepad and TextEdit read UTF-8.
inline bool write_envelopes(const fs::path& folder, const std::vector<std::string>& names,
                            const std::vector<size_t>& target) {
    for (size_t i = 0; i < names.size(); ++i) {
        std::error_code ec;
        fs::create_directories(folder / u8path(names[i]), ec);
        std::ofstream f(envelope(folder, names[i]), std::ios::binary);
        f << "\xEF\xBB\xBF" << "Hi " << names[i] << "!\n\nYou are the Secret Santa for: " << names[target[i]] << "\n";
        if (!f) return false;
    }
    return true;
}

}  // namespace santa

#pragma once
#include <windows.h>
#include <bcrypt.h>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace santa {

// Uniform integer in [0, n) from the OS's secure RNG; rejecting the top sliver avoids modulo bias.
inline size_t random_below(size_t n) {
    const uint64_t limit = UINT64_MAX - UINT64_MAX % n;
    uint64_t x;
    do BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(&x), sizeof x, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    while (x >= limit);
    return x % n;
}

// target[i] = who person i buys for, picked uniformly among all valid draws:
// shuffle (Fisher-Yates) and retry until nobody has themselves (and, if asked, no A<->B pairs).
// Needs n >= 2, and n >= 3 when no_mutual is set.
inline std::vector<size_t> draw(size_t n, bool no_mutual) {
    std::vector<size_t> t(n);
    for (bool ok = false; !ok;) {
        for (size_t i = 0; i < n; ++i) t[i] = i;
        for (size_t i = n - 1; i > 0; --i) std::swap(t[i], t[random_below(i + 1)]);
        ok = true;
        for (size_t i = 0; i < n && ok; ++i) ok = t[i] != i && !(no_mutual && t[t[i]] == i);
    }
    return t;
}

// Empty if the name is usable, otherwise the reason it isn't.
inline std::wstring name_error(const std::wstring& name, const std::vector<std::wstring>& names) {
    if (name.empty()) return L"Type a name first.";
    if (name.find_first_of(L"<>:\"/\\|?*") != std::wstring::npos) return L"Names can't contain < > : \" / \\ | ? *";
    for (const auto& n : names)  // case-insensitive in any alphabet, like Windows file names
        if (CompareStringOrdinal(n.c_str(), -1, name.c_str(), -1, TRUE) == CSTR_EQUAL)
            return n + L" is already on the list.";
    return L"";
}

inline std::filesystem::path envelope(const std::filesystem::path& folder, const std::wstring& name) {
    return folder / name / (name + L".txt");
}

inline std::string utf8(const std::wstring& w) {
    std::string s(WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr), '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, s.data(), int(s.size()), nullptr, nullptr);
    s.pop_back();  // drop the terminating NUL
    return s;
}

// Writes <folder>/<name>/<name>.txt for everyone. UTF-8 with BOM so Notepad shows any alphabet.
inline bool write_envelopes(const std::filesystem::path& folder, const std::vector<std::wstring>& names,
                            const std::vector<size_t>& target) {
    for (size_t i = 0; i < names.size(); ++i) {
        std::error_code ec;
        std::filesystem::create_directories(folder / names[i], ec);
        std::ofstream f(envelope(folder, names[i]), std::ios::binary);
        f << "\xEF\xBB\xBF" << utf8(L"Hi " + names[i] + L"!\r\n\r\nYou are the Secret Santa for: " + names[target[i]] + L"\r\n");
        if (!f) return false;
    }
    return true;
}

}  // namespace santa

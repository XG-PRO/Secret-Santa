#include "santa.hpp"
#include <commctrl.h>
#include <shellapi.h>
#include <shobjidl.h>

enum { ID_TITLE = 1000, ID_NAME, ID_ADD, ID_LIST, ID_REMOVE, ID_NO_MUTUAL, ID_LABEL, ID_FOLDER, ID_BROWSE, ID_DRAW, ID_STATUS };

struct Control { int id; const wchar_t *cls, *text; DWORD style; int x, y, w, h; };
constexpr DWORD BTN = WS_TABSTOP | BS_PUSHBUTTON, EDIT = WS_TABSTOP | ES_AUTOHSCROLL | WS_BORDER;
const Control controls[] = {  // positions at 96 dpi
    {ID_TITLE, L"STATIC", L"Secret Santa", 0, 20, 12, 360, 36},
    {ID_NAME, L"EDIT", L"", EDIT, 20, 60, 268, 26},
    {ID_ADD, L"BUTTON", L"Add", BTN, 294, 60, 86, 26},
    {ID_LIST, L"LISTBOX", L"", WS_TABSTOP | WS_BORDER | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, 20, 94, 268, 220},
    {ID_REMOVE, L"BUTTON", L"Remove", BTN, 294, 94, 86, 26},
    {ID_NO_MUTUAL, L"BUTTON", L"No two people draw each other", WS_TABSTOP | BS_AUTOCHECKBOX, 20, 322, 360, 22},
    {ID_LABEL, L"STATIC", L"Save envelopes in:", 0, 20, 354, 360, 18},
    {ID_FOLDER, L"EDIT", L"", EDIT, 20, 374, 268, 26},
    {ID_BROWSE, L"BUTTON", L"Browse…", BTN, 294, 374, 86, 26},
    {ID_DRAW, L"BUTTON", L"Draw names", BTN, 20, 414, 360, 36},
    {ID_STATUS, L"STATIC", L"", SS_ENDELLIPSIS, 20, 458, 360, 20},
};

HWND wnd;
std::vector<std::wstring> names;
bool status_error;

HWND item(int id) { return GetDlgItem(wnd, id); }

std::wstring text_of(int id) {
    std::wstring s(GetWindowTextLengthW(item(id)) + 1, L'\0');
    s.resize(GetWindowTextW(item(id), s.data(), int(s.size())));
    return s;
}

void status(const std::wstring& s, bool error = false) {
    status_error = error;
    SetWindowTextW(item(ID_STATUS), s.c_str());
}

void show_count() {
    status(names.size() == 1 ? L"1 person" : std::to_wstring(names.size()) + L" people");
}

void add_name() {
    std::wstring name = text_of(ID_NAME);
    name.erase(0, name.find_first_not_of(L" \t"));
    name.erase(name.find_last_not_of(L" \t") + 1);
    if (auto err = santa::name_error(name, names); !err.empty()) return status(err, true);
    names.push_back(name);
    SendMessageW(item(ID_LIST), LB_ADDSTRING, 0, LPARAM(name.c_str()));
    SetWindowTextW(item(ID_NAME), L"");
    show_count();
}

void remove_name() {
    int i = int(SendMessageW(item(ID_LIST), LB_GETCURSEL, 0, 0));
    if (i < 0) return status(L"Select a name to remove.", true);
    names.erase(names.begin() + i);
    SendMessageW(item(ID_LIST), LB_DELETESTRING, i, 0);
    SendMessageW(item(ID_LIST), LB_SETCURSEL, std::min(i, int(names.size()) - 1), 0);
    show_count();
}

void browse() {
    IFileOpenDialog* dlg;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dlg)))) return;
    dlg->SetOptions(FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
    IShellItem* result;
    PWSTR path;
    if (SUCCEEDED(dlg->Show(wnd)) && SUCCEEDED(dlg->GetResult(&result))) {
        if (SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
            SetWindowTextW(item(ID_FOLDER), path);
            CoTaskMemFree(path);
        }
        result->Release();
    }
    dlg->Release();
}

void draw_names() {
    bool no_mutual = IsDlgButtonChecked(wnd, ID_NO_MUTUAL);
    if (names.size() < (no_mutual ? 3u : 2u))
        return status(no_mutual ? L"Add at least 3 people for this rule." : L"Add at least 2 people.", true);
    std::filesystem::path folder = text_of(ID_FOLDER);

    for (const auto& n : names)
        if (std::filesystem::exists(santa::envelope(folder, n))) {
            if (MessageBoxW(wnd, L"This folder already has envelopes from an earlier draw. Replace them?",
                            L"Secret Santa", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES) return;
            break;
        }

    if (!santa::write_envelopes(folder, names, santa::draw(names.size(), no_mutual)))
        return status(L"Couldn't save the envelopes in that folder.", true);
    status(L"Done! " + std::to_wstring(names.size()) + L" envelopes saved.");
    if (MessageBoxW(wnd, L"Each person has a folder with a file naming who they buy for.\nSend everyone only their own file.\n\nOpen the folder now?",
                    L"Envelopes ready", MB_YESNO | MB_ICONINFORMATION) == IDYES)
        ShellExecuteW(wnd, L"open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case ID_ADD: add_name(); break;
        case ID_REMOVE: remove_name(); break;
        case ID_BROWSE: browse(); break;
        case ID_DRAW: draw_names(); break;
        }
        return 0;
    case WM_CTLCOLORSTATIC:  // white background; red text for errors
        SetBkColor(HDC(wp), RGB(255, 255, 255));
        SetTextColor(HDC(wp), HWND(lp) == item(ID_STATUS) && status_error ? RGB(200, 30, 30) : RGB(30, 30, 30));
        return LRESULT(GetStockObject(WHITE_BRUSH));
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, PWSTR, int show) {
    // Modern-looking controls come from the comctl32 v6 manifest, embedded as resource 2
    // (resource 1 is MinGW's default manifest).
    ACTCTXW actctx{sizeof actctx};
    actctx.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;
    actctx.hModule = inst;
    actctx.lpResourceName = MAKEINTRESOURCEW(2);
    ULONG_PTR cookie;
    ActivateActCtx(CreateActCtxW(&actctx), &cookie);
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
    InitCommonControls();
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSW wc{};
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = HBRUSH(GetStockObject(WHITE_BRUSH));
    wc.lpszClassName = L"SecretSanta";
    RegisterClassW(&wc);

    const int dpi = int(GetDpiForSystem());
    auto px = [dpi](int v) { return MulDiv(v, dpi, 96); };
    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT r{0, 0, px(400), px(490)};
    AdjustWindowRectExForDpi(&r, style, FALSE, 0, dpi);
    wnd = CreateWindowExW(WS_EX_CONTROLPARENT, wc.lpszClassName, L"Secret Santa", style, CW_USEDEFAULT, CW_USEDEFAULT,
                          r.right - r.left, r.bottom - r.top, nullptr, nullptr, inst, nullptr);

    auto font = [&](int size, int weight) {
        return CreateFontW(-px(size), 0, 0, 0, weight, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    };
    HFONT body = font(14, FW_NORMAL), title = font(26, FW_SEMIBOLD);
    for (const auto& c : controls) {
        HWND h = CreateWindowW(c.cls, c.text, WS_CHILD | WS_VISIBLE | c.style, px(c.x), px(c.y), px(c.w), px(c.h), wnd,
                               HMENU(INT_PTR(c.id)), inst, nullptr);
        SendMessageW(h, WM_SETFONT, WPARAM(c.id == ID_TITLE ? title : body), FALSE);
    }
    SendMessageW(item(ID_NAME), EM_SETCUEBANNER, TRUE, LPARAM(L"Type a name and press Enter"));
    wchar_t exe[MAX_PATH];
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    SetWindowTextW(item(ID_FOLDER), std::filesystem::path(exe).parent_path().c_str());
    CheckDlgButton(wnd, ID_NO_MUTUAL, BST_CHECKED);
    show_count();

    ShowWindow(wnd, show);
    SetFocus(item(ID_NAME));
    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0) > 0) {
        if (m.message == WM_KEYDOWN && m.wParam == VK_RETURN && m.hwnd == item(ID_NAME)) { add_name(); continue; }
        if (m.message == WM_KEYDOWN && m.wParam == VK_DELETE && m.hwnd == item(ID_LIST)) { remove_name(); continue; }
        if (IsDialogMessageW(wnd, &m)) continue;
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }
    return 0;
}

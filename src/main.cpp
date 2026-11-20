#include "santa.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/fl_utf8.h>

static std::vector<std::string> names;
static Fl_Input *name_in, *folder_in;
static Fl_Hold_Browser* list;
static Fl_Check_Button *no_mutual, *own_folder;
static Fl_Box* status;

static void set_status(const std::string& s, bool error = false) {
    status->copy_label(s.c_str());
    status->labelcolor(error ? fl_rgb_color(200, 30, 30) : FL_FOREGROUND_COLOR);
    status->redraw();
}

static void show_count() { set_status(std::to_string(names.size()) + (names.size() == 1 ? " person" : " people")); }

static void add_cb(Fl_Widget*, void*) {
    std::string n = name_in->value();
    n.erase(0, n.find_first_not_of(" \t"));
    n.erase(n.find_last_not_of(" \t") + 1);
    name_in->take_focus();
    if (n.empty()) return set_status("Type a name first.", true);
    if (n.find_first_of("<>:\"/\\|?*") != std::string::npos) return set_status("Names can't contain < > : \" / \\ | ? *", true);
    for (const auto& e : names)  // case-insensitive in any alphabet
        if (fl_utf_strcasecmp(e.c_str(), n.c_str()) == 0) return set_status(e + " is already on the list.", true);
    names.push_back(n);
    list->add(n.c_str());
    name_in->value("");
    show_count();
}

static void remove_cb(Fl_Widget*, void*) {
    int i = list->value();  // 1-based, 0 = nothing selected
    if (!i) return set_status("Select a name to remove.", true);
    names.erase(names.begin() + i - 1);
    list->remove(i);
    list->select(std::min(i, list->size()));
    show_count();
}

static void browse_cb(Fl_Widget*, void*) {
    Fl_Native_File_Chooser fc(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
    fc.title("Choose where to save the envelopes");
    if (fc.show() == 0) folder_in->value(fc.filename());
}

static void draw_cb(Fl_Widget*, void*) {
    bool nm = no_mutual->value();
    if (names.size() < (nm ? 3u : 2u)) return set_status(nm ? "Add at least 3 people for this rule." : "Add at least 2 people.", true);
    bool own = own_folder->value();
    auto folder = santa::u8path(folder_in->value());
    for (const auto& n : names)
        if (std::filesystem::exists(santa::envelope(folder, n, own))) {
            if (fl_choice("This folder already has envelopes from an earlier draw. Replace them?", "Cancel", "Replace", nullptr) != 1) return;
            break;
        }
    if (!santa::write_envelopes(folder, names, santa::draw(names.size(), nm), own))
        return set_status("Couldn't save the envelopes in that folder.", true);
    set_status("Done! " + std::to_string(names.size()) + " envelopes saved.");
    fl_message("Envelopes saved in:\n%s\n\nEach person has %s naming who they buy for.\nSend everyone only their own file.",
               folder_in->value(), own ? "a folder with a file" : "a file");
}

int main(int argc, char** argv) {
    Fl::scheme("gtk+");
    Fl::background(250, 250, 250);
    fl_message_title_default("Secret Santa");

    auto* win = new Fl_Window(400, 516, "Secret Santa");
    auto* title = new Fl_Box(20, 12, 360, 40, "Secret Santa");
    title->labelfont(FL_HELVETICA_BOLD);
    title->labelsize(26);
    title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    name_in = new Fl_Input(20, 60, 268, 28);
    name_in->tooltip("Type a name and press Enter");
    name_in->when(FL_WHEN_ENTER_KEY_ALWAYS);
    name_in->callback(add_cb);
    (new Fl_Button(296, 60, 84, 28, "Add"))->callback(add_cb);

    list = new Fl_Hold_Browser(20, 96, 268, 220);
    list->format_char(0);  // show names starting with '@' literally
    (new Fl_Button(296, 96, 84, 28, "Remove"))->callback(remove_cb);

    no_mutual = new Fl_Check_Button(16, 324, 364, 26, "No two people draw each other");
    no_mutual->value(1);
    own_folder = new Fl_Check_Button(16, 350, 364, 26, "Put each file in its own folder");
    own_folder->value(1);

    auto* label = new Fl_Box(20, 382, 360, 20, "Save envelopes in:");
    label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    folder_in = new Fl_Input(20, 404, 268, 28);
    (new Fl_Button(296, 404, 84, 28, "Browse..."))->callback(browse_cb);
#ifdef _WIN32
    const char* home = fl_getenv("USERPROFILE");
#else
    const char* home = fl_getenv("HOME");
#endif
    auto base = santa::u8path(home ? home : ".");
    folder_in->value(santa::u8str((std::filesystem::is_directory(base / "Desktop") ? base / "Desktop" : base) / "Secret Santa").c_str());

    auto* draw = new Fl_Button(20, 444, 360, 38, "Draw names");
    draw->color(fl_rgb_color(176, 30, 50));
    draw->labelcolor(FL_WHITE);
    draw->labelfont(FL_HELVETICA_BOLD);
    draw->callback(draw_cb);

    status = new Fl_Box(20, 488, 360, 22);
    status->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    show_count();

    win->end();
    win->show(argc, argv);
    name_in->take_focus();
    return Fl::run();
}

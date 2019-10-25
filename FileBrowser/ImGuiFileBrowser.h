#ifndef IMGUIFILEBROWSER_H
#define IMGUIFILEBROWSER_H

#include <imgui.h>
#include <string>
#include <vector>

namespace imgui_addons
{
    class ImGuiFileBrowser
    {
        public:
            ImGuiFileBrowser();
            ~ImGuiFileBrowser();
            std::string showFileDialog(std::string label, ImVec2 sz_xy);

        private:
            std::string wStringToString(const wchar_t* wchar_arr);
            bool readDIR(std::string path);
            void parsePathTabs(std::string str);
            void loadWindowsDrives(); // Windows Exclusive
            void showErrorModal();
            bool onNavigationButtonClick(int idx);
            bool onContentClick(int idx, bool show_drives);

            std::vector<std::string> current_dirlist;
            std::vector<std::string> subdirs;
            std::vector<std::string> subfiles;
            std::string current_path;
            int col_items_limit, selected_idx;
            float col_width;
            bool show_hidden, is_dir;
    };
}


#endif // IMGUIFILEBROWSER_H

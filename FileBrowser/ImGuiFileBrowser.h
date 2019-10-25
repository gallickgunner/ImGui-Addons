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
            bool showFileDialog(std::string label, ImVec2 sz_xy);
            std::string selected_fn;

        private:
            struct Info
            {
                std::string name;
                bool is_hidden;
            };
            std::string wStringToString(const wchar_t* wchar_arr);
            bool readDIR(std::string path);
            void parsePathTabs(std::string str);
            void showErrorModal();
            bool loadWindowsDrives(); // Windows Exclusive
            bool onNavigationButtonClick(int idx);
            bool onContentClick(int idx, bool show_drives);

            std::vector<std::string> current_dirlist;
            std::vector<Info> subdirs;
            std::vector<Info> subfiles;
            std::string current_path;
            int col_items_limit, selected_idx;
            float col_width;
            bool show_hidden, is_dir;
    };
}


#endif // IMGUIFILEBROWSER_H

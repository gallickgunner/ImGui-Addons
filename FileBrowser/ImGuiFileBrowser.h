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
            bool showFileDialog(std::string label, ImVec2 sz_xy, std::string filter_files = "");
            std::string selected_fn;

        private:
            struct Info
            {
                std::string name;
                bool is_hidden;
            };
            static std::string wStringToString(const wchar_t* wchar_arr);
            static bool alphaSortComparator(const Info& a, const Info& b);

            bool validateFile();
            bool readDIR(std::string path);
            bool onNavigationButtonClick(int idx);
            bool onDirClick(int idx, bool show_drives);
            #if defined (WIN32) || defined (_WIN32) || defined (__WIN32)
            bool loadWindowsDrives(); // Windows Exclusive
            #endif

            void parsePathTabs(std::string str);
            void showErrorModal();
            void showInvalidFileModal();
            void clearOldEntries();


            ImGuiTextFilter filter;
            std::string filter_files;
            std::vector<std::string> valid_exts;
            std::vector<std::string> current_dirlist;
            std::vector<Info> subdirs;
            std::vector<Info> subfiles;
            std::vector<Info*> filtered_dirs; // Note: We don't need to call delete. It's just for storing filtered items from subdirs and subfiles so we don't use PassFilter every frame.
            std::vector<Info*> filtered_files;
            std::string current_path;
            int col_items_limit, selected_idx;
            float col_width;
            bool show_hidden, is_dir, filter_dirty;
    };
}


#endif // IMGUIFILEBROWSER_H

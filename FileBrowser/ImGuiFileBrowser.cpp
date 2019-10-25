#include "ImGuiFileBrowser.h"

#include <imgui_internal.h>
#include <sstream>
#include <cwchar>
#include <algorithm>
#include <iostream>
#include <cmath>

#if defined (WIN32) || defined (_WIN32)
#define OSWIN
#include "Dirent/dirent.h"
#include <windows.h>
#else
#include <dirent.h>
#endif // defined (WIN32) || defined (_WIN32)

namespace imgui_addons
{
    ImGuiFileBrowser::ImGuiFileBrowser() : col_items_limit(12), show_hidden(false), selected_idx(-1), col_width(220.0f)
    {
        is_dir = false;
        #ifdef OSWIN
        if(loadWindowsDrives())
            current_dirlist.push_back("Computer");
        else
            current_path = "./";
        #else
        current_path = "/";
        current_dirlist.push_back("/");
        #endif
    }

    ImGuiFileBrowser::~ImGuiFileBrowser()
    {

    }

    std::string ImGuiFileBrowser::showFileDialog(std::string label, ImVec2 sz_xy)
    {
        ImGuiIO& io = ImGui::GetIO();
        std::string selected_fn = "";
        bool show_error = false;

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowContentSize(sz_xy);
        if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            /* If subdirs and subfiles are empty, either we are on Unix OS or loadWindowsDrives() failed.
             * Hence read default directory (./) on Windows and "/" on Unix OS once
             */
            if(subdirs.empty() && subfiles.empty())
                show_error |= !(readDIR(current_path));

            int num_cols = std::ceil((subdirs.size() + subfiles.size()) / (float) col_items_limit);
            int filtered_items = 0;
            if(num_cols == 0)
                num_cols = 1;
            float content_width = num_cols * col_width;
            float frame_height = ImGui::GetFrameHeight();
            float frame_height_spacing = ImGui::GetFrameHeightWithSpacing();
            if(content_width < sz_xy.x)
                content_width = sz_xy.x;

            //Render top file bar for easy navigation
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.882f, 0.745f, 0.078f,1.0f));
            for(int i = 0; i < current_dirlist.size(); i++)
            {

                if( ImGui::Button(current_dirlist[i].c_str()) )
                {
                    //If last button clicked, nothing happens
                    if(i == current_dirlist.size() - 1)
                        break;
                    show_error |= !(onNavigationButtonClick(i));
                }

                //Draw Arrow Buttons
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.01f));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f,1.0f));
                if(i != current_dirlist.size() - 1)
                {
                    ImGui::SameLine(0,0);
                    ImGui::ArrowButtonEx("##Right", ImGuiDir_Right, ImVec2(frame_height,frame_height), ImGuiButtonFlags_Disabled);
                    ImGui::SameLine(0,0);
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            ImGui::PopStyleColor();

            ImGui::Separator();
            ImVec2 cursor_pos = ImGui::GetCursorPos();
            ImGui::SetCursorPosY(sz_xy.y - frame_height_spacing * 1.5f);

            ImGuiTextFilter filter;
            filter.Draw("Filter (inc, -exc)", sz_xy.x - 180);


            ImGui::SetCursorPos(cursor_pos);
            ImGui::SetNextWindowContentSize(ImVec2(content_width, 0.0f));
            ImGui::BeginChild("##ScrollingRegion", ImVec2(0, -70), true, ImGuiWindowFlags_HorizontalScrollbar);

            ImGui::Columns(num_cols);

            //Output directories in yellow
            bool show_drives = false;
            #ifdef OSWIN
            (current_dirlist.back() == "Computer") ? show_drives = true : show_drives = false;
            #endif // OSWIN

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.882f, 0.745f, 0.078f,1.0f));
            for (int i = 0; i < subdirs.size(); i++)
            {
                if( (!subdirs[i].is_hidden | show_hidden) && filter.PassFilter(subdirs[i].name.c_str()))
                {
                    filtered_items++;
                    if(ImGui::Selectable(subdirs[i].name.c_str(), selected_idx == i && is_dir, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        selected_idx = i;
                        is_dir = true;
                        if(ImGui::IsMouseDoubleClicked(0))
                            show_error |= !(onContentClick(i, show_drives));
                    }
                    if( (filtered_items) % col_items_limit == 0)
                        ImGui::NextColumn();
                }

            }
            ImGui::PopStyleColor(1);

            //Output files
            for (int i = 0; i < subfiles.size(); i++)
            {
                if( (!subfiles[i].is_hidden | show_hidden) && filter.PassFilter(subfiles[i].name.c_str()))
                {
                    filtered_items++;
                    if(ImGui::Selectable(subfiles[i].name.c_str(), selected_idx == i && !is_dir, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        selected_idx = i;
                        is_dir = false;
                        if(ImGui::IsMouseDoubleClicked(0))
                            selected_fn = current_path + subfiles[i].name;
                    }
                    if( (filtered_items) % col_items_limit == 0)
                        ImGui::NextColumn();
                }
            }
            ImGui::Columns(1);
            ImGui::EndChild();

            //Draw Remaining UI elements
            ImGui::SetCursorPosY(sz_xy.y);
            ImGui::Checkbox("Show Hidden Files and Folders", &show_hidden);

            ImGui::SetCursorPos(ImVec2(sz_xy.x - 110, sz_xy.y));
            if (ImGui::Button("Open", ImVec2(50, 0)))
            {
                if(selected_idx >= 0)
                {
                    if(is_dir)
                       show_error |= !(onContentClick(selected_idx, show_drives));
                    else
                        selected_fn = current_path + subfiles[selected_idx].name;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(50, 0)) || !selected_fn.empty())
                ImGui::CloseCurrentPopup();

            //Show Error Modal if there was an error opening any directory
            if(show_error)
                ImGui::OpenPopup("Error");
            showErrorModal();

            ImGui::EndPopup();
        }
        return selected_fn;
    }

    bool ImGuiFileBrowser::onNavigationButtonClick(int idx)
    {
        std::string new_path(current_path);

        #ifdef OSWIN
        //First Button corresponds to virtual folder Computer which lists all logical drives (hard disks and removables)
        if(idx == 0)
        {
            if(!loadWindowsDrives())
                return false;
            current_path.clear();
            current_dirlist.erase(current_dirlist.begin()+idx+1, current_dirlist.end());
            return true;
        }

        //Clicked on a drive letter?
        if(idx == 1)
            new_path = current_path.substr(0, 3);
        else
            new_path = current_path.substr(0, current_path.find("/" + current_dirlist[idx]) + current_dirlist[idx].length() + 2 );

        #else
        //find returns 0 based indices and substr takes length of chars thus + 1. Another +1 to include trailing "/"
        new_path = current_path.substr(0, current_path.find("/" + current_dirlist[idx]) + current_dirlist[idx].length() + 2 );
        #endif

        if(readDIR(new_path))
        {
            current_dirlist.erase(current_dirlist.begin()+idx+1, current_dirlist.end());
            current_path = new_path;
        }
        else
            return false;
        return true;
    }

    bool ImGuiFileBrowser::onContentClick(int idx, bool show_drives)
    {
        std::string name = subdirs[idx].name;
        std::string new_path(current_path);
        if(name == "..")
        {
            new_path.pop_back(); //Remove trailing "/"
            new_path = new_path.substr(0, new_path.find_last_of("/")+1);
        }
        else
        {
            #ifdef OSWIN
            if(show_drives)
            {
                //Remember we displayed drives as *Local/Removable Disk: X* hence we need last char only
                name = std::string(1, name.back()) + ":";
                new_path += name + "/";
            }
            else
                new_path += name + "/";
            #else
            new_path += name + "/";
            #endif // OSWIN
        }

        if(readDIR(new_path))
        {
            if(name == "..")
                current_dirlist.pop_back();
             else
                current_dirlist.push_back(name);
             current_path = new_path;
        }
        else
           return false;
        return true;
    }

    void ImGuiFileBrowser::showErrorModal()
    {
        ImVec2 window_size(250, 80);
        ImGui::SetNextWindowSize(window_size);
        if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
        {
            float frame_height = ImGui::GetFrameHeightWithSpacing();

            std::string text = "Error Opening Directory!";
            ImGui::SetCursorPosX(window_size.x/2.0 - text.length() * 3.3);
            ImGui::Text(text.c_str());

            ImGui::Separator();
            ImGui::SetCursorPos(ImVec2(window_size.x/2.0 - 25, window_size.y - frame_height));
            if (ImGui::Button("Ok", ImVec2(50, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }

    bool ImGuiFileBrowser::readDIR(std::string pathdir)
    {
        DIR* dir;
        struct dirent *ent;
        if ((dir = opendir (pathdir.c_str())) != NULL)
        {
            #ifdef OSWIN
            // If we are on Windows and failed to load Windows Drives, populate filebar with default path
            if(current_dirlist.empty() && current_path == "./")
            {
                const wchar_t* absolute_path = dir->wdirp->patt;
                std::string current_directory = wStringToString(absolute_path);
                std::replace(current_directory.begin(), current_directory.end(), '\\', '/');

                //Remove trailing "/*" returned by ** dir->wdirp->patt **  Don't know if this exists in UNIX based OS
                current_path = current_directory.substr(0, current_directory.length()-2);

                //Create a vector of each directory in the file path for the filepath bar. Not Necessary for linux as starting directory is "/"
                parsePathTabs(current_path);
            }
            #endif // OSWIN

            // store all the files and directories within directory
            subdirs.clear();
            subfiles.clear();
            selected_idx = -1;

            while ((ent = readdir (dir)) != NULL)
            {
                bool is_hidden = false;
                //Ignore current directory
                if((ent->d_name[0] == '.' && ent->d_namlen == 1))
                    continue;
                if( !(ent->d_namlen == 2 && ent->d_name[0] == '.' && ent->d_name[1] == '.') )
                {

                    #ifdef OSWIN
                    std::string dir = pathdir + std::string(ent->d_name);
                    // IF system file skip it...
                    if (FILE_ATTRIBUTE_SYSTEM & GetFileAttributesA(dir.c_str()))
                        continue;
                    if (FILE_ATTRIBUTE_HIDDEN & GetFileAttributesA(dir.c_str()))
                        is_hidden = true;
                    #else
                    if((ent->d_name[0] == '.' && ent->d_namlen > 1))
                        is_hidden = true;
                    #endif // OSWIN
                }
                //Store directories and files in separate vectors
                if(ent->d_type == DT_DIR)
                    subdirs.push_back({std::string(ent->d_name), is_hidden});
                else if(ent->d_type == DT_REG)
                    subfiles.push_back({std::string(ent->d_name), is_hidden});
            }
            closedir (dir);
        }
        else
        {
            std::cout << "Could not open dir" << std::endl;
            return false;
        }
        return true;
    }

    void ImGuiFileBrowser::parsePathTabs(std::string path)
    {
        std::string path_element = "";
        std::string root = "";

        current_dirlist.push_back("Computer");
        current_dirlist.push_back(path.substr(0,2));

        // Erase the Drive letter and colon in path "X:/abc/xyz/etc...."
        path.erase(0,2);
        std::istringstream iss(path);
        while(std::getline(iss, path_element, '/'))
        {
            if(!path_element.empty())
                current_dirlist.push_back(path_element);
        }
    }

    bool ImGuiFileBrowser::loadWindowsDrives()
    {
        subdirs.clear();
        subfiles.clear();
        DWORD len = GetLogicalDriveStringsA(0,NULL);
        char drives[len];
        if(!GetLogicalDriveStringsA(len,drives))
            return false;

        char* temp = drives;
        for(char *drv = NULL; *temp != NULL; temp++)
        {
            drv = temp;
            if(DRIVE_REMOVABLE == GetDriveTypeA(drv))
                subdirs.push_back({"Removable Disk: " + std::string(1,drv[0]), false});
            else if(DRIVE_FIXED == GetDriveTypeA(drv))
                subdirs.push_back({"Local Disk: " + std::string(1,drv[0]), false});
            //Go to null character
            while(*(++temp));
        }
        return true;
    }

    std::string ImGuiFileBrowser::wStringToString(const wchar_t* wchar_arr)
    {
        std::mbstate_t state = std::mbstate_t();
         //MinGW bug (patched in mingw-w64), wcsrtombs doesn't ignore length parameter when dest = null. Hence the large number.
        size_t len = 1 + std::wcsrtombs(NULL, &(wchar_arr), 600000, &state);

        char char_arr[len];
        std::wcsrtombs(char_arr, &wchar_arr, len, &state);
        return std::string(char_arr);

    }
}

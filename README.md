![BUILD STATUS](https://github.com/gallickgunner/ImGui-Addons/actions/workflows/ci.yml/badge.svg)
![Dear ImGui version](https://img.shields.io/badge/Dear%20ImGui%20Updated-v1.92.8-blue)

# ImGui-Addons

Addon widgets for GUI library Dear ImGui.

## File Dialog

A simple cross-platform file dialog that supports opening files, saving files and selecting directories.

## Features

* 3 different modes are supported currently. `OPEN` for opening files, `SAVE` for saving files and `SELECT` for selecting a directory.

* Unicode/UTF-8 aware. Filenames and paths are preserved as UTF-8. Unicode Characters supported by the fonts configured in Dear ImGui can be displayed. Complex-script shaping and bidirectional text rendering are outside the scope of the file dialog.

* The GUI greatly resembles the Windows File Dialog.

* Written in C++11 for compatibility with older code bases.

## Working

Code uses a modified version of Toni's ported [dirent](https://github.com/tronkko/dirent) interface on Windows. On Unix-like systems, the code uses the native `dirent.h` header.

The modified Windows `dirent` implementation treats its narrow-string interface as UTF-8. Windows wide-character filenames are correctly converted between UTF-16 and UTF-8 via the native `MultiByteToWideChar` and `WideCharToMultiByte` API calls.

The file browser considers all paths and filenames returned by `ImGuiFileBrowser` as UTF-8. This differs from the original ported `dirent` implementation, whose narrow-character interface normally follows the active Windows code page.

You will nevertheless need an appropriate font for your console and Dear ImGui to view Unicode characters supported by that font. Complex-script shaping and bidirectional text rendering are outside the scope of the file dialog itself. Check the preview folder and the `CMakeLists.txt` for an example.

Thanks to [@Sandy](https://github.com/bwrsandman), the code was tested on Linux and runs fine.

### Unicode Notes

The file dialog guarantees UTF-8 filename and path preservation, but this does not imply full Unicode linguistic processing.

* Windows filesystem paths are converted between UTF-8 and UTF-16 at the Win32 API boundary.
* Sorting is not locale-aware and is very basic.
* Case-insensitive sorting and filtering are primarily ASCII-oriented.
* Unicode normalization is not performed.
* Complex-script shaping and bidirectional text rendering are not handled by this file dialog.

For example, Arabic or Urdu filenames can be preserved correctly as UTF-8, but correct visual shaping and right-to-left rendering depend on the text-rendering setup used with Dear ImGui.


### Usage

Addon is designed for drop-in style use. Make sure all the ImGui files are accessible as `imgui.h` and so on without specifying the folder they are in. You must mention the path to the folder they are in in your IDE. Also make sure `dirent.h` is accessible as `Dirent/dirent.h`. If you don't like these include paths, you can change how `ImGuiFileBrowser.h` and the corresponding cpp file access these yourself. Now include `ImGuiFileBrowser.h` and use it like this..

```cpp
..
imgui_addons::ImGuiFileBrowser file_dialog; // As a class member or globally

// Now inside any function
void showMainMenu()
{
    bool open = false, save = false;
    if(ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            if (ImGui::MenuItem("Open", NULL))
                open = true;
            if (ImGui::MenuItem("Save", NULL))
                save = true;
            
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    //Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
    if(open)
        ImGui::OpenPopup("Open File");
    if(save)
        ImGui::OpenPopup("Save File");
        
    /* Optional fourth parameter. Support opening only compressed rar/zip/7z files. 
     * Opening any other file will show error, return false and won't close the dialog.
     */
    if(file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".rar,.zip,.7z"))
    {
        std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
        std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
    }
    if(file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".png,.jpg,.bmp"))
    {
        std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
        std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
        std::cout << file_dialog.ext << std::endl;              // Access ext separately
        //Do writing of files based on extension here
    }
}
```

Note that the extension list provided to the `showFileDialog` function must be comma separated without any spaces and must include the `.` character before each extension.

In the `SAVE` mode, if the user types a valid extension from the given list, for e.g. `.jpg` in the example above, inside the input file name region and saves, then the selected extension in the extension drop down box is ignored.

Enough chitchat, here's a gif in-action, click for full video (yes... you saw it right.. those are all anime wallpapers.)

<img width="1153" height="705" alt="image" src="https://github.com/user-attachments/assets/eee0cce2-9bde-4ea7-b044-83ce66cfff60" />

[![Demo](https://i.imgur.com/kNOeYme.gif)](https://www.youtube.com/watch?v=cPyfgYFdiy0)

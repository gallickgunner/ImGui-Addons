[![Build Status](https://travis-ci.com/gallickgunner/ImGui-Addons.svg?branch=master)](https://travis-ci.com/gallickgunner/ImGui-Addons)

# ImGui-Addons
Addon widgets for GUI library Dear ImGui.

## File Dialog
A simple cross-platform file dialog that uses [dirent](https://github.com/tronkko/dirent) interface for reading directories and files. It's present as a standard header file in Unix based systems afaik and is also contained in some compilers like MinGW but I have decided to use Toni Rönkkö's ported version so atleast the code remains compiler independent. Both Open and Save file dialogs are supported.

Code uses ported `dirent.h` provided by Toni on Windows and on UNIX code uses standard `dirent.h` header, but unfortunately the code hasn't been checked extensively on UNIX based systems especially MacOS. So if someone finds problems on other platforms do tell me or submit a pull request. Also I don't think the code will work with Unicode paths containing language specific or other special characters as I blatantly use normal `char*` and `std::string` everywhere. The ported `dirent.h` for Windows uses `wcstombs` function to convert widechars to multibyte sequences but according to the docs this also fails if a wide character that doesnt' correspond to a valid Mulitbyte char is encountered. Anyways Not an expert at this topic so you may find errors if your paths contain special characters outside the normal 0-255 range.

Thanks to [@Sandy](https://github.com/bwrsandman), the code was tested on linux and runs fine except except that double clicks don't work all the time. This might be due to problems in ImGui itself or a problem on a specific computer only. So if anybody else encounters any issues do tell me.

### Usage
Copy all the necessary ImGui files in one folder and include it's path in your IDE. Also include the path to `Dirent` folder so `dirent.h` is accessible as `Dirent/dirent.h`. Now include `ImGuiFileBrowser.h` and use it like this..
```
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
	    
	 /* Optional third parameter. Support opening only compressed rar/zip files. 
          * Opening any other file will show error, return false and won't close the dialog.
          */
	if(file_dialog.showOpenFileDialog("Open File", ImVec2(700, 310), ".rar,.zip,.7z"))
	    std::cout << file_dialog.selected_fn << std::endl;
	if(file_dialog.showSaveFileDialog("Save File", ImVec2(700, 310), ".png,.jpg,.bmp"))
        {
	    std::cout << file_dialog.selected_fn << std::endl; // Absolute path to file with extension
	    std::cout << file_dialog.ext << std::endl; 	       // Access ext separately
	    //Do writing of files based on extension here
        }
}
```
I've also added the modified `imgui_demo.cpp` to include the file dialog in the menu bar so you can check how it's working there.

Enough chitchat, here's a gif in-action, click for full video (I hope you guys don't consider me a weeb after seeing the screensavers collection xD )

[![Demo](https://i.imgur.com/HcwKNmi.gif)](https://www.youtube.com/watch?v=cPyfgYFdiy0)




# ImGui-Addons
Addon widgets for GUI library Dear ImGui.

## File Dialog
A simple file dialog that uses [dirent](https://github.com/tronkko/dirent) interface for reading directories and files. It's present as a standard header file in Unix based systems afaik and is also contained in some compilers like MinGW but I have decided to use Toni Rönkkö's ported version so atleast the code remains compiler independent. 

I have tried my best to write platform independent code, on UNIX code uses standard `dirent.h` header, but unfortunately the code hasn't been checked for UNIX based systems. If someone is willing to help me test it on Linux/Mac I'd be grateful. Also I don't think the code will work with Unicode paths containing language specific or other special characters as I blatantly use normal `char*` and `std::string` everywhere. The ported `dirent.h` for Windows uses `wcstombs` function to convert widechars to multibyte sequences but according to the docs this also fails if a wide character that doesnt' correspond to a valid Mulitbyte char is encountered. Anyways Not an expert at this topic so you may find errors if your paths contain special characters outside the normal 0-255 range.

Enough chitchat, here's a gif in-action, click for full video (I hope you guys don't consider me a weeb after seeing the screensavers collection xD )

[![Demo](https://i.imgur.com/HcwKNmi.gif)](https://www.youtube.com/watch?v=cPyfgYFdiy0)


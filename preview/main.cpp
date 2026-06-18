#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ImGuiFileBrowser.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

static imgui_addons::ImGuiFileBrowser file_dialog;

static std::string g_last_open_filename;
static std::string g_last_open_path;

static std::string g_last_save_filename;
static std::string g_last_save_path;
static std::string g_last_save_ext;

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

static void showMainMenu()
{
    bool open = false;
    bool save = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            if (ImGui::MenuItem("Open", nullptr))
                open = true;

            if (ImGui::MenuItem("Save", nullptr))
                save = true;

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // The popup name passed to ImGui::OpenPopup() and showFileDialog()
    // must be exactly the same.
    if (open)
        ImGui::OpenPopup("Open File");

    if (save)
        ImGui::OpenPopup("Save File");

    /*
     * Optional fourth parameter:
     * Support opening only compressed rar/zip/7z files.
     * Opening any other file should show an error, return false,
     * and keep the dialog open, depending on addon behavior.
     */
    if (file_dialog.showFileDialog(
            "Open File",
            imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
            ImVec2(700, 310),
            ".rar,.zip,.7z"))
    {
        g_last_open_filename = file_dialog.selected_fn;
        g_last_open_path = file_dialog.selected_path;

        std::cout << "Open filename: " << file_dialog.selected_fn << std::endl;
        std::cout << "Open path: " << file_dialog.selected_path << std::endl;
    }

    if (file_dialog.showFileDialog(
            "Save File",
            imgui_addons::ImGuiFileBrowser::DialogMode::SAVE,
            ImVec2(700, 310),
            ".png,.jpg,.bmp"))
    {
        g_last_save_filename = file_dialog.selected_fn;
        g_last_save_path = file_dialog.selected_path;
        g_last_save_ext = file_dialog.ext;

        std::cout << "Save filename: " << file_dialog.selected_fn << std::endl;
        std::cout << "Save path: " << file_dialog.selected_path << std::endl;
        std::cout << "Save extension: " << file_dialog.ext << std::endl;

        // Do writing of files based on extension here.
    }
}

static void showPreviewWindow()
{
    ImGui::Begin("ImGui FileBrowser Addon Preview");

    ImGui::TextWrapped("Use Menu -> Open or Menu -> Save to test the file browser addon.");

    ImGui::Separator();

    ImGui::Text("Last Open Selection:");
    ImGui::Text("Filename: %s", g_last_open_filename.empty() ? "<none>" : g_last_open_filename.c_str());
    ImGui::TextWrapped("Path: %s", g_last_open_path.empty() ? "<none>" : g_last_open_path.c_str());

    ImGui::Separator();

    ImGui::Text("Last Save Selection:");
    ImGui::Text("Filename: %s", g_last_save_filename.empty() ? "<none>" : g_last_save_filename.c_str());
    ImGui::TextWrapped("Path: %s", g_last_save_path.empty() ? "<none>" : g_last_save_path.c_str());
    ImGui::Text("Extension: %s", g_last_save_ext.empty() ? "<none>" : g_last_save_ext.c_str());

    ImGui::End();
}

int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        return 1;

#if defined(__APPLE__)
    const char* glsl_version = "#version 150";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    GLFWwindow* window = glfwCreateWindow(
        1280,
        720,
        "ImGui-Addons FileBrowser Preview",
        nullptr,
        nullptr
    );

    if (window == nullptr)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        std::cerr << "Failed to initialize ImGui GLFW backend." << std::endl;
        return 1;
    }

    if (!ImGui_ImplOpenGL3_Init(glsl_version))
    {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend." << std::endl;
        return 1;
    }

    ImVec4 clear_color = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        showMainMenu();
        showPreviewWindow();

        ImGui::Render();

        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);
        glClearColor(
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w
        );
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
#include <cmath>
#include <glad/glad.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <print>


const float EPS = 1e-5;

bool IsZeroLen(ImVec2 v) {
    return (v.x * v.x + v.y * v.y) < EPS * EPS;
}

class TestApp {
public:
    TestApp(GLFWwindow* window) : window_(window) {}

    void Draw() {
    }

    void Update(float dt) {
        static bool is_button_pressed = false;

        ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Once);
        ImGui::SetNextWindowPos(win_pos_, ImGuiCond_Once);


        ImGui::Begin("Test window");
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        // auto cursor_pos = ImGui::GetCursorPos();
        ImVec2 window_pos = ImGui::GetWindowPos();       // Top-left corner of the window
        ImVec2 window_padding = ImGui::GetStyle().WindowPadding;
        ImVec2 content_pos = window_pos + window_padding;
        draw_list->PushClipRectFullScreen();
        draw_list->AddCircleFilled({content_pos.x + 200.f, content_pos.y + 100.f}, 50.f, IM_COL32(255, 0, 0, 255), 362);
        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2{-50.f, 50.f});
        if(ImGui::Button("Press and Hold")) {
            std::println("Button changed state to pressed");
            is_button_pressed = true;
        }
        // Check if the button remains active
        if (ImGui::IsItemActive() && ImGui::IsMouseDown(0))
        {
            std::println("Button changed state to pressed");
            is_button_pressed = true; // Keep the button pressed if holding down
        }
        else if (!ImGui::IsMouseDown(0))
        {
            std::println("Button changed state to released");
            is_button_pressed = false; // Reset when the mouse is released
        }
        (void)is_button_pressed;
        draw_list->PopClipRect();


        auto dir = GetDir();
        if(!IsZeroLen(dir)) {
            win_pos_.x += dir.x * speed_ * dt;
            win_pos_.y += dir.y * speed_ * dt;
        } else {
            win_pos_ = ImGui::GetWindowPos();
        }

        ImGui::SetWindowPos(win_pos_, ImGuiCond_Always);

        ImGui::End();

        ImGui::SetNextWindowPos(win_pos_ + ImVec2{300.f, 200.f}, ImGuiCond_Once);
        ImGui::Begin("Test window 2");
        ImGui::End();
    }

private:
    ImVec2 GetDir() {
        ImVec2 dir;
        if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
           dir.x += 1.0;
        }
        if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
            dir.x -= 1.0;
        }
        if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS) {
            dir.y += 1.0;
        }
        if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS) {
            dir.y -= 1.0;
        }
        float len = sqrt(dir.x * dir.x + dir.y * dir.y);
        if(len > EPS) {
            dir.x /= len;
            dir.y /= len;
        }
        return dir;
    }

private:
    GLFWwindow* window_;
    ImVec2 win_pos_{100, 100};
    float speed_{1.f};
};

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "ImGui Demo", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize OpenGL loader (depending on your setup)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Set up Dear ImGui style
    ImGui::StyleColorsDark();

    TestApp app(window);

    // Initialize ImGui for GLFW and OpenGL3
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // Adjust GLSL version as needed

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw interface
        app.Update(1.f);
        app.Draw();
        // ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

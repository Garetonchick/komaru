#include <cmath>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>


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
        ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Once);
        ImGui::SetNextWindowPos(win_pos_, ImGuiCond_Once);

        ImGui::Begin("Test window");

        auto dir = GetDir();
        if(!IsZeroLen(dir)) {
            win_pos_.x += dir.x * speed_ * dt;
            win_pos_.y += dir.y * speed_ * dt;
        } else {
            win_pos_ = ImGui::GetWindowPos();
        }

        ImGui::SetWindowPos(win_pos_, ImGuiCond_Always);

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

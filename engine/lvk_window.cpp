#include "lvk_window.hpp"

#include <utility>
#include <stdexcept>
namespace lvk {
    LvkWindow::LvkWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{std::move(name)} {
     initWindow();
    }

    LvkWindow::~LvkWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    void LvkWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

    }
    void LvkWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
        if (glfwCreateWindowSurface(instance, window, nullptr, surface)!= VK_SUCCESS){
            throw std::runtime_error("failed to create a window");
        }
    };
}
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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }
    void LvkWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
        if (glfwCreateWindowSurface(instance, window, nullptr, surface)!= VK_SUCCESS){
            throw std::runtime_error("failed to create a window");
        }
    }
    void LvkWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto lvkWindow = reinterpret_cast<LvkWindow *>(glfwGetWindowUserPointer(window));
        lvkWindow->framebufferResized = true;
        lvkWindow->width = width;
        lvkWindow->height = height;
    };
}
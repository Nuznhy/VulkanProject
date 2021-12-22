#pragma once

#include "lvk_window.hpp"
#include "lvk_device.hpp"
#include "lvk_renderer.hpp"
#include "lvk_model.hpp"
#include "lvk_game_object.hpp"
#include "lvk_descriptors.hpp"
//std
#include <memory>
#include <vector>
namespace lvk {
    class App {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App();
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;
        void run();
    private:
        void loadGameObjects();


        LvkWindow lvkWindow{WIDTH, HEIGHT, "First app"};
        LvkDevice lvkDevice{lvkWindow};
        LvkRenderer lvkRenderer{lvkWindow, lvkDevice};

        std::unique_ptr<LvkDescriptorPool> globalPool{};
        std::vector<LvkGameObject> gameObjects;
    };
}
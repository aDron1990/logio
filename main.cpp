#include "imgui_theme.hpp"

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui-SFML.h>
#include <cmrc/cmrc.hpp>

#include <print>

CMRC_DECLARE(res);

auto fs = cmrc::res::get_filesystem();

int main(int argc, char* argv[])
{
    auto videoMode = sf::VideoMode{800, 500};
    auto window = sf::RenderWindow{videoMode, "sfml"};
    window.setVerticalSyncEnabled(true);

    IMGUI_CHECKVERSION();
    ImGui::SFML::Init(window);

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("ubuntu.ttf", 16);
    ImGui::SFML::UpdateFontTexture();
    setImguiTheme();

    auto owlFile = fs.open("resources/images/owl.jpg");

    auto texture = sf::Texture{};
    texture.loadFromMemory(owlFile.begin(), owlFile.size());

    auto owl = sf::Sprite{texture};
    owl.setPosition({300, 0});
    float fcolor[] = {1.0f, 1.0f, 1.0f, 1.0f};

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        auto event = sf::Event{};
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) 
                window.close();
        }
        ImGui::SFML::Update(window, deltaClock.restart());
        
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({300, 500});
        ImGui::Begin("color", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
        ImGui::ColorPicker4("", fcolor);
        unsigned char r = fcolor[0] * 255;
        unsigned char g = fcolor[1] * 255;
        unsigned char b = fcolor[2] * 255;
        unsigned char a = fcolor[3] * 255;
        auto color = sf::Color{r, g, b ,a};

        owl.setColor(color);
        ImGui::End();
        
        window.clear();
        window.draw(owl);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
	return 0;
}
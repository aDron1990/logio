#include "ui.hpp"
#include "imgui_theme.hpp"
#include "defines.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui-SFML.h>

#include <ranges>

UI::UI(sf::RenderWindow& window, cmrc::file fontBin) 
{
    IMGUI_CHECKVERSION();
    ImGui::SFML::Init(window);
    auto& io = ImGui::GetIO();
    auto fontCfg = ImFontConfig{};
    fontCfg.FontDataOwnedByAtlas = false;
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryTTF((void*)fontBin.begin(), fontBin.size(), 18, &fontCfg);
    ImGui::SFML::UpdateFontTexture();
    setImguiTheme();
}

UI::~UI()
{
    ImGui::SFML::Shutdown();
}

void UI::processEvent(sf::RenderWindow& window, sf::Event& event)
{
    ImGui::SFML::ProcessEvent(window, event);
}

void UI::beginDraw(sf::RenderWindow& window, sf::Time deltaTime)
{
    ImGui::SFML::Update(window, deltaTime);
}

void UI::endDraw(sf::RenderWindow& window)
{
    ImGui::SFML::Render(window);
}

void UI::drawSidebar(std::vector<std::unique_ptr<Element>>& elementTypes, std::atomic_int& currentId) 
{
    const float BUTTON_SIZE = 75.0f;
    const sf::Color ACTIVE_ELEMENT_BG{0xA0A0A0FF};
    auto padding = ImGui::GetStyle().WindowPadding;
    auto spacing = ImGui::GetStyle().ItemSpacing;
    const auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

    auto size = ImVec2{};
    size.y = sf::VideoMode::getDesktopMode().height;
    size.x = BUTTON_SIZE + padding.x * 2;
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::Begin("sprites", nullptr, flags);

    for (auto element : elementTypes | std::views::enumerate)
    {
        auto cursor = ImGui::GetCursorPos();
        ImGui::PushID(std::get<0>(element));
        if (ImGui::InvisibleButton("spriteButton", {BUTTON_SIZE, BUTTON_SIZE}))
        {
            currentId = std::get<0>(element);
        }
        ImGui::PopID();
        ImGui::SetCursorPos(cursor);
        if (currentId == std::get<0>(element)) ImGui::DrawRectFilled({0, 0, BUTTON_SIZE, BUTTON_SIZE}, ACTIVE_ELEMENT_BG);
        auto sprite = std::get<1>(element)->getDefaultSprite();
        auto scale = sprite.getScale();
        scale = {(float)scale.x / SPRITE_SIZE, (float)scale.y / SPRITE_SIZE};
        sprite.setScale(scale * BUTTON_SIZE);
        ImGui::Image(sprite);
        ImGui::SetCursorPos({cursor.x, cursor.y + BUTTON_SIZE + spacing.y});
    }

    ImGui::End();
}

void UI::drawMenu(std::atomic_bool& running, 
    std::function<void(const std::filesystem::path& path)> onSave, 
    std::function<bool(const std::filesystem::path& path)> onLoad,
    std::function<void()> onNew)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) ImGui::OpenPopup("menu");

    const auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;
    const auto padding = ImGui::GetStyle().WindowPadding;
    const auto windowSize = ImVec2{300, 500};
    const auto buttonSize = ImVec2{-FLT_MIN, 0.0f};

    bool open = m_inMenu;
    if (ImGui::BeginPopupModal("menu", &open, flags))
    {
        ImGui::SetWindowSize(windowSize);
        if (ImGui::Button("Continue", buttonSize)) ImGui::CloseCurrentPopup();
        if (ImGui::Button("New", buttonSize))
        {
            onNew();
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Save", buttonSize)) ImGui::OpenPopup("save");
        if (ImGui::Button("Load", buttonSize)) ImGui::OpenPopup("load");
        if (ImGui::Button("Exit", buttonSize)) running = false;    
        if (ImGui::BeginPopupModal("load", nullptr, flags))
        {
            ImGui::SetWindowSize(windowSize);
            auto currentItem = std::filesystem::path{};
            ImGui::BeginChild("Saves", {0.0f, -ImGui::GetFrameHeightWithSpacing()});
            if (!std::filesystem::exists("saves/")) std::filesystem::create_directory("saves/");
            for (auto pathAndId : std::filesystem::directory_iterator("saves/") | std::views::enumerate)
            {
                if (!std::get<1>(pathAndId).is_regular_file()) continue;
                auto path = std::get<1>(pathAndId).path();
                auto filename = path.filename().string();
                auto select = false;
                if (ImGui::Selectable(filename.c_str(), &select))
                {
                    if (onLoad(path))
                    {
                        ImGui::CloseCurrentPopup();
                        open = false;
                    }
                }
            }
            ImGui::EndChild();
            if (ImGui::Button("Back", {-FLT_MIN, 0.0f})) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupModal("save", nullptr, flags))
        {
            ImGui::SetWindowSize(windowSize);
            using namespace std::literals;
            auto currentItem = std::filesystem::path{};
            ImGui::BeginChild("Saves", {0.0f, -(ImGui::GetFrameHeightWithSpacing() * 3)});
            if (!std::filesystem::exists("saves/")) std::filesystem::create_directory("saves/");
            for (auto pathAndId : std::filesystem::directory_iterator("saves/") | std::views::enumerate)
            {
                if (!std::get<1>(pathAndId).is_regular_file()) continue;
                auto path = std::get<1>(pathAndId).path();
                auto filename = path.filename().string();
                auto select = false;
                if (ImGui::Selectable(filename.c_str(), &select))
                {
                    m_saveName = filename;
                }
            }
            ImGui::EndChild();
            ImGui::PushItemWidth(-FLT_MIN);
            ImGui::InputText("##", &m_saveName);
            ImGui::PopItemWidth();
            m_savePath = "saves/"s + m_saveName;
            if (ImGui::Button("Save", {-FLT_MIN, 0.0f})) onSave(m_savePath);
            if (ImGui::Button("Back", {-FLT_MIN, 0.0f})) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        ImGui::EndPopup();
    }
    m_inMenu = open;
}

void UI::commandMenu() 
{
    m_inMenu = !m_inMenu;
}

bool UI::isInMenu() 
{
    return m_inMenu;
}
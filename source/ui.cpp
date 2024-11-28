#include "ui.hpp"
#include "imgui_theme.hpp"
#include "defines.hpp"

#include <imgui.h>
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

void UI::drawMenu(std::atomic_bool& running)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) ImGui::OpenPopup("menu");

    const auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;
    const auto padding = ImGui::GetStyle().WindowPadding;
    const auto windowSize = ImVec2{300, 500};
    const auto buttonSize = ImVec2{windowSize.x - padding.x * 2, 30};

    bool open = m_inMenu;
    if (ImGui::BeginPopupModal("menu", &open, flags))
    {
        ImGui::SetWindowSize(windowSize);
        if (ImGui::Button("Continue", buttonSize)) ImGui::CloseCurrentPopup();
        if (ImGui::Button("Exit", buttonSize)) running = false;
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
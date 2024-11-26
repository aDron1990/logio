cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(logio)

find_package(CMakeRC CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

add_executable(logio 
	source/main.cpp
	source/imgui_theme.hpp
	source/game.cpp
	source/game.hpp
	source/grid.hpp
	source/field.cpp
	source/field.hpp
)

cmrc_add_resource_library(res
	ALIAS logio::res
	NAMESPACE res
	resources/images/atlas.png
	resources/fonts/ubuntu.ttf
)

target_link_libraries(logio PRIVATE
	logio::res
	sfml-system 
	sfml-network 
	sfml-graphics 
	sfml-window
	imgui::imgui
	ImGui-SFML::ImGui-SFML
)



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
	source/defines.hpp
	source/imgui_theme.hpp
	source/ui.cpp
	source/ui.hpp
	source/game.cpp
	source/game.hpp
	source/grid.hpp
	source/field.cpp
	source/field.hpp
	source/rotation.hpp
	source/element_data.hpp
	source/element.hpp

	source/elements/wire.cpp
	source/elements/wire.hpp
	source/elements/jump.cpp
	source/elements/jump.hpp
	source/elements/not.cpp
	source/elements/not.hpp
	source/elements/and.cpp
	source/elements/and.hpp
	source/elements/tree.cpp
	source/elements/tree.hpp
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

target_include_directories(logio PRIVATE source/)

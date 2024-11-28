cmake_minimum_required(VERSION 3.16)
project(water-quality-qt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts)
qt_standard_project_setup()

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/examplepage.cpp
    src/dashboard.cpp
)

add_executable(water-quality-qt
    ${PROJECT_SOURCES}
)

target_link_libraries(water-quality-qt PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
    
)
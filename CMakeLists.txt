cmake_minimum_required(VERSION 3.16)
project(water-quality-qt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core LinguistTools Gui Widgets Charts)
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES de fr es)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/PollutantOverview.cpp
    src/pop.cpp
    src/Compound.cpp
    src/LitterIndicators.cpp
    src/dashboard.cpp
    src/data/window.cpp
    src/data/stats.cpp
    src/data/quake.cpp
    src/data/model.cpp
    src/data/dataset.cpp
    src/data/data_processors.cpp
    src/data/config.cpp
)

add_executable(water-quality-qt
    ${PROJECT_SOURCES}
)

qt_add_translations(water-quality-qt)


target_link_libraries(water-quality-qt PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
    
)
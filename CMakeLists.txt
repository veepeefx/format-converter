cmake_minimum_required(VERSION 4.0)
project(format-converter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(format-converter src/main.cpp
        src/MainWindow.cpp
        src/MainWindow.h
        src/CommonEnums.h)

target_link_libraries(format-converter
        Qt::Core
        Qt::Gui
        Qt::Widgets
)


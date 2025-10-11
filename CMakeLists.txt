cmake_minimum_required(VERSION 3.5)
project(Discard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_AUTORCC ON)
set(app_icon "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Svg Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Svg Sql)
set(BOOST_ROOT "C:/boost_1_88_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}")
set(
    styles/dark.qss
    utils.h
)
# Your normal sources and UI files:
# Grab every .cpp and .h under core/, mainwindow/, pages/, etc.
file(GLOB_RECURSE CPP_SOURCES
    "${CMAKE_SOURCE_DIR}/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/mainwindow/*.cpp"
    "${CMAKE_SOURCE_DIR}/pages/*.cpp"
    "${CMAKE_SOURCE_DIR}/assets/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/core/*.h"
    "${CMAKE_SOURCE_DIR}/mainwindow/*.h"
    "${CMAKE_SOURCE_DIR}/pages/*.h"
    "${CMAKE_SOURCE_DIR}/assets/*.h"
    "${CMAKE_SOURCE_DIR}/utils/*.h"
)

# Grab every .ui under mainwindow/ and pages/
file(GLOB_RECURSE UI_FILES
    "${CMAKE_SOURCE_DIR}/mainwindow/*.ui"
    "${CMAKE_SOURCE_DIR}/pages/*.ui"
)

# This will generate ui_*.h for each .ui
qt6_wrap_ui(UI_HEADERS ${UI_FILES})



find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg Multimedia MultimediaWidgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

    add_executable(Discard
        WIN32
        main.cpp
        styles/dark.qss
        resource.qrc
        ${CPP_SOURCES}
        ${HEADERS}
        ${UI_FILES}
        ${QT_RESOURCES}
        ${app_icon}
    )

target_link_libraries(Discard PRIVATE Qt6::Core Qt6::Gui Qt6::Svg Qt6::Widgets Qt6::Multimedia Qt6::MultimediaWidgets)
target_link_libraries(Discard PRIVATE Qt6::Widgets)


target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::Sql
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
      ${BOOST_INCLUDEDIR}
      ${CMAKE_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}/core
      ${CMAKE_SOURCE_DIR}/mainwindow
      ${CMAKE_SOURCE_DIR}/pages
      ${CMAKE_SOURCE_DIR}/widgets
      ${CMAKE_SOURCE_DIR}/utils
      ${CMAKE_SOURCE_DIR}/assets
      ${CMAKE_CURRENT_BINARY_DIR}
)

qt_finalize_executable(Discard)

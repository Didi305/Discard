cmake_minimum_required(VERSION 3.5)
project(Discard VERSION 0.1 LANGUAGES CXX)
include(GNUInstallDirs)


if(WIN32)
    set(CMAKE_INSTALL_BINDIR ".")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_AUTORCC ON)
set(app_icon "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Svg)
set(BOOST_ROOT "C:/boost_1_88_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}")
set(
    styles/dark.qss
    utils.h
)

file(GLOB_RECURSE CPP_SOURCES
    "${CMAKE_SOURCE_DIR}/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/mainwindow/*.cpp"
    "${CMAKE_SOURCE_DIR}/pages/*.cpp"
    "${CMAKE_SOURCE_DIR}/assets/*.cpp"
    "${CMAKE_SOURCE_DIR}/utils/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/core/*.h"

    "${CMAKE_SOURCE_DIR}/mainwindow/*.h"
    "${CMAKE_SOURCE_DIR}/pages/*.h"
    "${CMAKE_SOURCE_DIR}/assets/*.h"
    "${CMAKE_SOURCE_DIR}/utils/*.h"
)

list(REMOVE_ITEM CPP_SOURCES
    "${CMAKE_SOURCE_DIR}/core/chatserver.cpp"
)

list(REMOVE_ITEM HEADERS
    "${CMAKE_SOURCE_DIR}/core/chatserver.h"
    "${CMAKE_SOURCE_DIR}/core/serverprotocol.h"
)

# Grab every .ui under mainwindow/ and pages/
file(GLOB_RECURSE UI_FILES
    "${CMAKE_SOURCE_DIR}/mainwindow/*.ui"
    "${CMAKE_SOURCE_DIR}/pages/*.ui"
)

# CMAKE_AUTOUIC generates ui_*.h from these files. A second qt6_wrap_ui()
# rule for the same headers creates a dependency cycle with Ninja.



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

)

target_include_directories(${PROJECT_NAME}
    PRIVATE
      ${BOOST_INCLUDEDIR}
      ${CMAKE_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}/core
      ${CMAKE_SOURCE_DIR}/server
      ${CMAKE_SOURCE_DIR}/mainwindow
      ${CMAKE_SOURCE_DIR}/pages
      ${CMAKE_SOURCE_DIR}/widgets
      ${CMAKE_SOURCE_DIR}/utils
      ${CMAKE_SOURCE_DIR}/assets
      ${CMAKE_CURRENT_BINARY_DIR}
)
add_subdirectory(server)
if(WIN32)
    install(TARGETS Discard RUNTIME DESTINATION .)
    qt_generate_deploy_app_script(TARGET Discard OUTPUT_SCRIPT discard_deploy
        NO_TRANSLATIONS NO_UNSUPPORTED_PLATFORM_ERROR)
    install(SCRIPT ${discard_deploy})
endif()
qt_finalize_executable(Discard)

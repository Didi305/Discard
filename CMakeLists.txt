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

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Svg Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Svg Sql)
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
    Qt${QT_VERSION_MAJOR}::Sql
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
# ============================================================
# WINDOWS PORTABLE DEPLOYMENT
# ============================================================

if(WIN32)

    # PostgreSQL installation used by the Qt QPSQL driver.
    set(POSTGRESQL_BIN_DIR
        "C:/Program Files/PostgreSQL/17/bin"
        CACHE PATH
        "Path to PostgreSQL bin directory"
    )

    if(NOT EXISTS "${POSTGRESQL_BIN_DIR}/libpq.dll")
        message(FATAL_ERROR
            "libpq.dll was not found in: ${POSTGRESQL_BIN_DIR}"
        )
    endif()


    # --------------------------------------------------------
    # 1. Install Discard.exe
    # --------------------------------------------------------

    install(
        TARGETS Discard
        RUNTIME DESTINATION .
    )


    # --------------------------------------------------------
    # 2. Deploy Qt libraries, Qt plugins and MinGW runtime
    #
    # This replaces manually running windeployqt.
    # --------------------------------------------------------

    qt_generate_deploy_app_script(
        TARGET Discard
        OUTPUT_SCRIPT discard_qt_deploy_script
        NO_TRANSLATIONS
        NO_UNSUPPORTED_PLATFORM_ERROR
    )

    install(SCRIPT ${discard_qt_deploy_script})


    # --------------------------------------------------------
    # 3. Deploy PostgreSQL client runtime
    #
    # qsqlpsql.dll requires libpq.dll, which itself depends on
    # OpenSSL, libintl and other PostgreSQL runtime DLLs.
    #
    # Copy all PostgreSQL DLLs automatically so we don't have
    # to chase dependency chains manually.
    # --------------------------------------------------------

    file(
        GLOB POSTGRESQL_RUNTIME_DLLS
        CONFIGURE_DEPENDS
        "${POSTGRESQL_BIN_DIR}/*.dll"
    )

    install(
        FILES ${POSTGRESQL_RUNTIME_DLLS}
        DESTINATION .
    )

endif()
qt_finalize_executable(Discard)

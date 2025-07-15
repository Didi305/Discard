cmake_minimum_required(VERSION 3.5)
project(chat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(app_icon "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Svg Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Svg Sql)


# Your normal sources and UI files:
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    chatserver.cpp
    chatserver.h
    client.cpp
    client.h
    manager.cpp
    manager.h
    login.cpp
    login.h
    login.ui
    register.cpp
    register.h
    register.ui
    row.cpp
    row.h
    pendingrequestswidget.cpp
    pendingrequestswidget.h
    utils.h
    addfriendpage.cpp
    addfriendpage.h
    addfriendpage.ui
    outgoingpage.cpp
    outgoingpage.h
    outgoingpage.ui
    friends.cpp
    friends.h
    friends.ui
    styles/dark.qss
    resource.h
    directmessage.h
    directmessage.cpp
    directmessage.ui
)


if (WIN32)
    add_executable(chat
        WIN32
        ${PROJECT_SOURCES}
        ${QT_RESOURCES}
        ${app_icon}
    )
endif()

target_link_libraries(chat PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::Sql
)

qt_finalize_executable(chat)

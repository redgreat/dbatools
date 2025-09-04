cmake_minimum_required(VERSION 3.16)

project(dbatools VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        loginwindow.cpp
        loginwindow.h
        loginwindow.ui
        apimanager.cpp
        apimanager.h
        stringformatter.cpp
        stringformatter.h
        usermanager.cpp
        usermanager.h
        usereditor.cpp
        usereditor.h
        rolemanager.cpp
        rolemanager.h
        roleeditor.cpp
        roleeditor.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(dbatools
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(dbatools SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(dbatools
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(dbatools
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(dbatools)
endif()
cmake_minimum_required(VERSION 3.16)

project(dbatools VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# 启用Qt6的自动MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置源文件
set(SOURCES
    src/main.cpp
    src/loginwindow.cpp
    src/mainwindow.cpp
    src/stringformatter.cpp
    src/apimanager.cpp
)

# 设置头文件
set(HEADERS
    src/loginwindow.h
    src/mainwindow.h
    src/stringformatter.h
    src/apimanager.h
)

# 设置UI文件
set(UI_FILES
    ui/loginwindow.ui
    ui/mainwindow.ui
    ui/stringformatter.ui
)

# 创建可执行文件
add_executable(dbatools
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# 链接Qt6库
target_link_libraries(dbatools
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

# 设置包含目录
target_include_directories(dbatools PRIVATE
    src
    ui
)

# Windows特定设置
if(WIN32)
    set_target_properties(dbatools PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
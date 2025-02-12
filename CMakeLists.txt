cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(testpad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#find_package(Qt5 REQUIRED COMPONENTS Core Quick Gui Qml)

find_package(Qt6 6.8 COMPONENTS Core REQUIRED)
find_package(Qt6 6.8 COMPONENTS Quick REQUIRED)
find_package(Qt6 6.8 COMPONENTS QuickControls2 REQUIRED)

find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    message("OpenGL found.")
    message("OpenGL include dir: ${OPENGL_INCLUDE_DIR}")
    message("OpenGL link libraries: ${OPENGL_gl_LIBRARY}")
else (OPENGL_FOUND)
    message("OpenGL not found.")
endif()

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

add_executable(${PROJECT_NAME}
    qml.qrc
    application.h
    application.cpp
    cabstractmeshloader.cpp
	cabstractmeshloader.h
    cmesh.cpp
    cmesh.h
    cobjloader.cpp
    cobjloader.h
    cpatch.cpp
    cpatch.h
    cstlloader.cpp
    cstlloader.h
    defines.h
    enums.cpp
    enums.h
    fpscounter.h
    fpscounter.cpp
    graphicslayer.cpp
    graphicslayer.h
    main.cpp
    myrenderer.cpp
    myrenderer.h
    reactionwheel.h
    reactionwheel.cpp
    shadowmap.h
    shadowmap.cpp
    skybox.h
    skybox.cpp
    ssao.h
    ssao.cpp
    terrain.h
    terrain.cpp
    texturecombomodel.h
    texturecombomodel.cpp
    texturedplane.h
    texturedplane.cpp
    utils.cpp
    utils.h
    ui/widthmanager.h
    ui/widthmanager.cpp
    ui/craftcolumn.h
    ui/craftcolumn.cpp
 "defines.h")

# Link the Qt5 libraries to the executable
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core 
    Qt6::Quick
    Qt6::Gui
    Qt6::Qml
)

# Include the Qt5 include directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE ${Qt6_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})
target_link_libraries(${PROJECT_NAME} Qt6::Core)
target_link_libraries(${PROJECT_NAME} Qt6::Quick)
target_link_libraries(${PROJECT_NAME} Qt6::QuickControls2)

# Add resources
qt_add_resources(RESOURCES resources.qrc)

set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_WARNING_AS_ERROR ON)

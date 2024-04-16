cmake_minimum_required(VERSION 3.22)
project(DonutQMLPlayer)


include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/renderer
)

### QT库设置
find_package(Qt6 COMPONENTS
    Core
    Qml
    Quick
    OpenGL
    OpenGLWidgets
    
    REQUIRED


)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


file(GLOB ${PROJECT_NAME}_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

file(GLOB ${PROJECT_NAME}_RENDERER_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/renderer/*.h)
file(GLOB ${PROJECT_NAME}_RENDERER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/renderer/*.cpp)

file(GLOB ${PROJECT_NAME}_OPENGL_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/renderer/opengl/*.h)
file(GLOB ${PROJECT_NAME}_OPENGL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/renderer/opengl/*.cpp)

file(GLOB ${PROJECT_NAME}_GLM_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/include/glm/glm/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/glm/glm/*.inl
)

file(GLOB ${PROJECT_NAME}_STB_IMAGE_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/stb_image/stb_image.h)
file(GLOB ${PROJECT_NAME}_STB_IMAGE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/include/stb_image/stb_image.cpp)


file(GLOB ${PROJECT_NAME}_QRC_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.qrc)
file(GLOB ${PROJECT_NAME}_QML_FILES  ${CMAKE_CURRENT_LIST_DIR}/src/*.qml)
file(GLOB ${PROJECT_NAME}_QML_DIR  ${CMAKE_CURRENT_LIST_DIR}/src/qmldir)

#####################################################################################################
# assets
file(GLOB ${PROJECT_NAME}_TEXTURES
    ${CMAKE_CURRENT_LIST_DIR}/bin/assets/textures/*.*
)

file(GLOB ${PROJECT_NAME}_SHADERS
    ${CMAKE_CURRENT_LIST_DIR}/bin/assets/shaders/*.*
)

source_group("qrc"                              FILES ${${PROJECT_NAME}_QRC_FILES} ${${PROJECT_NAME}_QML_DIR})
source_group("qrc//qml"                         FILES ${${PROJECT_NAME}_QML_FILES})
source_group("src"                              FILES ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SOURCES})
source_group("src//renderer"                    FILES ${${PROJECT_NAME}_RENDERER_HEADERS} ${${PROJECT_NAME}_RENDERER_SOURCES})
source_group("src//renderer//opengl"            FILES ${${PROJECT_NAME}_OPENGL_HEADERS} ${${PROJECT_NAME}_OPENGL_SOURCES})
source_group("src//glm"                         FILES ${${PROJECT_NAME}_GLM_HEADERS})
source_group("src//stb_image"                   FILES ${${PROJECT_NAME}_STB_IMAGE_HEADERS} ${${PROJECT_NAME}_STB_IMAGE_SOURCES})
source_group("assets//shaders"                  FILES ${${PROJECT_NAME}_SHADERS} )
source_group("assets//textures"                 FILES ${${PROJECT_NAME}_TEXTURES} )

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_RENDERER_HEADERS} ${${PROJECT_NAME}_RENDERER_SOURCES}
    ${${PROJECT_NAME}_OPENGL_HEADERS} ${${PROJECT_NAME}_OPENGL_SOURCES}
    ${${PROJECT_NAME}_QRC_FILES}
    ${${PROJECT_NAME}_QML_FILES}
    ${${PROJECT_NAME}_QML_DIR}
    ${${PROJECT_NAME}_GLM_HEADERS}
    ${${PROJECT_NAME}_STB_IMAGE_HEADERS} ${${PROJECT_NAME}_STB_IMAGE_SOURCES}

    ${${PROJECT_NAME}_SHADERS}
    ${${PROJECT_NAME}_TEXTURES}
    # ${QRC_FILES}
)

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup" )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin"
    )
endif()


set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/src/SkinColor.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)


qt_add_qml_module(
    ${PROJECT_NAME}
    VERSION 1.0
    URI Donut.DonutQMLPlayer
    SOURCES ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SOURCES}
)

set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/glm
    ${CMAKE_CURRENT_LIST_DIR}/include/stb_image
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_17
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::Gui
    Qt6::Network
    Qt6::OpenGL
    Qt6::OpenGLWidgets

)

# 项目输出路径
set(RUNTIME "${CMAKE_CURRENT_LIST_DIR}/bin")
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${RUNTIME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RUNTIME}
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_17
)
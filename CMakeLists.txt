cmake_minimum_required(VERSION 3.22)
project(DonutQMLPlayer)


include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/renderer
    ${CMAKE_CURRENT_LIST_DIR}/src/log
    ${CMAKE_CURRENT_LIST_DIR}/src/media
    ${CMAKE_CURRENT_LIST_DIR}/src/media/media_data
    ${CMAKE_CURRENT_LIST_DIR}/src/handler
)

### QT库设置
find_package(Qt6 COMPONENTS
    Core
    Qml
    Quick REQUIRED

)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


file(GLOB ${PROJECT_NAME}_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB ${PROJECT_NAME}_QRC_FILES ${CMAKE_CURRENT_LIST_DIR}/src/*.qrc)
file(GLOB ${PROJECT_NAME}_QML_FILES  ${CMAKE_CURRENT_LIST_DIR}/src/*.qml)
file(GLOB ${PROJECT_NAME}_QML_DIR  ${CMAKE_CURRENT_LIST_DIR}/src/qmldir)

FILE(GLOB ${PROJECT_NAME}_LOG_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/log/*.h)
FILE(GLOB ${PROJECT_NAME}_LOG_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/log/*.cpp)

file(GLOB ${PROJECT_NAME}_RENDERER_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/renderer/*.h)
file(GLOB ${PROJECT_NAME}_RENDERER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/renderer/*.cpp)

file(GLOB ${PROJECT_NAME}_HANDLER_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/handler/*.h)
file(GLOB ${PROJECT_NAME}_HANDLER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/handler/*.cpp)

file(GLOB ${PROJECT_NAME}_MEDIA_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/media/*.h)
file(GLOB ${PROJECT_NAME}_MEDIA_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/media/*.cpp)
file(GLOB ${PROJECT_NAME}_MEDIA_DATA_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/media/media_data/*.h)
file(GLOB ${PROJECT_NAME}_MEDIA_DATA_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/media/media_data/*.cpp)

file(GLOB ${PROJECT_NAME}_STBIMAGE_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/stb_image/*.h)
file(GLOB ${PROJECT_NAME}_STBIMAGE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/include/stb_image/*.cpp)

file(GLOB ${PROJECT_NAME}_SHADERS ${CMAKE_CURRENT_LIST_DIR}/bin/assets/shaders/*.glsl)

source_group("qrc"                              FILES  ${${PROJECT_NAME}_QRC_FILES} ${${PROJECT_NAME}_QML_DIR})
source_group("qrc//qml"                         FILES  ${${PROJECT_NAME}_QML_FILES})
source_group("src"                              FILES  ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SOURCES})
source_group("src//log"                         FILES  ${${PROJECT_NAME}_LOG_HEADERS} ${${PROJECT_NAME}_LOG_SOURCES})
source_group("src//renderer"                    FILES  ${${PROJECT_NAME}_RENDERER_HEADERS} ${${PROJECT_NAME}_RENDERER_SOURCES})
source_group("src//media"                       FILES  ${${PROJECT_NAME}_MEDIA_HEADERS} ${${PROJECT_NAME}_MEDIA_SOURCES})
source_group("src//media//media_data"           FILES  ${${PROJECT_NAME}_MEDIA_DATA_HEADERS} ${${PROJECT_NAME}_MEDIA_DATA_SOURCES})
source_group("src//handler"                     FILES  ${${PROJECT_NAME}_HANDLER_HEADERS} ${${PROJECT_NAME}_HANDLER_SOURCES})
source_group("src//renderer//stb_image"         FILES  ${${PROJECT_NAME}_STBIMAGE_HEADERS} ${${PROJECT_NAME}_STBIMAGE_SOURCES})
source_group("src//renderer//assets//shaders"   FILES  ${${PROJECT_NAME}_SHADERS})


add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_HEADERS}
    ${${PROJECT_NAME}_SOURCES}

    ${${PROJECT_NAME}_RENDERER_HEADERS}
    ${${PROJECT_NAME}_RENDERER_SOURCES}

    ${${PROJECT_NAME}_LOG_HEADERS}
    ${${PROJECT_NAME}_LOG_SOURCES}

    ${${PROJECT_NAME}_MEDIA_HEADERS}
    ${${PROJECT_NAME}_MEDIA_SOURCES}

    ${${PROJECT_NAME}_MEDIA_DATA_HEADERS}
    ${${PROJECT_NAME}_MEDIA_DATA_SOURCES}
    
    ${${PROJECT_NAME}_HANDLER_HEADERS}
    ${${PROJECT_NAME}_HANDLER_SOURCES}

    ${${PROJECT_NAME}_GLM_HEADERS}
    ${${PROJECT_NAME}_STBIMAGE_HEADERS} ${${PROJECT_NAME}_STBIMAGE_SOURCES}

    ${${PROJECT_NAME}_QRC_FILES}
    ${${PROJECT_NAME}_QML_FILES}
    ${${PROJECT_NAME}_QML_DIR}

    ${${PROJECT_NAME}_SHADERS}
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
    SOURCES
    ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SOURCES}
    #${${PROJECT_NAME}_HANDLER_HEADERS} ${${PROJECT_NAME}_HANDLER_SOURCES}
    ${${CMAKE_CURRENT_LIST_DIR}/src/handler/donut_qml_av_manager.h} ${${CMAKE_CURRENT_LIST_DIR}/src/handler/donut_qml_av_manager.cpp}
)

set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR})

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lib
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/glm
    ${CMAKE_CURRENT_LIST_DIR}/include/stb_image
    ${CMAKE_CURRENT_LIST_DIR}/include/ffmpeg
    ${CMAKE_CURRENT_LIST_DIR}/include/spdlog/include
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    DN_PLATFORM_WINDOWS
    DN_BUILD_STATIC
    __STDC_CONSTANT_MACROS
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

    SDL2
    SDL2main

    avcodec
    avdevice
    avfilter
    avformat
    avutil
    swresample
    swscale
)

# 项目输出路径
set(RUNTIME "${CMAKE_CURRENT_LIST_DIR}/bin")
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${RUNTIME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RUNTIME}
)


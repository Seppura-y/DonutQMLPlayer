cmake_minimum_required(VERSION 3.22)
project(DonutQMLPlayer)


include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

### QT库设置
find_package(Qt6 COMPONENTS Quick REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB ${PROJECT_NAME}_HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.h)
file(GLOB ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB ${PROJECT_NAME}_QRC ${CMAKE_CURRENT_LIST_DIR}/src/*.qrc)

### opencv库设置
set(OPENCVPATH "${CMAKE_CURRENT_LIST_DIR}/lib/opencv/")

if(MINGW)
    set(PROJECT_TOOLCHAIN "mingw")
    string(APPEND OPENCVPATH "mingw/")
endif()
if(MSVC)
    set(PROJECT_TOOLCHAIN "msvc")
    string(APPEND OPENCVPATH "msvc/lib/")
endif()



add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_HEADERS}
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_QRC}
)

qt_add_qml_module(
    ${PROJECT_NAME}
    VERSION 1.0
    URI DonutQMLPlayer
    RESOURCES qml.qrc
    RESOURCES src/qml.qrc
)


set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Quick
)

# 项目输出路径
set(RUNTIME "${CMAKE_CURRENT_LIST_DIR}/bin/${PROJECT_TOOLCHAIN}")
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${RUNTIME}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RUNTIME}
)


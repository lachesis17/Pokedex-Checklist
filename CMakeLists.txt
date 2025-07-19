cmake_minimum_required(VERSION 3.16)
project(Pokedex_Checklist)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_PREFIX_PATH "C:/Qt/qtbase-6.8/build/lib/cmake")

# Bundle mingw/gcc
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/rapidjson
)

set(PROJECT_SRC
    src/main.cpp
    src/mainWindow.cpp
)

set(PROJECT_HEADERS
    src/animatedHoverButton.h
    src/animatedHoverIcon.h
    src/pokemonData.h
    src/mainWindow.h
)

set(RESOURCES
    assets/resources.qrc
)
set(WIN_RESOURCES assets/resources.rc)

find_package(Qt6 COMPONENTS Widgets Core Gui REQUIRED)

add_executable(Pokedex_Checklist
    ${PROJECT_SRC}
    ${PROJECT_HEADERS}
    ${RESOURCES}
    ${WIN_RESOURCES}
)

target_link_libraries(Pokedex_Checklist
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui
)

if(WIN32)
    set_target_properties(Pokedex_Checklist PROPERTIES WIN32_EXECUTABLE YES)
endif()

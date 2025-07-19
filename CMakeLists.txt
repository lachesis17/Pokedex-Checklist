cmake_minimum_required(VERSION 3.16)
project(PokedexChecklist)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# manual path to Qt libraries
# set(CMAKE_PREFIX_PATH "C:/Qt/qtbase-6.8/build/lib/cmake")

if(NOT CMAKE_PREFIX_PATH)
    if(WIN32)
        set(QT_POSSIBLE_PATHS
            "C:/Qt/6.9.1/mingw_64/lib/cmake"
            "C:/Qt/6.8.3/mingw_64/lib/cmake"
            "C:/Qt/6.7.3/mingw_64/lib/cmake"
            "C:/Qt/6.6.3/mingw_64/lib/cmake"
            "C:/Qt/6.5.3/mingw_64/lib/cmake"
            "C:/Qt/qtbase-6.8/build/lib/cmake"
        )
        foreach(path ${QT_POSSIBLE_PATHS})
            if(EXISTS ${path})
                set(CMAKE_PREFIX_PATH ${path})
                message(STATUS "Auto-detected Qt at: ${path}")
                break()
            endif()
        endforeach()
    endif()
endif()

if(NOT CMAKE_PREFIX_PATH)
    message(FATAL_ERROR "Qt not found! Please uncomment and set CMAKE_PREFIX_PATH in CMakeLists.txt to your Qt installation.")
endif()

# bundle mingw/gcc
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs/rapidjson
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

add_executable(PokedexChecklist
    ${PROJECT_SRC}
    ${PROJECT_HEADERS}
    ${RESOURCES}
    ${WIN_RESOURCES}
)

target_link_libraries(PokedexChecklist
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui
)

if(WIN32)
    set_target_properties(PokedexChecklist PROPERTIES WIN32_EXECUTABLE YES)
endif()

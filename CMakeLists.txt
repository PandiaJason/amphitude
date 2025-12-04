cmake_minimum_required(VERSION 3.14)
project(Amphitude VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---

# 1. SDL2 & Friends
# We use a custom module path or rely on Config mode.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# On macOS with Homebrew, hint the location
if(APPLE)
    set(CMAKE_PREFIX_PATH "/opt/homebrew" ${CMAKE_PREFIX_PATH})
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)



# --- Game Executable ---

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Utils.cpp

)

add_executable(amphitude ${SOURCES})

# Include directories
target_include_directories(amphitude PRIVATE 
    include
    /opt/homebrew/include

)

# Link libraries
target_link_libraries(amphitude PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_net::SDL2_net

)

# --- Server Executable ---
add_executable(server_app server/main.cpp)
target_link_libraries(server_app PRIVATE SDL2::SDL2 SDL2_net::SDL2_net)

# Copy Assets to Build Directory (for running locally)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# --- Packaging (CPack) ---

# Install Rules
install(TARGETS amphitude DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)

# CPack Configuration
set(CPACK_PACKAGE_NAME "Amphitude")
set(CPACK_PACKAGE_VENDOR "PandiaJason")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Amphitude - A Retro Multiplayer Platformer")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop") # .dmg
    set(CPACK_DMG_VOLUME_NAME "Amphitude")
    set(CPACK_SYSTEM_NAME "macOS")
    # set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/assets/icon.icns") # Placeholder
    set(CPACK_BUNDLE_NAME "Amphitude")
    # set(CPACK_BUNDLE_ICON "icon.icns")
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS") # .exe installer
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM") # .deb / .rpm
endif()

include(CPack)

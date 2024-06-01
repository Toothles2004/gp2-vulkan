cmake_minimum_required(VERSION 3.10)

# Project Name
project(MainProject)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8 # Specify the version you need
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw)

# Find the required packages
find_package(Vulkan REQUIRED)

# Fetch Imgui
FetchContent_Declare(
  imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.4.zip
)

FetchContent_MakeAvailable(imgui)

message ("ImGUI dowloaded to ${imgui_SOURCE_DIR}")

# Dear ImGui
set(IMGUI_DIR ${imgui_SOURCE_DIR})
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

# Include Directories
include_directories(${Vulkan_INCLUDE_DIRS})

add_subdirectory(src)

# If using validation layers, copy the required JSON files (optional)
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        "${CMAKE_SOURCE_DIR}/path_to_json/VK_LAYER_KHRONOS_validation.json"
#        $<TARGET_FILE_DIR:${PROJECT_NAME}>)








# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
# endif()
# 
# set(CMAKE_CXX_STANDARD 11)
# set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

# option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
# option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
# option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
# option(GLFW_INSTALL "Generate installation target" OFF)
# option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)



# Libraries
# find_package(Vulkan REQUIRED)
#find_library(VULKAN_LIBRARY
  #NAMES vulkan vulkan-1)
#set(LIBRARIES "glfw;${VULKAN_LIBRARY}")
# set(LIBRARIES "glfw;Vulkan::Vulkan")

# Use vulkan headers from glfw:
# include_directories(${GLFW_DIR}/deps)


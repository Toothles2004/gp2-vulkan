# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)

set(SOURCES
    "main.cpp"
)

# file(GLOB sources *.cpp)

# Create the executable
# add_executable(example_glfw_vulkan ${sources} ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)
add_executable(MainProject ${SOURCES}  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp ${GLSL_SOURCE_FILES}  "Window.h" "Window.cpp" "Application.h" "Application.cpp" "Pipeline.h" "Pipeline.cpp" "Device.h" "Device.cpp" "SwapChain.h" "SwapChain.cpp" "Mesh.h" "Mesh.cpp" "GameObject.h"  "Renderer.h" "Renderer.cpp" "SimpleRenderSystem.h" "SimpleRenderSystem.cpp" "Camera.h" "Camera.cpp" "KeyboardInput.h" "KeyboardInput.cpp" "TinyObjLoader.h"  "Utils.h"  "GameObject.cpp" "Buffer.h" "Buffer.cpp" "FrameInfo.h"  "RenderSystem2D.h" "RenderSystem2D.cpp")
add_dependencies(${PROJECT_NAME} Shaders)

set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_WARNING_AS_ERROR ON)

# Link libraries
# target_link_libraries(example_glfw_vulkan ${LIBRARIES})
# target_compile_definitions(example_glfw_vulkan PUBLIC -DImTextureID=ImU64)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MainProject PRIVATE ${Vulkan_LIBRARIES} glfw)
target_compile_definitions(MainProject PUBLIC -DImTextureID=ImU64)

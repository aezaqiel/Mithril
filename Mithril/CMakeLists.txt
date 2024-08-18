add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
add_subdirectory(vendor/volk)

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/Mithril/src/*.cpp)

add_library(Mithril STATIC ${SRC_FILES})

target_include_directories(Mithril
PUBLIC
    src
)

target_link_libraries(Mithril
PUBLIC
    spdlog
    glm
    volk
PRIVATE
    glfw
)

target_compile_options(Mithril PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

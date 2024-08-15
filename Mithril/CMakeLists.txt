add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

add_library(Mithril STATIC
    src/Mithril/Mithril.hpp
    src/Mithril/EntryPoint.hpp

    src/Mithril/Core/Application.hpp
    src/Mithril/Core/Application.cpp
    src/Mithril/Core/Logger.hpp
    src/Mithril/Core/Logger.cpp
    src/Mithril/Core/KeyCodes.hpp
    src/Mithril/Core/MouseCodes.hpp
    src/Mithril/Core/Events/Event.hpp
    src/Mithril/Core/Events/ApplicationEvent.hpp
    src/Mithril/Core/Events/WindowEvent.hpp
    src/Mithril/Core/Events/KeyEvent.hpp
    src/Mithril/Core/Events/MouseEvent.hpp
    src/Mithril/Core/Window.hpp
    src/Mithril/Core/Window.cpp
    src/Mithril/Core/Input.hpp
    src/Mithril/Core/Input.cpp
    src/Mithril/Core/Layer.hpp
    src/Mithril/Core/LayerStack.hpp
    src/Mithril/Core/LayerStack.cpp
)

target_include_directories(Mithril
PUBLIC
    src
)

target_link_libraries(Mithril
PUBLIC
    spdlog
    glm
PRIVATE
    glfw
)

target_compile_options(Mithril PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

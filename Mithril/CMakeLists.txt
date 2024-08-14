add_subdirectory(vendor/spdlog)

add_library(Mithril STATIC
    src/Mithril/Mithril.hpp
    src/Mithril/EntryPoint.hpp

    src/Mithril/Core/Application.hpp
    src/Mithril/Core/Application.cpp
    src/Mithril/Core/Logger.hpp
    src/Mithril/Core/Logger.cpp
)

target_include_directories(Mithril
PUBLIC
    src
)

target_link_libraries(Mithril
PUBLIC
    spdlog
)

target_compile_options(Mithril PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

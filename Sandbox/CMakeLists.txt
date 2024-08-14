
add_executable(Sandbox
    src/Sandbox.hpp
    src/Sandbox.cpp
)

target_include_directories(Sandbox
PRIVATE
    src
)

target_link_libraries(Sandbox
PRIVATE
    Mithril
)

target_compile_options(Sandbox PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

#pragma once

#include <memory>

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8  = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;

namespace Mithril {

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename... A>
    constexpr Scope<T> CreateScope(A&&... args)
    {
        return std::make_unique<T>(std::forward<A>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename... A>
    constexpr Ref<T> CreateRef(A&&... args)
    {
        return std::make_shared<T>(std::forward<A>(args)...);
    }

}

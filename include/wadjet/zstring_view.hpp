#pragma once

#include <string_view>
#include <cstdint>
#include <cstddef>

namespace wadjet {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Zero-terminated string view.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Primitive implementation of zero-terminated string_view.
template<typename Char>
class basic_zstring_view
{
public:
    constexpr basic_zstring_view(const Char* null_terminated_string) noexcept;

    constexpr const Char*  data() const noexcept;
    constexpr const size_t size() const noexcept;

private:
    inline static constexpr size_t strlen_impl(const Char* null_terminated_string);

    const Char*  data_m;
    const size_t size_m;
};

using zstring_view = basic_zstring_view<char>;

template<typename Char>
inline constexpr basic_zstring_view<Char>::basic_zstring_view(
    const Char* null_terminated_string) noexcept :
    data_m(null_terminated_string), size_m(strlen_impl(null_terminated_string))
{
}

template<typename Char>
inline constexpr const Char* basic_zstring_view<Char>::data() const noexcept
{
    return data_m;
}

template<typename Char>
inline constexpr const size_t basic_zstring_view<Char>::size() const noexcept
{
    return size_m;
}

template<typename Char>
inline constexpr size_t basic_zstring_view<Char>::strlen_impl(const Char* null_terminated_string)
{
    for(size_t i = 0;; ++i)
    {
        if(null_terminated_string[i] == 0)
            return i;
    }

    return -1;
}

} // namespace wadjet

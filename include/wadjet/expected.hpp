#pragma once

#include <cassert>
#include <utility>
#include <variant>

namespace wadjet {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Expected.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Primitive implementation of unexpected concept (C++23).
template<typename E>
class unexpected
{
public:
    explicit constexpr unexpected(E&& value) noexcept;

    constexpr const E& value() const noexcept;

private:
    const E value_m;
};

template<typename E>
inline constexpr unexpected<E>::unexpected(E&& value) noexcept : value_m(value)
{
}

template<typename E>
inline constexpr const E& unexpected<E>::value() const noexcept
{
    return value_m;
}

template<typename E, typename... Args>
unexpected<E> make_unexpected(Args&&... args)
{
    return unexpected<E>{E(std::forward<Args>(args)...)};
}

template<typename T, typename E>
class expected
{
public:
    constexpr expected(T&& value) noexcept;
    constexpr expected(unexpected<E>&& value) noexcept;

    bool     has_value() const noexcept;
    T&       value() noexcept;
    const E& error() const noexcept;

    explicit operator bool() const noexcept;
    T&       operator*() noexcept;
    T*       operator->() noexcept;

private:
    std::variant<T, unexpected<E>> storage_m;
};

template<typename T, typename E>
inline constexpr expected<T, E>::expected(T&& value) noexcept :
    storage_m(std::move_if_noexcept(value))
{
}

template<typename T, typename E>
inline constexpr expected<T, E>::expected(unexpected<E>&& error) noexcept :
    storage_m(std::move_if_noexcept(error))
{
}

template<typename T, typename E>
inline bool expected<T, E>::has_value() const noexcept
{
    return std::holds_alternative<T>(storage_m);
}

template<typename T, typename E>
inline T& expected<T, E>::value() noexcept
{
    assert(has_value());
    return std::get<T>(storage_m);
}

template<typename T, typename E>
inline const E& expected<T, E>::error() const noexcept
{
    assert(!has_value());
    return std::get<unexpected<E>>(storage_m).value();
}

template<typename T, typename E>
inline expected<T, E>::operator bool() const noexcept
{
    return has_value();
}

template<typename T, typename E>
inline T& expected<T, E>::operator*() noexcept
{
    return value();
}

template<typename T, typename E>
inline T* expected<T, E>::operator->() noexcept
{
    return &value();
}

} // namespace wadjet

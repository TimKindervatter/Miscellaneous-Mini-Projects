#include <type_traits>
#include <iostream>
#include <iomanip>
#include <functional>


class Assignable
{};

class Copyable
{
public:
    Copyable& operator=(const Copyable&) = default;
    Copyable& operator=(Copyable&&) = delete;
};

class Movable
{
public:
    Movable& operator=(const Movable&) = delete;
    Movable& operator=(Movable&&) = default;
};

class NotAssignable
{
    NotAssignable& operator=(const NotAssignable&) = delete;
    NotAssignable& operator=(NotAssignable&&) = delete;
};

namespace detail
{
    template<class T, typename = decltype(std::declval<T(&)()>())>
    auto test_returnable(int)->std::true_type;

    template<class>
    auto test_returnable(...)->std::false_type;

    template<typename To>
    constexpr bool is_returnable = decltype(test_returnable<To>(0))::value;


#define function_with_To_parameter std::declval<void(&)(To)>()
#define From_value std::declval<From>()

    template<class From, class To, typename = decltype(function_with_To_parameter(From_value))>
    auto test_implicitly_convertible(int)->std::true_type;

    template<class, class>
    auto test_implicitly_convertible(...)->std::false_type;

    template<typename From, typename To>
    constexpr bool is_implicitly_convertible = decltype(test_implicitly_convertible<From, To>(0))::value;


    template<typename From, typename To>
    constexpr bool from_and_to_are_both_void = (std::is_void<From>::value && std::is_void<To>::value);
} // namespace detail

using namespace detail;

template<class From, class To>
struct is_convertible : std::bool_constant<
    (is_returnable<To>&& is_implicitly_convertible<From, To>) ||
    from_and_to_are_both_void<From, To>
> {};

#define PRINT(...) std::cout << (#__VA_ARGS__) << ": " << __VA_ARGS__::value << '\n';

int main()
{
    std::cout << std::boolalpha << std::right;

    PRINT(is_convertible<short, int>)
    PRINT(is_convertible<double, float>)
    PRINT(is_convertible<double, int>)
    PRINT(is_convertible<char[5], char*>)
    std::cout << '\n';

    PRINT(is_convertible<const char*, std::string>)
    PRINT(is_convertible<std::string, const char*>)
    std::cout << '\n';

    auto stateless_lambda = [](int) {};
    PRINT(is_convertible<decltype(stateless_lambda), void(*)(int)>)
    PRINT(is_convertible<void(*)(int), decltype(stateless_lambda)>)
    PRINT(is_convertible<void(*)(int), std::function<void(int)>>)
    PRINT(is_convertible<std::function<void(int)>, void(*)(int)>)
    std::cout << '\n';

    PRINT(is_convertible<void, int>)
    PRINT(is_convertible<int, void>)
    PRINT(is_convertible<void, void>)
    std::cout << '\n';

    PRINT(is_convertible<Assignable, Assignable>)
    PRINT(is_convertible<Copyable, Copyable>)
    PRINT(is_convertible<Movable, Movable>)
    PRINT(is_convertible<NotAssignable, NotAssignable>)
    std::cout << '\n';
}
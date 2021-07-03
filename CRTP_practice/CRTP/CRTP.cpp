#include <type_traits>
#include <iostream>


// See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf.
template <typename...>
using void_t = void;

// Primary template handles all types not supporting the operation.
template <typename, template <typename> class, typename = void_t<>>
struct detect : std::false_type {};

// Specialization recognizes/validates only types supporting the archetype.
template <typename T, template <typename> class Op>
struct detect<T, Op, void_t<Op<T>>> : std::true_type {};


template<typename T>
concept HasStub = requires(T t)
{
    t.stub();
};


template<typename T>
class Base
{
private:
    template <typename U = T>
    using stub_t = decltype(std::declval<U>().stub());

    template <typename U = T>
    using has_stub = detect<U, stub_t>;

    template <typename U = T>
    inline static constexpr bool has_stub_v = has_stub<U>().value;

public:
    //template<typename U = T, typename = std::enable_if_t<has_stub_v<U>>>
    void do_something() requires HasStub<T>
    {
        std::cout << self().stub() << '\n';
    }

private:
    Base()
    {
        static_assert(std::is_base_of_v<Base, T>, "This class is only meant for use with CRTP. That is, the base class must be templated on one of its derived classes.");
    }
    friend T;

    T& self() { return static_cast<T&>(*this); }
    const T& self() const { return static_cast<const T&>(*this); }
};

class Derived1 : public Base<Derived1>
{
public:
    int stub() { return m_stub; }

private:
    int m_stub{ 42 };
};

class Derived2 : public Base<Derived2>
{
public:
    std::string stub() { return m_stub; }

private:
    std::string m_stub{ "Hello World" };
};

class Derived3 : public Base<Derived3>
{

};

class Derived4 : public Base<Derived2>
{

};

class NotDerived : public Base<int>
{
public:
    double stub() { return m_stub; }

private:
    double m_stub{};
};

int main()
{
    Derived1 d1;
    Derived2 d2;
    Derived3 d3;
    Derived4 d4;
     //NotDerived nd;  // Fails with static assert in constructor of Base because NotDerived inherits from Base<int> and Base is only meant for use with CRTP, so Base<int> is not allowed.
    Base<int> b;

    d1.do_something();
    d2.do_something();
    //d3.do_something();  // Does not compile because Derived3::do_something() is SFINAE'd out, since Derived3 has no member stub()
    //d4.do_something();  // Runtime error because d4 gets static cast to Derived2. Derived4 has no members, so trying to access stub is UB.
}
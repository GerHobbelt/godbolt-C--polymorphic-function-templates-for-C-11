// MSVC:   /std:c++14 /Zc:__cplusplus /Zc:preprocessor    (without the /Zc the language version reporting + macro work will fail!)
// clang:  -std=c++11
// gcc:    -std=c++11
//
// Note: https://learn.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version?view=msvc-170
// MSVC2026 doesn't support going back to any C++ version before C++/14.
//

#include <initializer_list>
#include <string>
#include <iostream>
#include <cstdarg>
#include <cassert>
#include <type_traits>
#include <stdint.h>
#include <utility>


template <typename T,
          typename std::enable_if<
            std::is_floating_point<T>::value,
            bool
          >::type = true
        >
std::string test(T v) {
  char buf[40];
  snprintf(buf, sizeof(buf), "%lg", double(v));
  return buf;
}


template <typename T,
          typename std::enable_if<
            (std::is_arithmetic<T>::value || std::is_enum<T>::value)
            && !std::is_floating_point<T>::value,
            bool
          >::type = true
        >
std::string test(T v) {
  char buf[sizeof(T) * 8 + 3];
  snprintf(buf, sizeof(buf), "%d", int(v));
  return buf;
}

static_assert(std::is_null_pointer<decltype(nullptr)>::value                         , "uh-oh!");
static_assert(!std::is_null_pointer<int*>::value                                     , "uh-oh!");
static_assert(!std::is_pointer<decltype(nullptr)>::value                             , "uh-oh!");
static_assert(std::is_pointer<int*>::value                                           , "uh-oh!");

template <typename T = void,
          typename std::enable_if<
            std::is_null_pointer<T>::value,
            bool
          >::type = true
        >
std::string test(const T v) {
  return "(null)";
}


template <typename T,
          typename std::enable_if<
            (
                std::is_assignable<const char *, T>::value
            ),
            bool
          >::type = true
        >
std::string test(const T &v) {
  const char *s = v;
  if (!v) {
    s = "(null)";
  }
  return std::string("STR.ca:") + s;
}


template <typename T,
          typename std::enable_if<
            (
                std::is_assignable<char *, T>::value
            ),
            bool
          >::type = true
        >
std::string test(T &v) {
  const char *s = v;
  if (!v) {
    s = "(null)";
  }
  return std::string("STR.a:") + s;
}


template <typename T,
          typename std::enable_if<
            (
              std::is_base_of<const char *, T>::value
            ),
            bool
          >::type = true
        >
std::string test(const T &v) {
  const char *s = v;
  if (!v) {
    s = "(null)";
  }
  return std::string("STR.cb:") + s;
}


template <typename T,
          typename std::enable_if<
            (
              std::is_base_of<char *, T>::value
            ),
            bool
          >::type = true
        >
std::string test(T &v) {
  const char *s = v;
  if (!v) {
    s = "(null)";
  }
  return std::string("STR.b:") + s;
}


// the flavours above DO NOT match the `const char *` or `{const} char[]` literal types...    :-S

template <typename T,
          typename std::enable_if<
            (
              std::is_same<const char *, T>::value
            ),
            bool
          >::type = true
        >
std::string test(const T &v) {
  const char *s = v;
  if (!v) {
    s = "(null)";
  }
  return std::string("STR.cs:") + s;
}


template <typename T,
          typename std::enable_if<
            (
              std::is_same<char *, T>::value
            ),
            bool
          >::type = true
        >
std::string test(T &v) {
  const char *s = v;
  if (!v) {
    s = "(null)";
  }
  return std::string("STR.s:") + s;
}


#if 01 // the `const char *` argument types won't match otherwise!   :-S

template <typename T,
          typename std::enable_if<
            (
              std::is_convertible<T, const char *>::value
              && ! std::is_same<char *, T>::value
              && ! std::is_same<const char *, T>::value
              && ! std::is_null_pointer<T>::value
            ),
            bool
          >::type = true
        >
std::string test(const T &v) {
  const char *s = v;
  if (!v) {
    s = "(null)";
  }
  return std::string("STR.cc:") + s;
}

#endif





typedef enum E {
    A1, B1, C1
} E_t;

class CL {
public:
    int b;
    const char *c_str() const {
        return "class CL instance";
    }
};

class ND {
public:
    int b;
    const char *foobar() const {
        return "class ND instance";
    }
};




static_assert(decltype(std::true_type())()                                                                           , "uh-oh!");
static CL f_o_r{};
static auto csrv = f_o_r.c_str();
static_assert(std::is_same<decltype(csrv), const char *>::value                                                      , "uh-oh!");
static_assert(std::is_same<decltype(std::declval<CL>().c_str()), const char *>::value                                , "uh-oh!");
            //
            // NOTE: std::enable_if<T> only has a conditionally available boolean ::type member,
            // so we turn that into a value by using `::type{true}`
            //
static_assert(std::enable_if<std::true_type{}, bool>::type{true}                                                     , "uh-oh!");
static_assert(std::enable_if<true, bool>::type{true}                                                                 , "uh-oh!");
static_assert(std::enable_if<std::is_same<decltype(std::declval<CL>().c_str()), const char *>::value, bool>::type{true}                         , "uh-oh!");
static_assert(std::is_same<       decltype(std::declval<CL>().c_str()), const char *>::value                         , "uh-oh!");
static_assert(std::is_convertible<decltype(std::declval<CL>().c_str()), const char *>::value                         , "uh-oh!");




// Primary template (default: false)
//template <typename T>
//struct has_cstr_member_function : std::false_type {};

// Specialization: Check for member function "c_str" with signature const char *c_str(void)
template <typename T>
struct has_cstr_member_function {
private:
    // Helper 1: Substitute T::func() and check return type
    template <typename U>
    static auto test(int) -> decltype(
            // Attempt to call U::func
            // Validate return type matches `const char *`
            std::is_convertible<decltype(std::declval<U>().c_str()), const char *>::value,
            std::true_type{}
        ) ;

    // Helper 2: Fallback
    template <typename U>
    static auto test(...) -> std::false_type ;

public:
    static constexpr bool value = decltype(test<T>(0)){};
};


template <typename T>
using obtain_cstr_func_t = decltype(std::declval<T&>().c_str());


template <typename T,
          typename std::enable_if<
            has_cstr_member_function<T>::value,
            bool
          >::type = false
        >
std::string test(const T &v) {
  auto s = v.c_str();
  return std::string("c_str() --> ") + s;
}


#if 01

template <typename T,
          typename std::enable_if<
            !(std::is_arithmetic<T>::value ||
              std::is_enum<T>::value ||
              std::is_assignable<const char *, T>::value ||
              std::is_convertible<T, const char *>::value ||
              std::is_same<const char *, T>::value ||
              std::is_assignable<char *, T>::value ||
              std::is_convertible<T, char *>::value ||
              std::is_same<char *, T>::value ||
              has_cstr_member_function<T>::value ||
              std::is_null_pointer<T>::value
             ),
            bool
          >::type = true
        >
std::string test(const T &v) {
  char buf[sizeof(T) * 8 + 16];
#if 01    // very specific to this test code source file...
  snprintf(buf, sizeof(buf), "%p (%s)", (const void *)&v, v.foobar());
#else     // generic...
  snprintf(buf, sizeof(buf), "%p", (const void *)&v);
#endif
  return buf;
}

#endif



/*
   Expected output:

```
TESTS --------------->
0
1000
3.1415
1
97
(null)
STR.cs:strrrrring
STR.cc:xyz
STR.s:(null)
STR.cs:buggerit
c_str() --> buggerit
c_str() --> class CL instance
0x7fffc83b312c (class ND instance)


Done!
```

Note: of course, the raw pointer value will be different each run.
*/

int main(void) {
    std::cout << "TESTS ---------------> "
    << "\n";

    std::cout << test(false) << "\n";
    std::cout << test(1000) << "\n";
    std::cout << test(3.1415) << "\n";
    std::cout << test(B1) << "\n";
    std::cout << test('a') << "\n";
    std::cout << test(nullptr) << "\n";
    const char *str = "strrrrring";
    std::cout << test(str) << "\n";
    std::cout << test("xyz") << "\n";

    char *nullp = nullptr;
    std::cout << test(nullp) << "\n";

    std::string foul_old_Ron("buggerit");
    std::cout << test(foul_old_Ron.c_str()) << "\n";
    std::cout << test(foul_old_Ron) << "\n";

    CL duck_man;
    std::cout << test(duck_man) << "\n";

    ND the_lady;
    std::cout << test(the_lady) << "\n";

    std::cout << "\n\nDone!\n\n";

    return 0;
}

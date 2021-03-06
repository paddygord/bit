# &lt;bit&gt;

This repo provides a C++11/14/17 equivalent for C++20s [&lt;bit&gt; header](https://en.cppreference.com/w/cpp/header/bit).

It aims for complete interface equivalence while still being performant, using builtin bitwise operations where available, and fallback slow implementations where not.

## Dependencies

For testing and installing:
 - [Meson](https://mesonbuild.com/) `sudo apt install meson`

## Caveats

[\[\[nodiscard\]\]](https://en.cppreference.com/w/cpp/language/attributes/nodiscard) is only supported in C++17

[constexpr](https://en.cppreference.com/w/cpp/language/constexpr) functions are only supported in C++14

## Usage

Include `bit.hh` and use `bit::` namespace instead of `std::` namespace

## Building and Testing

`test.sh`

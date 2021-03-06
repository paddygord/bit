#if defined __GNUC__
#include <immintrin.h>
#elif defined _MSC_VER
#include <intrin.h>
#endif
#include <limits>
#include <type_traits>
#include <cstring>

namespace bit {

namespace {
#if __cplusplus >= 201703L
#define bit_nodiscard [[nodiscard]]
#else
#define bit_nodiscard
#endif

#if __cplusplus >= 201402L
#define bit_constexpr constexpr
#else
#define bit_constexpr
#endif

template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B,T>::type;
}

enum class endian {
#if defined __GNUC__
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#elif defined _WIN32
    little = 0,
    big    = 1,
    native = little
#endif
};

template <class To, class From, typename = enable_if_t<
    sizeof(To) == sizeof(From) &&
    std::is_trivially_copyable<From>::value &&
    std::is_trivially_copyable<To>::value &&
    std::is_trivially_constructible<To>::value,
    To>>
To bit_cast(const From& src) noexcept {
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_nodiscard
bit_constexpr T rotl(T x, int s) noexcept {
    int r = s % std::numeric_limits<T>::digits;
    if (r == 0) {
        return x;
    } else if (r > 0) {
        return (x << r) | (x >> (std::numeric_limits<T>::digits - r));
    } else {
        return (x >> -r) | (x << (std::numeric_limits<T>::digits + r));
    }
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_nodiscard
bit_constexpr T rotr(T x, int s) noexcept {
    int r = s % std::numeric_limits<T>::digits;
    if (r == 0) {
        return x;
    } else if (r > 0) {
        return (x >> r) | (x << (std::numeric_limits<T>::digits - r));
    } else {
        return (x << -r) | (x >> (std::numeric_limits<T>::digits + r));
    }
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_constexpr int countl_zero(T x) noexcept {
#if defined __GNUC__
    if (x != 0) {
        if (std::is_same<T, unsigned>::value) {
            return __builtin_clz(x);
        } else if (std::is_same<T, unsigned long>::value) {
            return __builtin_clzl(x);
        } else if (std::is_same<T, unsigned long long>::value) {
            return __builtin_clzll(x);
        } else {
            int s = std::numeric_limits<unsigned>::digits - std::numeric_limits<T>::digits;
            return __builtin_clz(x << s);
        }
    } else {
        return 0;
    }
#elif defined _MSC_VER
    unsigned long out;
    if (std::is_same<T, unsigned long>::value) {
        _BitScanReverse(&out, x);
    } else if (std::is_same<T, unsigned long long>::value) {
        _BitScanReverse64(&out, x);
    } else {
        int s = std::numeric_limits<unsigned long>::digits - std::numeric_limits<T>::digits;
        _BitScanReverse(&out, x << s);
    }
    return out;
#else
    int i;
    T mask = T{1} << (std::numeric_limits<T>::digits - 1);
    for (i = 0; (x & mask) == 0; x <<= 1, i++);
    return i;
#endif
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_constexpr int countl_one(T x) noexcept {
    return countl_zero(~x);
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_constexpr int countr_zero(T x) noexcept {
#if defined __GNUC__
    if (x != 0) {
        if (std::is_same<T, unsigned long long>::value) {
            return __builtin_ctzll(x);
        } else if (std::is_same<T, unsigned long>::value) {
            return __builtin_ctzl(x);
        } else {
            return __builtin_ctz(x);
        }
    } else {
        return 0;
    }
#elif defined _MSC_VER
    unsigned long out;
    if (std::is_same<T, unsigned long>::value) {
        _BitScanForward(&out, x);
    } else if (std::is_same<T, unsigned long long>::value) {
        _BitScanForward64(&out, x);
    } else {
        _BitScanForward(&out, x);
    }
    return out;
#else
    int i;
    for (i = 0; (x & 1) == 0; x >>= 1, i++);
    return i;
#endif
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_constexpr int countr_one(T x) noexcept {
    return countr_zero(~x);
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_constexpr int popcount(T x) noexcept {
#if defined __GNUC__
    return __builtin_popcountll(x);
#else
    int c = 0;
    for (int i = 0; i < std::numeric_limits<T>::digits; i++) {
        c += (x & 1);
        x >>= 1;
    }
    return c;
#endif
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_constexpr bool has_single_bit(T x) noexcept {
    return popcount(x) == 1;
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_constexpr T bit_width(T x) noexcept {
    return std::numeric_limits<T>::digits - countl_zero(x);
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_constexpr T bit_ceil(T x) {
    if (x == ~T{0}) {
        return 0;
    } else if (has_single_bit(x)) {
        return x;
    } else {
        return T{1} << bit_width(x);
    }
}
template<class T, typename = enable_if_t<!std::numeric_limits<T>::is_signed>>
bit_constexpr T bit_floor(T x) noexcept {
    if (x != 0) {
        return T{1} << (bit_width(x) - 1);
    } else {
        return 0;
    }
}

}

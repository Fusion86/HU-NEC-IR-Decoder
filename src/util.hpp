#pragma once

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define DEFAULT_LITTLE_ENDIAN false
#else
#define DEFAULT_LITTLE_ENDIAN true
#endif

template <typename T>
static void print_bits(T x, bool little_endian = DEFAULT_LITTLE_ENDIAN) {
    if (little_endian) {
        constexpr int num = (int)sizeof(T) * 8 - 1;
        for (int i = num; i >= 0; i--) {
            if (i < num && i % 8 == 7) hwlib::cout << ' ';
            hwlib::cout << (x & (1 << i) ? '1' : '0');
        }
    } else {
        for (size_t i = 0; i < sizeof(T) * 8; i++) {
            if (i > 0 && i % 8 == 0) hwlib::cout << ' ';
            hwlib::cout << (x >> i & 0x1 ? '1' : '0');
        }
    }
    hwlib::cout << '\n';
}

#pragma once
#include "pch.h"

#ifdef LBDLL_EXPORTS
#define LBDLL_API __declspec(dllexport)
#else
#define LBDLL_API __declspec(dllimport)
#endif


// Объявляем функции для импорта из динамической библиотеки
extern "C" {
    LBDLL_API bool send_image(const char* ip_address, uint16_t port, const unsigned char* image_data, int width, int height, int channels);
    LBDLL_API unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels);
    LBDLL_API void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel);
    LBDLL_API int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b);
}
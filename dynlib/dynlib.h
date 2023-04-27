#pragma once
#include "common.h"

// Объявляем функции для импорта из динамической библиотеки
extern "C" {
    __declspec(dllimport) bool send_image(const char* ip_address, uint16_t port, const unsigned char* image_data, int width, int height, int channels);
    __declspec(dllimport) unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels);
    __declspec(dllimport) void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel);
    __declspec(dllimport) int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b);
}
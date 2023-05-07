#pragma once

// image_processing_lib.cpp
#include <cstdint>
#include <functional>

// ¬ключаем заголовочный файл WinSocks2 дл€ работы с сокетами
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// ¬ключаем заголовочный файл stb_image дл€ работы с изображени€ми
#include "../shared/stb_image.h"



#ifdef LBDLL_EXPORTS
#define LBDLL_API __declspec(dllexport)
#else
#define LBDLL_API __declspec(dllimport)
#endif


// ќбъ€вл€ем функции дл€ импорта из динамической библиотеки
extern "C" {
    LBDLL_API bool send_image(const char* ip_address, uint16_t port, const unsigned char* image_data, int width, int height, int channels);
    LBDLL_API unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels);
    LBDLL_API void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel);
    LBDLL_API int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b);
}

// линкуем dll если она используетс€ в другом проекте
#ifndef LBDLL_EXPORTS
//#pragma comment(lib, "lbdll.lib")
#endif // LBDLL_EXPORTS
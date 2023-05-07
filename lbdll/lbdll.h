#pragma once

// image_processing_lib.cpp
#include <cstdint>
#include <functional>

// �������� ������������ ���� WinSocks2 ��� ������ � ��������
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// �������� ������������ ���� stb_image ��� ������ � �������������
#include "../shared/stb_image.h"



#ifdef LBDLL_EXPORTS
#define LBDLL_API __declspec(dllexport)
#else
#define LBDLL_API __declspec(dllimport)
#endif


// ��������� ������� ��� ������� �� ������������ ����������
extern "C" {
    LBDLL_API bool send_image(const char* ip_address, uint16_t port, const unsigned char* image_data, int width, int height, int channels);
    LBDLL_API unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels);
    LBDLL_API void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel);
    LBDLL_API int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b);
}

// ������� dll ���� ��� ������������ � ������ �������
#ifndef LBDLL_EXPORTS
//#pragma comment(lib, "lbdll.lib")
#endif // LBDLL_EXPORTS
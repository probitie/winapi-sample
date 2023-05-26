#pragma once

// do not delete "unused" includes as they're referenced by other subprojects
// TODO this may be a bad practice
// but in the other hand it incapsulates winsock connection and other libs

#include <cstdint>
#include <functional>
#include <iostream>
#include <thread>


#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "stb_image.h"


#define LB_IMG "./testimg.jpg" // in build directory

#define LB_HOST "127.0.0.1" //
#define LB_PORT 8888	// port for sending image


#define LB_ASSERT(statement, msg_on_fail) \
	if (!(statement)) \
	{\
		std::cout << (msg_on_fail) << "\n";\
		std::terminate();\
	}

#ifdef LBDLL_EXPORTS
#define LBDLL_API __declspec(dllexport)
#else
#define LBDLL_API __declspec(dllimport)
#endif


extern "C" {

	// TODO use remove_cv and pass image as const
	/**
	 * \brief sends a JPEG loaded with stb_image
	 * \details it does not affect image_data 
	 *          also it is blocking function and it guarantees
	 *          that the whole data is received
	 * \param ip_address where to send. most likely it will be localhost - 127.0.0.1:
	 *			         Use macro LB_HOST if you don't have any reason to specify the host directly
	 * \param port port the port: Use macro LB_PORT
	 *             if you don't have any reason to specify the port directly
	 * \param image_data stb_image image data
	 * \param width stb_image image param
	 * \param height stb_image image param
	 * \param channels stb_image image param
	 */
    LBDLL_API bool send_image(const char* ip_address, uint16_t port, unsigned char* image_data, int width, int height, int channels);

	/**
	 * \brief receives a JPEG loaded with stb_image
	 * \details Attention! It allocates data on HEAP
	 *          also it is blocking function and it guarantees
	 *          that the whole data is received
	 * \param port port where to listen
	 * \param width stb_image image param
	 * \param height stb_image image param
	 * \param channels stb_image image param
	 */
	LBDLL_API unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels);

	/**
	 * \brief updates image pixels with new values
	 * \param image_data stb_image image data
	 * \param width stb_image image param
	 * \param height stb_image image param
	 * \param channels stb_image image param
	 */
	LBDLL_API void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel);


	/**
	 * \brief counts pixels with the same color as that is being passed
	 * \param image_data stb_image image data
	 * \param width stb_image image param
	 * \param height stb_image image param
	 * \param channels stb_image image param
	 */
	LBDLL_API int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b);
}

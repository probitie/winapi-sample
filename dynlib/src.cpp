#include "common.h"

// Объявляем функции для экспорта из динамической библиотеки
extern "C" {
    __declspec(dllexport) bool send_image(const char* ip_address, uint16_t port, const unsigned char* image_data, int width, int height, int channels);
    __declspec(dllexport) unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels);
    __declspec(dllexport) void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel);
    __declspec(dllexport) int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b);
}

// Реализация функций
bool send_image(const char* ip_address, uint16_t port, const unsigned char* image_data, int width, int height, int channels) {
    // Инициализация Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return false;
    }

    // Создание сокета
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    // Настройка адреса и порта
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &server_address.sin_addr);

    // Подключение к серверу
    if (connect(sock, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return false;
    }

    // Отправка размеров изображения
    int dimensions[3] = { width, height, channels };
    send(sock, (const char*)dimensions, sizeof(dimensions), 0);

    // Отправка данных изображения
    send(sock, (const char*)image_data, width * height * channels, 0);

    // Закрытие сокета и выход
    closesocket(sock);
    WSACleanup();
    return true;
}

unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels) {
    // Инициализация Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return nullptr;
    }

    // Создание сокета
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return nullptr;
    }

    //

#include "common.h"

// ��������� ������� ��� �������� �� ������������ ����������
extern "C" {
    __declspec(dllexport) bool send_image(const char* ip_address, uint16_t port, const unsigned char* image_data, int width, int height, int channels);
    __declspec(dllexport) unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels);
    __declspec(dllexport) void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel);
    __declspec(dllexport) int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b);
}

// ���������� �������
bool send_image(const char* ip_address, uint16_t port, const unsigned char* image_data, int width, int height, int channels) {
    // ������������� Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return false;
    }

    // �������� ������
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    // ��������� ������ � �����
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &server_address.sin_addr);

    // ����������� � �������
    if (connect(sock, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return false;
    }

    // �������� �������� �����������
    int dimensions[3] = { width, height, channels };
    send(sock, (const char*)dimensions, sizeof(dimensions), 0);

    // �������� ������ �����������
    send(sock, (const char*)image_data, width * height * channels, 0);

    // �������� ������ � �����
    closesocket(sock);
    WSACleanup();
    return true;
}

unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels) {
    // ������������� Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return nullptr;
    }

    // �������� ������
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return nullptr;
    }

    //

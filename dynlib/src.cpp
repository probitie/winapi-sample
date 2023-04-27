#include "dynlib.h"

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

    // ��������� ������ � �����
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // �������� ������ � ������ � �����
    if (bind(sock, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return nullptr;
    }

    // ������������� �������� ����������
    if (listen(sock, 1) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return nullptr;
    }

    // �������� ��������� ����������
    SOCKET client_socket;
    sockaddr_in client_address;
    int client_address_length = sizeof(client_address);
    client_socket = accept(sock, (sockaddr*)&client_address, &client_address_length);
    if (client_socket == INVALID_SOCKET) {
        closesocket(sock);
        WSACleanup();
        return nullptr;
    }

    // ��������� �������� �����������
    int dimensions[3] = { 0 };
    recv(client_socket, (char*)dimensions, sizeof(dimensions), 0);
    *width = dimensions[0];
    *height = dimensions[1];
    *channels = dimensions[2];

    // ��������� ������ �����������
    unsigned char* image_data = new unsigned char[(*width) * (*height) * (*channels)];
    recv(client_socket, (char*)image_data, (*width) * (*height) * (*channels), 0);

    // �������� ������� � �����
    closesocket(client_socket);
    closesocket(sock);
    WSACleanup();
    return image_data;
}

void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel) {
    // �������� �� ���� �������� �����������
    for (int i = 0; i < width * height * channels; i += channels) {
        // �������� ������ ��� ��������� ������� �������, ��������� ���������� R, G � B �� ������
        process_pixel(image_data[i], image_data[i + 1], image_data[i + 2]);
    }
}

int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b) {
    int count = 0;
    // �������� �� ���� �������� �����������
    for (int i = 0; i < width * height * channels; i += channels) {
        // ���� ���� ������� ��������� � �������� ������, ����������� �������
        if (image_data[i] == r && image_data[i + 1] == g && image_data[i + 2] == b) {
            count++;
        }
    }
    return count;
}

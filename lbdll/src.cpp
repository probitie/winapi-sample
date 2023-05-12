#include "pch.h"

#include <thread>



/**
 * \brief Отправляет все данные по сокету.
 * \param sock Дескриптор сокета для отправки данных.
 * \param data Указатель на данные для отправки.
 * \param length Длина данных для отправки.
 * \param flags Флаги, определяющие поведение функции отправки.
 */
void sendAll(SOCKET sock, const char* data, int32_t length, int flags)
{
    int total{ 0 };
    int amount_sent{ 0 };

    int length_sent = send(sock, reinterpret_cast<const char*>(&length), sizeof(int32_t), flags);

    if (length_sent == 0 || length_sent == SOCKET_ERROR)
    {
        std::cerr << "error when sending data";
        std::abort();
    }

    while (total < length)
    {
        amount_sent = send(sock, data + total, length - total, flags);
        if (amount_sent == 0 || amount_sent == SOCKET_ERROR)
        {
            std::cerr << "error when sending data";
            std::abort();
        }
        total += amount_sent;
    }
}

/**
 * \brief Получает все данные по сокету.
 * \param sock Дескриптор сокета для получения данных.
 * \param flags Флаги, определяющие поведение функции получения.
 * \return Указатель на буфер с полученными данными.
 */
unsigned char* recvAll(SOCKET sock, int flags)
{
    int32_t length{};
    int length_recv = recv(sock, reinterpret_cast<char*>(&length), sizeof(int32_t), 0);

    if (length_recv == 0 || length_recv == SOCKET_ERROR)
    {
        std::cerr << "error when receiving data";
        std::abort();
    }

    LB_ASSERT(length >= 0, "length is negative");

    auto buffer = new unsigned char[length];

    int total{ 0 };
    int amount_recv{ 0 };
    while (total < length)
    {
        amount_recv = recv(sock, reinterpret_cast<char*>(buffer) + total, length - total, flags);
        if (amount_recv == 0 || amount_recv == SOCKET_ERROR)
        {
            std::cerr << "error when receiving data";
            std::abort();
        }
        total += amount_recv;
    }

    return buffer;
}

/**
 * \brief Отправляет изображение по сокету.
 * \param sock Дескриптор сокета для отправки изображения.
 * \param data Указатель на данные изображения.
 * \param width Ширина изображения.
 * \param height Высота изображения.
 * \param channels Количество каналов в изображении.
 */
void sendImg(SOCKET sock, uint8_t* data, int width, int height, int channels)
{
    int dimensions[3] = { width, height, channels };
    sendAll(sock, reinterpret_cast<char*>(dimensions), sizeof(dimensions), 0);

    // Отправка данных изображения
    sendAll(sock, reinterpret_cast<char*>(data), width * height * channels, 0);
}
/**
 * \brief Получает изображение по сокету.
 * \param sock Дескриптор сокета для получения изображения.
 * \param data Указатель, по которому будет записан указатель на полученные данные изображения.
 * \param width Указатель, по которому будет записана ширина изображения.
 * \param height Указатель, по которому будет записана высота изображения.
 * \param channels Указатель, по которому будет записано количество каналов в изображении.
 */
void recvImg(SOCKET sock, uint8_t** data, int* width, int* height, int* channels)
{
    // Получение размеров изображения
    int* dimensions = reinterpret_cast<int*>(recvAll(sock, 0));
    *width = dimensions[0];
    *height = dimensions[1];
    *channels = dimensions[2];
    delete[] dimensions;  // не забудьте освободить память!

    // Получение данных изображения
    *data = recvAll(sock, 0);
}

bool send_image(const char* ip_address, uint16_t port, unsigned char* image_data, int width, int height, int channels) {
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

    // Отправка изображения
    sendImg(sock, image_data, width, height, channels);

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

    // Настройка адреса и порта
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета к адресу и порту
    if (bind(sock, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return nullptr;
    }

    // Прослушивание входящих соединений
    if (listen(sock, 1) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return nullptr;
    }

    // Принятие входящего соединения
    SOCKET client_socket;
    sockaddr_in client_address;
    int client_address_length = sizeof(client_address);
    client_socket = accept(sock, (sockaddr*)&client_address, &client_address_length);
    if (client_socket == INVALID_SOCKET) {
        closesocket(sock);
        WSACleanup();
        return nullptr;
    }

    // Получение изображения
    unsigned char* image_data;
    recvImg(client_socket, &image_data, width, height, channels);

    // Закрытие сокетов и выход
    closesocket(client_socket);
    closesocket(sock);
    WSACleanup();
    return image_data;
}



// TODO сделать все отдельным классом чтобы можно было узнавать статус отправки через std::thread


void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel) {
    // Проходим по всем пикселям изображения
    for (int i = 0; i < width * height * channels; i += channels) {
        // Вызываем лямбду для обработки каждого пикселя, передавая компоненты R, G и B по ссылке
        process_pixel(image_data[i], image_data[i + 1], image_data[i + 2]);
    }
}

int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b) {
    int count = 0;
    // Проходим по всем пикселям изображения
    for (int i = 0; i < width * height * channels; i += channels) {
        // Если цвет пикселя совпадает с заданным цветом, увеличиваем счетчик
        if (image_data[i] == r && image_data[i + 1] == g && image_data[i + 2] == b) {
            count++;
        }
    }
    return count;
}

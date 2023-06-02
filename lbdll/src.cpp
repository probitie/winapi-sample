#include "pch.h"


/**
 * \brief Sends all data over a socket.
 * \param sock The socket descriptor for sending data.
 * \param data Pointer to the data to be sent.
 * \param length The length of the data to be sent.
 * \param flags Flags that determine the behavior of the send function.
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
 * \brief Receives all data over a socket.
 *
 * \details Attention! It allocates data on HEAP
 *          also it is blocking function and it guarantees
 *          that the whole data is received
 *
 *
 * \param sock The socket descriptor for receiving data.
 * \param flags Flags that determine the behavior of the receive function.
 * \return Pointer to the buffer containing the received data.
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
 * \brief Sends an image over a socket.
 * \param sock The socket descriptor for sending the image.
 * \param data Pointer to the image data.
 * \param width The width of the image.
 * \param height The height of the image.
 * \param channels The number of channels in the image.
 */
void sendImg(SOCKET sock, uint8_t* data, int width, int height, int channels)
{
    int dimensions[3] = { width, height, channels };

	// send image parameters - size and channels number
	sendAll(sock, reinterpret_cast<char*>(dimensions), sizeof(dimensions), 0);

    // send the image
    sendAll(sock, reinterpret_cast<char*>(data), width * height * channels, 0);
}


/**
 * \brief Receives an image over a socket.
 * \param sock The socket descriptor for receiving the image.
 * \param data Pointer where the pointer to the received image data will be stored.
 * \param width Pointer where the width of the image will be stored.
 * \param height Pointer where the height of the image will be stored.
 * \param channels Pointer where the number of channels in the image will be stored.
 */
void recvImg(SOCKET sock, uint8_t** data, int* width, int* height, int* channels)
{
    // Obtaining the dimensions of the image
    int* dimensions = reinterpret_cast<int*>(recvAll(sock, 0));
    *width = dimensions[0];
    *height = dimensions[1];
    *channels = dimensions[2];
    delete[] dimensions; // recvAll allocates memory

    *data = recvAll(sock, 0);
}

bool send_image(const char* ip_address, uint16_t port, unsigned char* image_data, int width, int height, int channels) {

    // init winsock
	WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return false;
    }

    // create tcp socket with Ipv4 address family
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    // address and port
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &server_address.sin_addr);

    // establish connection
    if (connect(sock, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return false;
    }

    sendImg(sock, image_data, width, height, channels);

    // clean up
    closesocket(sock);
    WSACleanup();
    return true;
}

unsigned char* receive_image(uint16_t port, int* width, int* height, int* channels) {
    // init winsock
	WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return nullptr;
    }

    // init socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return nullptr;
    }

    // address and port
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // listen

    if (bind(sock, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return nullptr;
    }

    if (listen(sock, 1) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return nullptr;
    }

    SOCKET client_socket;
    sockaddr_in client_address;
    int client_address_length = sizeof(client_address);
    client_socket = accept(sock, (sockaddr*)&client_address, &client_address_length);
    if (client_socket == INVALID_SOCKET) {
        closesocket(sock);
        WSACleanup();
        return nullptr;
    }

    // receive image
    unsigned char* image_data;
    recvImg(client_socket, &image_data, width, height, channels);

    // clean up
    closesocket(client_socket);
    closesocket(sock);
    WSACleanup();
    return image_data;
}

// Функция обработки каждого пикселя изображения
// process_pixel - это функция, которая принимает по ссылке компоненты R, G, B каждого пикселя.
void process_image(unsigned char* image_data, int width, int height, int channels, std::function<void(uint8_t& r, uint8_t& g, uint8_t& b)> process_pixel) {
    // Проходим через каждый пиксель
    for (int i = 0; i < width * height * channels; i += channels) {
        // Вызываем функцию process_pixel, передавая в нее компоненты B, G, R каждого пикселя (формат BGRA)
        process_pixel(image_data[i + 2], image_data[i + 1], image_data[i]);
    }
}

// Функция подсчета пикселей определенного цвета на изображении
// r, g, b - это компоненты цвета, который нужно найти.
int count_colored_pixels(const unsigned char* image_data, int width, int height, int channels, uint8_t r, uint8_t g, uint8_t b) {
    int count = 0;
    // Проходим через каждый пиксель
    for (int i = 0; i < width * height * channels; i += channels) {
        // Если цвет пикселя соответствует заданному цвету, увеличиваем счетчик
        // Здесь мы сравниваем компоненты B, G, R каждого пикселя с заданными значениями r, g, b (формат BGRA)
        if (image_data[i + 2] == r && image_data[i + 1] == g && image_data[i] == b) {
            count++;
        }
    }
    return count;
}


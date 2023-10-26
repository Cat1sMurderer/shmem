#include <iostream>
#include <winsock2.h>

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to a local address and port
    sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(1234);
    localAddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(sock, (sockaddr*)&localAddr, sizeof(localAddr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    result = listen(sock, SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen for connections: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Listening for incoming messages..." << std::endl;

    // Wait for incoming connections and messages
    while (true)
    {
        // Create a set of sockets to monitor for incoming data
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sock, &readSet);

        // Wait for data to arrive on the socket
        result = select(0, &readSet, nullptr, nullptr, nullptr);
        if (result == SOCKET_ERROR)
        {
            std::cerr << "select() failed: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            return 1;
        }

        // Check if there is data available on the socket
        if (FD_ISSET(sock, &readSet))
        {
            // Accept the incoming connection
            sockaddr_in clientAddr;
            int clientAddrSize = sizeof(clientAddr);
            SOCKET clientSock = accept(sock, (sockaddr*)&clientAddr, &clientAddrSize);
            if (clientSock == INVALID_SOCKET)
            {
                std::cerr << "Failed to accept incoming connection: " << WSAGetLastError() << std::endl;
                closesocket(sock);
                WSACleanup();
                return 1;
            }

            std::cout << "Accepted incoming connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;

            // Receive data from the client
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            result = recv(clientSock, buffer, sizeof(buffer), 0);
            if (result == SOCKET_ERROR)
            {
                std::cerr << "Failed to receive data: " << WSAGetLastError() << std::endl;
                closesocket(clientSock);
                closesocket(sock);
                WSACleanup();
                return 1;
            }

            // Print the received data
            std::cout << "Received message: " << buffer << std::endl;
            if (strcmp(buffer, "exit") == 0)
            {
                std::cout << "Received exit command, exiting..." << std::endl;
                closesocket(clientSock);
                break;
            }

            // Close the client socket
            // closesocket(clientSock);
        }
    }

    // Cleanup Winsock
    closesocket(sock);
    WSACleanup();

    return 0;
}
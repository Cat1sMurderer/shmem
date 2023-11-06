#include <iostream>
#include <winsock2.h>
#include "sharemem.h"
#include <iomanip> 

bool TcpHandler()
{
    int port = 15001;
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Create a socket
    // Create a listening socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to a local address and port
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen for incoming connections: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Listening for incoming connections on port " << port << "..." << std::endl;

    // Create a set of sockets to monitor for read events
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);

    // Loop to continuously listen for incoming connections and messages
    while (true)
    {
        // Wait for incoming connections and messages
        int result = select(0, &readSet, nullptr, nullptr, nullptr);
        if (result == SOCKET_ERROR)
        {
            std::cerr << "Failed to wait for incoming connections and messages: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            return 1;
        }

        // Check if there is an incoming connection
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

            // Loop to continuously receive and process messages from the client
            while (true)
            {
                // Receive data from the client
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                result = recv(clientSock, buffer, sizeof(buffer), 0);
                if (result == SOCKET_ERROR)
                {
                    std::cerr << "Failed to receive data: " << WSAGetLastError() << std::endl;
                    closesocket(clientSock);
                    break;
                }
                bool isNegotiationMsg = (buffer[0] == (char)0xff && buffer[1] == (char)0xfb && buffer[2] == (char)0x1f && buffer[3] == (char)0xff && buffer[4] == (char)0xfb && buffer[5] == (char)0x20 && buffer[6] == (char)0xff && buffer[7] == (char)0xfb && buffer[8] == (char)0x18 && buffer[9] == (char)0xff && buffer[10] == (char)0xfb && buffer[11] == (char)0x27 && buffer[12] == (char)0xff && buffer[13] == (char)0xfd && buffer[14] == (char)0x01 && buffer[15] == (char)0xff && buffer[16] == (char)0xfb && buffer[17] == (char)0x03 && buffer[18] == (char)0xff && buffer[19] == (char)0xfd && buffer[20] == (char)0x03);
                // Print the received data
                if((buffer[0] != '\r')&&(buffer[0] != '\0') && !isNegotiationMsg)
                {
                    if(strncmp(buffer, "pp", 2) == 0)
                    {
                        std::cout << "Received message: " << buffer << std::endl;
                        std::string message = "You sent: ";
                        message += buffer;
                        message += "\r\n";
                        send(clientSock, message.c_str(), message.length(), 0);
                        createSharedMemory(message);
                    }
                    else
                    {
                        // std::cout << "Invalid message: "<< buffer << std::endl;;
                        // std::cout << "Received invalid message: " << std::string(buffer, 10) << std::endl;
                            std::cout << "Received message (hex): ";
                        for (int i = 0; i < 20; i++)
                        {
                            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)buffer[i] << " ";
                        }
                        std::cout << std::endl;

                    }
                }

                // std::cout << "Received message: " << buffer << std::endl;

                // Check if the received message is "exit"
                if (strcmp(buffer, "exit") == 0)
                {
                    std::cout << "Received exit command, closing connection..." << std::endl;
                    closesocket(clientSock);
                    break;
                }

                // Clear the buffer
                memset(buffer, 0, sizeof(buffer));
                // Wait for 20ms
                Sleep(100);
            }
        }
    }

    // Close the listening socket and cleanup Winsock
    closesocket(sock);
    WSACleanup();
}
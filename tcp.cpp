#include <iostream>
#include <winsock2.h>

int main()
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
            char buffer2[1024];
            memset(buffer, 0, sizeof(buffer));
            result = recv(clientSock, buffer, sizeof(buffer), 0);
            if (result == SOCKET_ERROR)
            {
                std::cerr << "Failed to receive data: " << WSAGetLastError() << std::endl;
                closesocket(clientSock);
                break;
            }

            // Print the received data
            if((memcmp(buffer, buffer2, sizeof(buffer2))) == 0)//if the message is the same as the previous message, do nothing
            {
                //do nothing
            }
            else
            {
                if(strncmp(buffer, "pp", 2) == 0)
                {
                    std::cout << "Received message: " << buffer << std::endl;
                    std::string message = "You sent: ";
                    message += buffer;
                    message += "\r\n";
                    send(clientSock, message.c_str(), message.length(), 0);
                }
                else
                {
                    std::cout << "Invalid message " << std::endl;
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
            memccpy(buffer2, buffer, sizeof(buffer), sizeof(buffer2));
            

            // Wait for 20ms
            Sleep(20);
        }
    }
}

// Close the listening socket and cleanup Winsock
closesocket(sock);
WSACleanup();

return 0;
}
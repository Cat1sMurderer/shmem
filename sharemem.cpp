#include "sharemem.h"
#include <cstring>
#include <cstdlib>
#include <string>
#include <Windows.h>
#include <iostream>

#define MAX_SIZE 1024
#define SHARED_MEM_NAME "MySharedMemory"
#define INTERVAL_100 100
#define INTERVAL_1000 1000
#define INTERVAL_10000 10000

void createSharedMemory(LPVOID* lpMapAddress , HANDLE* hMapFile) 
{
    // Create a shared memory object
    *hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_SIZE + 1, SHARED_MEM_NAME);
    if (*hMapFile == NULL)
    {
        std::cerr << "Failed to create shared memory object: " << GetLastError() << std::endl;
        return;
    }

    // Map the shared memory object to the address space of the current process
    *lpMapAddress = MapViewOfFile(*hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, MAX_SIZE + 1);
    if (*lpMapAddress == NULL)
    {
        std::cerr << "Failed to map shared memory object: " << GetLastError() << std::endl;
        CloseHandle(*hMapFile);
        return;
    }

    std::cout << "Create shared memory." << std::endl;

}

void shareMessage(const std::string message , PVOID* lpMapAddress,HANDLE* hMapFile)
{
    if (*lpMapAddress == NULL)
    {
        std::cerr << "Failed to map shared memory object: " << GetLastError() << std::endl;
        CloseHandle(*hMapFile);
        return;
    }

    // Read the message from the shared memory
    char buffer[MAX_SIZE];
    // memset(buffer, 0, sizeof(buffer));
    // memcpy(buffer, lpMapAddress, sizeof(buffer));
    memcpy(lpMapAddress, message.c_str(), message.size() + 1);

    // Sleep for 10000 ms
    Sleep(100);

    // Unmap the shared memory object from the address space of the current process
    UnmapViewOfFile(lpMapAddress);

    // Print the message to the console
    std::cout << "shared message: " << message << std::endl;



    // Close the shared memory object
    CloseHandle(*hMapFile);
}

void closeSharedMemory(HANDLE* hMapFile)
{
    // Close the shared memory object
    CloseHandle(hMapFile);
    std::cout << "Close shared memory." << std::endl;
}
void readSharedMemory(std::string &message)
{
    // Open the shared memory object
    HANDLE rhMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEM_NAME);
    if (rhMapFile == NULL)
    {
        // std::cerr << "Failed to open shared memory object: " << GetLastError() << std::endl;
        return;
    }

    // Map the shared memory object to the address space of the current process
    LPVOID lpMapAddress = MapViewOfFile(rhMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (lpMapAddress == NULL)
    {
        // std::cerr << "Failed to map shared memory object: " << GetLastError() << std::endl;
        CloseHandle(rhMapFile);
        return;
    }

    // Read the message from the shared memory
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, lpMapAddress, sizeof(buffer));
    message = buffer;

    // Unmap the shared memory object from the address space of the current process
    UnmapViewOfFile(lpMapAddress);

    // Print the message to the console
    std::cout << "Received message: " << message << std::endl;

    // Sleep for 1000 ms
    // Sleep(1000);

    // Close the shared memory object
    CloseHandle(rhMapFile);
}
#include "sharemem.h"
#include <cstring>
#include <cstdlib>
#include <string>
#include <Windows.h>
#include <iostream>
void createSharedMemory(const std::string &message) 
{
    // Create a shared memory object
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, message.size() + 1, "MySharedMemory");
    if (hMapFile == NULL)
    {
        std::cerr << "Failed to create shared memory object: " << GetLastError() << std::endl;
        return;
    }

    // Map the shared memory object to the address space of the current process
    LPVOID lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, message.size() + 1);
    if (lpMapAddress == NULL)
    {
        std::cerr << "Failed to map shared memory object: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return;
    }

    // Write the message to the shared memory
    memcpy(lpMapAddress, message.c_str(), message.size() + 1);

    // Unmap the shared memory object from the address space of the current process
    UnmapViewOfFile(lpMapAddress);

    Sleep(10000);
    // Close the shared memory object
    CloseHandle(hMapFile);

    std::cout << "Message written to shared memory." << std::endl;

}
void test()
{
    // std::cout << "test" << std::endl;
}
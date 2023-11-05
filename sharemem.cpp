#include "sharemem.h"
#include <cstring>
#include <cstdlib>
#include <string>
#include <Windows.h>
#include <iostream>
void createSharedMemory(std::string &message) 
{
    HANDLE hMapFile;
    LPCTSTR pBuf;

    // Open the shared memory region
    hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MySharedMemory");

    if (hMapFile == NULL)
    {
        std::cerr << "Failed to open shared memory region: " << GetLastError() << std::endl;
    }

    // Map the shared memory region into the current process
    pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 1024);

    if (pBuf == NULL)
    {
        std::cerr << "Failed to map shared memory region: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
    }

    // Read data from the shared memory region
    strcpy((char*)pBuf, message.c_str());

    std::cout << "set \"" << (char*)pBuf << "\" to shared memory" << std::endl;

    // Unmap the shared memory region from the current process
    UnmapViewOfFile(pBuf);

    // Close the shared memory handle
    CloseHandle(hMapFile);

}
void test()
{
    std::cout << "test" << std::endl;
}
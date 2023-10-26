#include <iostream>
#include <Windows.h>
#include <cstring>

int main()
{
    HANDLE hMapFile;
    LPCTSTR pBuf;

    // Create the shared memory region
    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, "MySharedMemory");

    if (hMapFile == NULL)
    {
        std::cerr << "Failed to create shared memory region: " << GetLastError() << std::endl;
        return 1;
    }

    // Map the shared memory region into the current process
    pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 1024);

    if (pBuf == NULL)
    {
        std::cerr << "Failed to map shared memory region: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // Write data to the shared memory region
    std::string message = "Hello, world!";
    std::memcpy((void*)pBuf, message.c_str(), message.length() + 1);

    std::cout << "Wrote \"" << message << "\" to shared memory" << std::endl;

    
    // Wait for keyboard input before exiting
    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();

    //Modify the shared memory region
    std::string message2 = "New message";
    std::memcpy((void*)pBuf, message2.c_str(), message2.length() + 1);
    
    // Wait for keyboard input before exiting
    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();

    // Unmap the shared memory region from the current process
    UnmapViewOfFile(pBuf);

    // Close the shared memory handle
    CloseHandle(hMapFile);

    return 0;
}
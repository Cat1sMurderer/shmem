#include <iostream>
#include <Windows.h>

int main()
{
    HANDLE hMapFile;
    LPCTSTR pBuf;

    // Open the shared memory region
    hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MySharedMemory");

    if (hMapFile == NULL)
    {
        std::cerr << "Failed to open shared memory region: " << GetLastError() << std::endl;
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

    // Read data from the shared memory region
    std::string message = (char*)pBuf;

    std::cout << "Read \"" << message << "\" from shared memory" << std::endl;

    // Unmap the shared memory region from the current process
    UnmapViewOfFile(pBuf);

    // Close the shared memory handle
    CloseHandle(hMapFile);

    return 0;
}
#include <iostream>
#include <windows.h>

void readSharedMemory()
{
    // Open the shared memory object
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MySharedMemory");
    if (hMapFile == NULL)
    {
        std::cerr << "Failed to open shared memory object: " << GetLastError() << std::endl;
        return;
    }

    while (true)
    {
        // Map the shared memory object to the address space of the current process
        LPVOID lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (lpMapAddress == NULL)
        {
            std::cerr << "Failed to map shared memory object: " << GetLastError() << std::endl;
            CloseHandle(hMapFile);
            return;
        }

        // Read the message from the shared memory
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, lpMapAddress, sizeof(buffer));

        // Unmap the shared memory object from the address space of the current process
        UnmapViewOfFile(lpMapAddress);

        // Print the message to the console
        std::cout << "Received message: " << buffer << std::endl;

        // Sleep for 1000 ms
        Sleep(1000);
    }

    // Close the shared memory object
    CloseHandle(hMapFile);
}
int main()
{
    readSharedMemory();
    return 0;
}
#include <iostream>
#include <windows.h>
#include "sharemem.h"

void msgListener()
{
    std::string previousMessage = "";
    while(true)
    {
        std::string message = "";
        readSharedMemory(message);
        if((message != "") && (message != previousMessage))
        {
            std::cout << "Message received: " << message << std::endl; // avoding the same message
            previousMessage = message;
        }
        else
        {
           //do nothing 
        }
        Sleep(5);
    }
}


int main()
{
    msgListener();
    return 0;
}
#include <iostream>
#include <windows.h>
#include "sharemem.h"

void msgListener()
{
    int counter = 0;
    std::string previousMessage = "";
    while(true)
    {
        counter++;
        if (counter == 10000)
        {
            break;
        }
        
        std::string message = "";
        readSharedMemory(message);
        if(message != "" && message != previousMessage)
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
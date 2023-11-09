#include <iostream>
#include <Windows.h>
#include "tinyxml2.h"
#include <sstream>
#include "rci.h"
#include "sharemem.h"

#define RCI_CONFIG_FILE "cfg/rci_config.xml"
#define INTERVAL_5_MS 5

void readXML()
{
    std::string cfgPath = "";
    cfgPath = RCI_CONFIG_FILE;
    // Load the XML document from a file
    tinyxml2::XMLDocument doc;
    doc.LoadFile(cfgPath.c_str());
    // Get the DMI node
    tinyxml2::XMLElement* dmiNode = doc.FirstChildElement("root")->FirstChildElement("RCI")->FirstChildElement("common")->FirstChildElement("dmi_pc_event_device");
    if (dmiNode != nullptr)
    {
        strName = dmiNode->Attribute("name");
        dmiNode->QueryDoubleAttribute("scale", &scale);
    }
    else
    {
        std::cout << "Failed to read DMI node" << std::endl;
    }

    // Get the first screen node
    tinyxml2::XMLElement* screenNode = dmiNode->FirstChildElement("screen");
    if (screenNode != nullptr)
    {
        screenNode->QueryIntAttribute("x", &x_1);
        screenNode->QueryIntAttribute("y", &y_1);
        screenNode->QueryIntAttribute("width", &width1);
        screenNode->QueryIntAttribute("height", &height1);

        // Get the second screen node using the first screen node as a reference
        screenNode = screenNode->NextSiblingElement("screen");
        if (screenNode != nullptr)
        {
            screenNode->QueryIntAttribute("x", &x_2);
            screenNode->QueryIntAttribute("y", &y_2);
            screenNode->QueryIntAttribute("width", &width2);
            screenNode->QueryIntAttribute("height", &height2);
        }
    }

}
/**
 * @brief Simulates a touch event on a Windows screen at the specified position.
 * 
 * @param posX The X coordinate of the touch event.
 * @param posY The Y coordinate of the touch event.
 */
void executePP(int posX,int posY)
{
    bool isDoubleScreen =  false;
    int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, strName.c_str(), -1, NULL, 0);
    wchar_t* wideCharString = new wchar_t[wideCharLength];
    MultiByteToWideChar(CP_UTF8, 0, strName.c_str(), -1, wideCharString, wideCharLength);
    LPCWSTR lpWideCharString = wideCharString;
    std::wcout << lpWideCharString << std::endl;

    HWND dmiWindow = FindWindow(NULL,strName.c_str());
    delete[] wideCharString;
    if (dmiWindow != NULL)
    {
        if ((strName == "D_CAB_A") || (strName == "D_CAB_B"))
        {
            isDoubleScreen = true; // the windows title is indicates double screen
            std::cout << "Double screen" << std::endl;
        }
        // Maximize the window
        if (IsIconic(dmiWindow))
        {
            ShowWindow(dmiWindow, SW_RESTORE);
        }
        // Get the dimensions of the window's bounding rectangle
        RECT windowRect;
        GetWindowRect(dmiWindow, &windowRect);
        std::cout << "Window dimensions: " << (windowRect.right - windowRect.left) << " x " << (windowRect.bottom - windowRect.top) << std::endl;

        // Get the x and y positions from the user
        std::string input_data;
        int x = posX;
        int y = posY;
        x = x*scale;
        y = y*scale;
        y = y+27; // add the additional pixel to x axis that dedicated for windows bar
        if(isDoubleScreen)// the double screen
        {
            if(x > x_1 + width1 ) // the given x is greater than the maximum size of the screen 1
            {
                x += x_2; // extended the screen one with start area of screen 2
            }
            else
            {
                x += x_1; // if the given x is in range of screen 1 area then add the start area of screen 1 with the config
            }
            y += y_1; // the y axis for both screen are the same
        }
        std::cout << "x: " << x << " y: " << y << std::endl;

        if (IsIconic(dmiWindow)) // always chaeck if the windows is minimized
        {
            ShowWindow(dmiWindow, SW_RESTORE);
            GetWindowRect(dmiWindow, &windowRect);
            SetForegroundWindow(dmiWindow);
            //set foucs to the window in case it is minimized
            SetCursorPos(windowRect.left, windowRect.top);
            mouse_event(MOUSEEVENTF_LEFTDOWN, windowRect.left , windowRect.top, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, windowRect.left , windowRect.top, 0, 0);
        }
        SetForegroundWindow(dmiWindow);

        // Calculate the position to click on the DMI window
        double clickX = windowRect.left + x;
        double clickY = windowRect.top + y;

        // // Send a left mouse button click message to the specified position


        SetCursorPos(clickX, clickY);
        mouse_event(MOUSEEVENTF_LEFTDOWN, clickX, clickY, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, clickX, clickY, 0, 0);
    }
    else
    {
        std::cout << "DMI window not found" << std::endl;
    }

}
//Listen to incomming message from Tcp server via shared memory
/**
 * @brief This function listens for messages in a shared memory region named "TcpToRciSharedMemory" and prints them to the console.
 * 
 * @details The function reads the shared memory region and checks if the message is different from the previous message received. If it is, the message is printed to the console. If it is not, the function does nothing. The function runs in an infinite loop with a sleep interval of 5 milliseconds between iterations.
 */
void msgListener()
{
    std::string previousMessage = "";
    while(true)
    {
        std::string message = "";
        readSharedMemory(message , "TcpToRciSharedMemory");
        if((message != "") && (message != previousMessage))
        {
            std::cout << "recieve message: " << message << std::endl;

            if(strncmp(message.c_str(), "pp", 2) == 0)
            {
                if(PPcommandValidation(message))
                {
                    std::stringstream ss(message);
                    std::string token;
                    int x = 0, y = 0;
                    ss >> token >> x >> y;

                    // Check if the extraction was successful and if the remaining stream is empty
                    if (ss.fail() || !ss.eof())
                    {
                        std::cout << "Invalid message format: " << message << std::endl;
                    }
                    else
                    {
                        std::cout << "x = " << x << ", y = " << y << std::endl;
                        executePP(x,y);
                    }            
                }
            }
            previousMessage = message;
        }
        else
        {
            // do nothing 
        }
        Sleep(INTERVAL_5_MS);
    }
}
bool PPcommandValidation(std::string command) 
{
    bool isValid = false;
    std::string delimiter = " ";
    std::string token = command.substr(0, command.find(delimiter));
    if(token == "pp")
    {
        std::string remaining = command.substr(command.find(delimiter) + 1);
        std::istringstream iss(remaining);
        int x, y;
        if ((iss >> x) && (iss >> y) && iss.eof())
        {
            isValid = true;
        }
    }
    return isValid;
}
int main()
{
    readXML();
    std::cout << "Screen 1: x = " << x_1 << ", y = " << y_1 << ", width = " << width1 << ", height = " << height1 << std::endl;
    std::cout << "Screen 2: x = " << x_2 << ", y = " << y_2 << ", width = " << width2 << ", height = " << height2 << std::endl;
    std::cout << "Scale: " << scale << std::endl;
    std::cout << "Name: " << strName << std::endl;
    msgListener();
    return 0;
}
#include <iostream>
#include <Windows.h>
#include "tinyxml2.h"
#include "rci.h"

#define RCI_CONFIG_FILE "cfg/rci_config.xml"

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
void touchWinScreen()
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

        // Loop to repeatedly take inputs and click on the DMI window
        while (true)
        {
            // Get the x and y positions from the user
            std::string input_data;
            std::getline(std::cin, input_data);
            if (input_data == "exit")
            {
                break;
            }

            int x, y;
            sscanf_s(input_data.c_str(), "%d %d", &x, &y);
            std::cout << "x: " << x << " y: " << y << std::endl;
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
    }
    else
    {
        std::cout << "DMI window not found" << std::endl;
    }


}
int main()
{
    std::cout << "Hello World!" << std::endl;
    readXML();
    std::cout << "Screen 1: x = " << x_1 << ", y = " << y_1 << ", width = " << width1 << ", height = " << height1 << std::endl;
    std::cout << "Screen 2: x = " << x_2 << ", y = " << y_2 << ", width = " << width2 << ", height = " << height2 << std::endl;
    std::cout << "Scale: " << scale << std::endl;
    std::cout << "Name: " << strName << std::endl;
    // touchWinScreen();
    return 0;
}
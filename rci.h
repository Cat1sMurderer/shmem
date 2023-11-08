#include <iostream>
#include <Windows.h>
#include "tinyxml2.h"

std::string strName = "";
bool isDoubleScreen =  false;
//reead the congiguration file
double scale = 1.0;
int x_1 = 0;
int y_1 = 0;
int x_2 = 0;
int y_2 = 0;
int width1;
int height1;
int width2;
int height2;

void readXML();
void touchWinScreen();

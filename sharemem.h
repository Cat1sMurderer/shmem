#include <string>
#include <Windows.h>
#include <iostream>

#ifndef SHARED_MEM_H
#define SHARED_MEM_H

void createSharedMemory(LPVOID* lpMapAddress , HANDLE* hMapFile);
void shareMessage(const std::string message , PVOID* lpMapAddress,HANDLE* hMapFile);
void readSharedMemory(std::string &message);
void closeSharedMemory(HANDLE* hMapFile);

//  HANDLE hMapFile = NULL;

#endif // SHARED_MEM_H
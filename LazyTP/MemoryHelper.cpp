#include "MemoryHelper.h"

#include <iostream>

MemoryHelper::MemoryHelper()
{
    pID = NULL;
    processHandle = NULL;
}

MemoryHelper::MemoryHelper(DWORD pID)
{
    this->pID = pID;
    HANDLE processHandle = NULL;
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL)
    {
        std::cerr << "Failed to open process -- invalid handle" << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
        throw "Failed to open process";
    }
    else
    {
        //std::cout << "MemoryHelper:: process handle sucessfully created!" << std::endl;
        this->processHandle = processHandle;
    }
}


MemoryHelper::~MemoryHelper()
{
    CloseHandle(this->processHandle);
}


uintptr_t MemoryHelper::GetAddress(uintptr_t AddressOfCall, int index, int length)
{
    if (!AddressOfCall)
        return 0;

    long delta;
    ReadProcessMemory(processHandle, (void *) (AddressOfCall + index), &delta, sizeof(long), NULL);
    return AddressOfCall + delta + length;
}

void MemoryHelper::SetpID(DWORD pID)
{
    this->pID = pID;
}

DWORD MemoryHelper::GetpID()
{
    return this->pID;
}

HANDLE MemoryHelper::GetprocessHandle()
{
    return this->processHandle;
}

uintptr_t MemoryHelper::GetModuleBaseAddress(TCHAR *lpszModuleName)
{
    uintptr_t dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
    MODULEENTRY32 ModuleEntry32 = {0};
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32))
    {
        do
        {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
            {
                dwModuleBaseAddress = (uintptr_t) ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));


    }
    pSize = ModuleEntry32.modBaseSize;
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

uintptr_t MemoryHelper::GetDynamicAddress(uintptr_t baseAddress, vector<int> offsets)
{
    uintptr_t dynamicAddress = baseAddress;
    for (int i = 0; i < offsets.size(); i++)
    {
        ReadProcessMemory(this->processHandle, (void*) (dynamicAddress + offsets[i]), &dynamicAddress,
                          sizeof(uintptr_t), NULL);
        //std::cout << "Current Adress: " << std::hex << healthAddress << std::endl;
    }
    dynamicAddress += offsets[offsets.size() - 1];
    return dynamicAddress;
}

uintptr_t MemoryHelper::SetpBaseAddress(TCHAR *moduleName)
{
    this->pBaseAddress = this->GetModuleBaseAddress(moduleName);
    return this->pBaseAddress;
}

uintptr_t MemoryHelper::GetAddressFromSignature(vector<int> signature)
{
    if (this->pBaseAddress == NULL || this->processHandle == NULL)
    {
//        cout << dec << "Requirements not fullfilled" << endl;
        return NULL;
    }
    std::vector<unsigned char> memBuffer(this->pSize);
    if (!ReadProcessMemory(this->processHandle, reinterpret_cast<LPCVOID>(this->pBaseAddress), memBuffer.data(),
                           this->pSize, NULL))
    {
//        std::cout << GetLastError() << std::endl;
        return NULL;
    }
    for (int i = 0; i < this->pSize; i++)
    {
        for (uintptr_t j = 0; j < signature.size(); j++)
        {
            if (signature.at(j) != -1 && signature[j] != memBuffer[i + j])
                //std::cout << std::hex << signature.at(j) << std::hex << memBuffer[i + j] << std::endl;
                break;
//			if(signature[j] == memBuffer[i + j] && j>0)
//				std::cout << std::hex << int(signature[j]) << std::hex << int(memBuffer[i + j]) << j <<std::endl;
            if (j + 1 == signature.size())
                return this->pBaseAddress + i;
        }
    }
    return NULL;
}
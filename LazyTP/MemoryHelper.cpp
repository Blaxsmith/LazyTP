#include "MemoryHelper.h"
#include <qdebug.h>


MemoryHelper::MemoryHelper(DWORD pID, const wchar_t* moduleName)
{
    processID = pID;
    processSize = 0;
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (processHandle == INVALID_HANDLE_VALUE || processHandle == nullptr)
    {
        qDebug() << "Failed to open process -- invalid handle";
        qDebug() << "Error code: " << GetLastError();
        throw std::exception("Failed to open process");
    }
    pBaseAddress = GetModuleBaseAddress((TCHAR *) moduleName);
}

MemoryHelper::~MemoryHelper()
{
    CloseHandle(processHandle);
}


uintptr_t MemoryHelper::GetBaseAddress()
{
    return pBaseAddress;
}

uintptr_t MemoryHelper::GetAddress(uintptr_t AddressOfCall, int index, int length)
{
    if (!AddressOfCall)
        return 0;

    long delta;
    ReadProcessMemory(processHandle, (void *) (AddressOfCall + index), &delta, sizeof(long), nullptr);
    return AddressOfCall + delta + length;
}

uintptr_t MemoryHelper::GetModuleBaseAddress(TCHAR *lpszModuleName)
{
    uintptr_t dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
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
    processSize = ModuleEntry32.modBaseSize;
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

uintptr_t MemoryHelper::GetAddressFromSignature(vector<unsigned char> signature)
{
    if (pBaseAddress == NULL || processHandle == nullptr)
    {
        return NULL;
    }
    std::vector<unsigned char> memBuffer(processSize);
    if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(pBaseAddress), memBuffer.data(),
                           processSize, nullptr))
    {
        return NULL;
    }
    for (int i = 0; i < processSize; i++)
    {
        for (uintptr_t j = 0; j < signature.size(); j++)
        {
            if (signature[j] != memBuffer[i + j])
                break;
            if (j + 1 == signature.size())
                return pBaseAddress + i;
        }
    }
    return NULL;
}

uintptr_t MemoryHelper::GetAddressWithOffsets(uintptr_t baseAddress, const vector<unsigned short>& offsets, bool baseAddressIsPointer) const
{
    uintptr_t dynamicAddress = baseAddress;
    std::optional<uintptr_t> content;
    if(baseAddressIsPointer)
    {
        content = Read<uintptr_t>(baseAddress);
        if(!content.has_value())
            return NULL;
        else
            dynamicAddress = content.value();
    }
    for(int i = 0; i < offsets.size() - 1; i++)
    {
        content = Read<uintptr_t>(dynamicAddress + offsets.at(i));
        if(!content.has_value())
            return NULL;
        else
            dynamicAddress = content.value();
    }
    return dynamicAddress + offsets.at(offsets.size() - 1);
}

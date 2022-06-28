//
// Created by TKK on 26.06.2022.
//

#include "BnSWrapper.h"
#include <cstdio>
#include <algorithm>


using namespace std;


BnSWrapper::BnSWrapper() :
        m_sig({0x48, 0x8D, 0x4D, 0xEF, 0x48, 0x89, 0x4D, 0x77, 0x48, 0x89, 0x7D, 0xEF, 0x48, 0x89, 0x7D, 0xF7}),
        m_uGameViewPort(0x788),
        m_uGameInstance(0x80),
        m_uLocalPlayer(0x38),
        m_uLocalPlayerFirstObject(0x00),
        m_aPlayerController(0x30),
        m_aPawn(0x2C0),
        m_customTimeDilation(0x98),
        m_uScenceComponent(0x130),
        m_pos(0x1F0)
{
    DWORD pid = 0;
    HWND hWnd = FindWindowA(0, ("Blade & Soul"));
    GetWindowThreadProcessId(hWnd, &pid);
    m_pHelper = new MemoryHelper(pid);
    auto moduleName = L"BNSR.exe";
    m_pBnsBase = m_pHelper->SetpBaseAddress((TCHAR *) moduleName);
    m_sigAddress = m_pHelper->GetAddressFromSignature(m_sig);
    m_pEngineAddr = m_pHelper->GetAddress(m_sigAddress - 0x1C, 3, 7);
    m_pEngine = m_pHelper->Read<uintptr_t>(m_pEngineAddr).value();
    m_pViewPort = m_pHelper->Read<uintptr_t>(m_pEngine + m_uGameViewPort).value();
    m_pInstance = m_pHelper->Read<uintptr_t>(m_pViewPort + m_uGameInstance).value();
    m_pLocalPlayer = m_pHelper->Read<uintptr_t>(m_pInstance + m_uLocalPlayer).value();
    m_pOwnPlayer = m_pHelper->Read<uintptr_t>(m_pLocalPlayer + m_uLocalPlayerFirstObject).value();
    m_pPlayerController = m_pHelper->Read<uintptr_t>(m_pOwnPlayer + m_aPlayerController).value();
    m_pPawn = m_pHelper->Read<uintptr_t>(m_pPlayerController + m_aPawn).value();
    m_pScene = m_pHelper->Read<uintptr_t>(m_pPawn + m_uScenceComponent).value();
    m_pPos = m_pScene + m_pos;

    float speed = m_pHelper->Read<float>(m_pPawn + m_customTimeDilation).value();
//    cout << "BnS: " << hex << m_pBnsBase << endl;
//    cout << "Engine: " << hex << m_pEngine << endl;
}

BnSWrapper::~BnSWrapper()
{
    delete m_pHelper;
}

QVector3D BnSWrapper::getCurrentPos()
{
    auto xo = m_pHelper->Read<float>(m_pPos + 0x00);
    auto yo = m_pHelper->Read<float>(m_pPos + 0x04);
    auto zo = m_pHelper->Read<float>(m_pPos + 0x08);
    if(!xo.has_value() || !yo.has_value() || !zo.has_value())
        return QVector3D(0, 0, 0);

    float x = xo.value();
    float y = yo.value();
    float z = zo.value();
    return QVector3D(x, y, z);
}

void BnSWrapper::setCurrentPos(const QVector3D& pos)
{
    auto bx = m_pHelper->Write(m_pPos + 0x00, pos.x());
    auto by = m_pHelper->Write(m_pPos + 0x04, pos.y());
    auto bz = m_pHelper->Write(m_pPos + 0x08, pos.z());
//    if(!bx || !by || !bz)
//        qDebug() << "Could not write to memory";
}

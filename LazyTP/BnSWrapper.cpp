//
// Created by Staz on 26.06.2022.
//

#include "BnSWrapper.h"
#include <algorithm>
#include <QMessageBox>
#include <QApplication>


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
    HWND hWnd = FindWindowA(nullptr, ("Blade & Soul"));
    if(hWnd == nullptr)
    {
        QMessageBox msg;
        msg.setText("Could not find BnS");
        msg.exec();
        QApplication::exit(0);
        exit(0);
    }
    GetWindowThreadProcessId(hWnd, &pid);

    m_offsetsPlayer = {m_uGameViewPort, m_uGameInstance, m_uLocalPlayer, m_uLocalPlayerFirstObject};
    m_offsetsPlayerPos = {m_aPlayerController, m_aPawn, m_uScenceComponent, m_pos};

    m_pHelper = new MemoryHelper(pid, L"BNSR.exe");
    m_pBnsBase = m_pHelper->GetBaseAddress();
    m_sigAddress = m_pHelper->GetAddressFromSignature(m_sig);
    m_pEngineAddr = m_pHelper->GetAddress(m_sigAddress - 0x1C, 3, 7);


    m_pOwnPlayer = m_pHelper->GetAddressWithOffsets(m_pEngineAddr, m_offsetsPlayer, true);
    if(!m_pOwnPlayer)
    {
        QMessageBox msg;
        msg.setText("Could not find player object");
        msg.exec();
        QApplication::exit(0);
        exit(0);
    }

    refreshPlayerController();
}

BnSWrapper::~BnSWrapper()
{
    delete m_pHelper;
}

QVector3D BnSWrapper::getCurrentPos()
{
    refreshPlayerController();
    if(!m_pPos)
        QVector3D(0, 0, 0);
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
    refreshPlayerController();
    if(!m_pPos)
        return;
    auto bx = m_pHelper->Write(m_pPos + 0x00, pos.x());
    auto by = m_pHelper->Write(m_pPos + 0x04, pos.y());
    auto bz = m_pHelper->Write(m_pPos + 0x08, pos.z());
//    if(!bx || !by || !bz)
//        qDebug() << "Could not write to memory";
}

void BnSWrapper::refreshPlayerController()
{
    if(m_pOwnPlayer)
    {
        m_pPos = m_pHelper->GetAddressWithOffsets(m_pOwnPlayer, m_offsetsPlayerPos, true);
    }
}

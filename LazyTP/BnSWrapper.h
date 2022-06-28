//
// Created by TKK on 26.06.2022.
//

#ifndef LAZYTP_BNSWRAPPER_H
#define LAZYTP_BNSWRAPPER_H

#include <qt_windows.h>

#include "MemoryHelper.h"
#include <QVector3D>

class BnSWrapper
{
public:
    BnSWrapper();
    ~BnSWrapper();

    QVector3D getCurrentPos();
    void setCurrentPos(const QVector3D& pos);



private:
    MemoryHelper* m_pHelper;
    vector<int> m_sig;

    //Offsets
    int m_uGameViewPort;
    int m_uGameInstance;
    int m_uLocalPlayer;
    int m_uLocalPlayerFirstObject;
    int m_aPlayerController;
    int m_aPawn;
    int m_customTimeDilation;
    int m_uScenceComponent;
    int m_pos;

    //Addresses
    uintptr_t m_pBnsBase;
    uintptr_t m_sigAddress;
    uintptr_t m_pEngineAddr;
    uintptr_t m_pEngine;
    uintptr_t m_pViewPort;
    uintptr_t m_pInstance;
    uintptr_t m_pLocalPlayer;
    uintptr_t m_pOwnPlayer;
    uintptr_t m_pPlayerController;
    uintptr_t m_pPawn;
    uintptr_t m_pScene;
    uintptr_t m_pPos;
};


#endif //LAZYTP_BNSWRAPPER_H

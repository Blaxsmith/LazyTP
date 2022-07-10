//
// Created by Staz on 26.06.2022.
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
    void refreshPlayerController();



private:
    MemoryHelper* m_pHelper;
    vector<unsigned char> m_sig;

    vector<unsigned short> m_offsetsPlayer;
    vector<unsigned short> m_offsetsPlayerPos;

    //Offsets
    unsigned short m_uGameViewPort;
    unsigned short m_uGameInstance;
    unsigned short m_uLocalPlayer;
    unsigned short m_uLocalPlayerFirstObject;
    unsigned short m_aPlayerController;
    unsigned short m_aPawn;
    unsigned short m_customTimeDilation;
    unsigned short m_uScenceComponent;
    unsigned short m_pos;

    //Addresses
    uintptr_t m_pBnsBase;
    uintptr_t m_sigAddress;
    uintptr_t m_pEngineAddr;
    uintptr_t m_pOwnPlayer;
    uintptr_t m_pPos;
};


#endif //LAZYTP_BNSWRAPPER_H

//
// Created by TKK on 25.06.2022.
//

#ifndef LAZYTP_KEYBOARDHOOK_H
#define LAZYTP_KEYBOARDHOOK_H

#include <qt_windows.h>
#include <Qt>
#include <qobject.h>


class KeyboardHook : public QObject
{
    Q_OBJECT


public:
    KeyboardHook(int vkKey = 0x69);
    ~KeyboardHook();

    void keyDown(DWORD key);
    void keyUp(DWORD key);

signals:
    void keyPressed();

private:
    HHOOK hkkLowLevelKybd;
    int m_vkKey;
};


#endif //LAZYTP_KEYBOARDHOOK_H

//
// Created by TKK on 25.06.2022.
//

#include "KeyboardHook.h"

KeyboardHook* kbhRef;


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
        case WM_KEYDOWN:
            kbhRef->keyDown(PKBDLLHOOKSTRUCT(lParam)->vkCode);
            break;
        case WM_KEYUP:
            kbhRef->keyUp(PKBDLLHOOKSTRUCT(lParam)->vkCode);
            break;
        default:
            break;
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

KeyboardHook::KeyboardHook(int vkKey)
{
    kbhRef = this;
    hkkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    m_vkKey = vkKey;
}

KeyboardHook::~KeyboardHook()
{
    UnhookWindowsHookEx(hkkLowLevelKybd);
}

void KeyboardHook::keyDown(DWORD key)
{
    if(key == m_vkKey)
    {
        emit keyPressed();
    }
}

void KeyboardHook::keyUp(DWORD key)
{

}

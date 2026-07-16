#pragma once

// Multiple Platform 
#if defined(_WIN32)

#include  "../EngineMinimal.h"

class FWinMainCommandParameters
{
public:
    FWinMainCommandParameters(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPSTR lpCmdLine,_In_ int nCmdShow);

    HINSTANCE Instance;
    HINSTANCE PrevInstance;
    LPSTR CmdLine;
    int CmdShow;
    
};
#elif 

#endif

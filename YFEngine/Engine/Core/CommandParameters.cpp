#include "CommandParameters.h"

FWinMainCommandParameters::FWinMainCommandParameters(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
    int nCmdShow)
        :Instance(hInstance), PrevInstance(hPrevInstance), CmdLine(lpCmdLine), CmdShow(nCmdShow)
{
}

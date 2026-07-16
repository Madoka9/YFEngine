#pragma once
#include "CommandParameters.h"
class FEngine
{
public:
    FEngine();
    virtual ~FEngine() = default;
    //============================Engine Init============================
    virtual int PreInit(
#if defined(_WIN32)
    const FWinMainCommandParameters& InParameters
#endif
        ) = 0;
    virtual int Init() = 0;
    virtual int PostInit() = 0;

    //============================Engine Render============================
    virtual void Tick() = 0;

    //============================Engine Exit============================
    virtual int PreExit() = 0;
    virtual int Exit() = 0;
    virtual int PostExit() = 0;
    
};

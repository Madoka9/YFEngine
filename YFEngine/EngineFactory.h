#pragma once
#include "Engine/Core/Engine.h"

class FEngineFactory
{
public:
    FEngineFactory();
    static FEngine *CreateFEngine();
};

#pragma once
#include "Engine/Core/Engine.h"

class FEngineFactory
{
    FEngineFactory();
public:
    static FEngine *CreateFEngine();
};

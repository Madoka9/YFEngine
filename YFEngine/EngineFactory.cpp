#include "EngineFactory.h"

#if defined(_WIN32)
#include "Engine/Platform/Windows/WindowsEngine.h"
#elif 0
#endif

FEngineFactory::FEngineFactory()
{
    
}

FEngine* FEngineFactory::CreateFEngine()
{
#if defined(_WIN32)
    return new FWindowsEngine;
#elif 0
#endif
    
    
}

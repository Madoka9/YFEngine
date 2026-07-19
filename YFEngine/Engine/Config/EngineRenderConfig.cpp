#include "EngineRenderConfig.h"

// 静态成员变量定义（分配实际存储）
FEngineRenderConfig* FEngineRenderConfig::RenderConfig = nullptr;

FEngineRenderConfig::FEngineRenderConfig()
    :ScreenWidth(1280), ScreenHeight(720), RefreshRate(60), SwapChainBufferCount(2)
{
}

FEngineRenderConfig* FEngineRenderConfig::GetEngineRenderConfig()
{
    if(!RenderConfig)
    {
        RenderConfig = new FEngineRenderConfig();
    }
    return RenderConfig;
}

void FEngineRenderConfig::Destroy()
{
    if(RenderConfig)
    {
        delete RenderConfig;
        RenderConfig = nullptr;
    }
}

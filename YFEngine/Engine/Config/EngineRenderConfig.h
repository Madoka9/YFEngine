#pragma once



struct FEngineRenderConfig
{
    FEngineRenderConfig();
public:
    int ScreenWidth;
    int ScreenHeight;
    int RefreshRate;
    int SwapChainBufferCount; //前后Buffer 一般是2 

    static FEngineRenderConfig* GetEngineRenderConfig();
    static void Destroy();
private:
    static FEngineRenderConfig* RenderConfig;
};

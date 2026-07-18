#pragma once
#if defined(_WIN32)
#include "../../Core/Engine.h"
class FWindowsEngine : public FEngine
{
    FWindowsEngine();
public:
    int PreInit(const FWinMainCommandParameters& InParameters) override;
    int Init() override;
    int PostInit() override;
    void Tick() override;
    int PreExit() override;
    int Exit() override;
    int PostExit() override;

private:
    bool InitWindows(const FWinMainCommandParameters& InParameters);
    bool InitDirect3D();

protected:
    //显卡适配器工厂
    ComPtr<IDXGIFactory4> DXGIFactory = nullptr;
    //GPU 设备（Device）
    ComPtr<ID3D12Device> D3DDevice = nullptr;
    //CPU - GPU 同步围栏
    ComPtr<ID3D12Fence> D3DFence = nullptr;

    //命令队列
    ComPtr<ID3D12CommandQueue> D3DCommandQueue = nullptr;
    //命令分配器
    ComPtr<ID3D12CommandAllocator> D3DCommandAllocator = nullptr;
    //命令列表
    ComPtr<ID3D12GraphicsCommandList> D3DCommandList = nullptr;
    
    ComPtr<IDXGISwapChain> SwapChain = nullptr;
    
protected:
    HWND MainWindowsHandle = nullptr;
    UINT MSAA4xNumQualityLevels;
    bool bEnableMSAA4x;
    DXGI_FORMAT SwapChainBufferFormat;
};
#endif

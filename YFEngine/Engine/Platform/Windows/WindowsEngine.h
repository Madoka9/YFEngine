#pragma once
#if defined(_WIN32)
#include "../../Core/Engine.h"
class FWindowsEngine : public FEngine
{
public:
    FWindowsEngine();
    
    int PreInit(const FWinMainCommandParameters& InParameters) override;
    int Init(const FWinMainCommandParameters& InParameters) override;
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

    ComPtr<ID3D12DescriptorHeap> RTVDescriptorHeap = nullptr;
    ComPtr<ID3D12DescriptorHeap> DSVDescriptorHeap = nullptr;

    vector<ComPtr<ID3D12Resource>> SwapChainBuffers;
    ComPtr<ID3D12Resource> DepthStencilBuffer = nullptr;
protected:
    HWND MainWindowsHandle = nullptr;
    UINT MSAA4xNumQualityLevels;
    UINT RTVHeapIncrementSize;
    bool bEnableMSAA4x;
    DXGI_FORMAT BackBufferFormat;
    DXGI_FORMAT DepthStencilFormat;
};
#endif

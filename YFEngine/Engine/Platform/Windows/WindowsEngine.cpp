#include "WindowsEngine.h"

#include "../../Config/EngineRenderConfig.h"
#include "../Engine/Debug/EngineDebug.h"
#if defined(_WIN32)
#include "WindowsMessageProcessing.h"

FWindowsEngine::FWindowsEngine()
    : MSAA4xNumQualityLevels(0), RTVHeapIncrementSize(0), bEnableMSAA4x(false),
      BackBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM),DepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT)

{
    for (int i = 0; i < FEngineRenderConfig::GetEngineRenderConfig()->SwapChainBufferCount; i++)
    {
        SwapChainBuffers.push_back(ComPtr<ID3D12Resource>());
    }
}

int FWindowsEngine::PreInit(const FWinMainCommandParameters& InParameters)
{
    constexpr char LogPath[] = "../log";
    init_log_system(LogPath);
    EG_LOG("Engine Pre Initializing...")

    EG_LOG("Engine Pre Initializing Complete!")
    return 0;
}

int FWindowsEngine::Init(const FWinMainCommandParameters& InParameters)
{
    if(!InitWindows(InParameters))
    {
        return -1;
    }
    if(!InitDirect3D())
    {
        return -1;
    }
    EG_LOG("Engine Initializing Complete!")
    return 0;
}

int FWindowsEngine::PostInit()
{
    for(ComPtr<ID3D12Resource> SwapChainBuffer : SwapChainBuffers)
    {
        SwapChainBuffer.Reset();
    }
    DepthStencilBuffer.Reset();
    SwapChain->ResizeBuffers(
        FEngineRenderConfig::GetEngineRenderConfig()->SwapChainBufferCount,
        FEngineRenderConfig::GetEngineRenderConfig()->ScreenWidth,
        FEngineRenderConfig::GetEngineRenderConfig()->ScreenHeight,
        BackBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    D3D12_CPU_DESCRIPTOR_HANDLE RTVHeapHandle = RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    RTVHeapIncrementSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // 描述符堆增量大小，设备固定值，DX-32bytes，AMD-40Bytes
    for (UINT i = 0; i < FEngineRenderConfig::GetEngineRenderConfig()->SwapChainBufferCount; i++)
    {
        SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffers[i]));
        D3DDevice->CreateRenderTargetView(SwapChainBuffers[i].Get(), nullptr, RTVHeapHandle);
        RTVHeapHandle.ptr += RTVHeapIncrementSize;
    }
    //
    D3D12_RESOURCE_DESC ResourceDesc;
    ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    ResourceDesc.DepthOrArraySize = 1;
    ResourceDesc.Width = FEngineRenderConfig::GetEngineRenderConfig()->ScreenWidth;
    ResourceDesc.Height = FEngineRenderConfig::GetEngineRenderConfig()->ScreenHeight;
    ResourceDesc.Alignment = 0;
    ResourceDesc.MipLevels = 1;
    ResourceDesc.Format = DepthStencilFormat;
    ResourceDesc.SampleDesc.Count = bEnableMSAA4x ? 4 : 1;
    ResourceDesc.SampleDesc.Quality = bEnableMSAA4x ? MSAA4xNumQualityLevels - 1 : 0;
    ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    
    D3D12_CLEAR_VALUE OptimizedClearValue;
    OptimizedClearValue.Format = DepthStencilFormat;
    OptimizedClearValue.DepthStencil.Depth = 1.0f;
    OptimizedClearValue.DepthStencil.Stencil = 0;
    /*
     * CreateCommittedResource 是 DX12 里创建 GPU 资源的核心函数。在 DX11 里对应 CreateBuffer / CreateTexture2D 等一整套函数，DX12 都统一走这一个入口。
     */
   D3D12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
   HRESULT DSBCreateResult = D3DDevice->CreateCommittedResource(
        &HeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &ResourceDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &OptimizedClearValue,
        IID_PPV_ARGS(&DepthStencilBuffer));

    if(FAILED(DSBCreateResult))
    {
        EG_LOG_ERROR("Error: DepthStencilBuffer FWindowsEngine Failed")
        ANALYSIS_HRESULT(DSBCreateResult);
        return -1;
    }
    D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
    DSVDesc.Format = DepthStencilFormat;
    DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
    DSVDesc.Texture2D.MipSlice = 0;
    
    D3D12_CPU_DESCRIPTOR_HANDLE DSVHeapHandle = DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    D3DDevice->CreateDepthStencilView(DepthStencilBuffer.Get(), &DSVDesc, DSVHeapHandle);

    D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
    DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    D3DCommandList->ResourceBarrier(1, &Barrier);
    D3DCommandList->Close();

    ID3D12CommandList* CommandLists[] = { D3DCommandList.Get() };
    D3DCommandQueue->ExecuteCommandLists(_countof(CommandLists), CommandLists);
    EG_LOG("Engine post Initializing Complete!")
    return 0;
}

void FWindowsEngine::Tick()
{
}

int FWindowsEngine::PreExit()
{
    EG_LOG("Engine Pre Exit Complete!")
    return 0;
}

int FWindowsEngine::Exit()
{
    EG_LOG("Engine Exit Complete!")
    return 0;
}

int FWindowsEngine::PostExit()
{
    FEngineRenderConfig::Destroy();
    EG_LOG("Engine Post Exit Complete!")
    return 0;
}


bool FWindowsEngine::InitWindows(const FWinMainCommandParameters& InParameters)
{
    // --- 注册窗口类 ---
    WNDCLASSEX WindowClass = {};
    WindowClass.cbSize        = sizeof(WNDCLASSEX);
    WindowClass.style         = CS_VREDRAW | CS_HREDRAW;
    WindowClass.lpfnWndProc   = EngineWindowProcess;
    WindowClass.cbClsExtra    = 0;
    WindowClass.cbWndExtra    = 0;
    WindowClass.hInstance     = InParameters.Instance;
    WindowClass.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
    WindowClass.hIconSm       = LoadIcon(nullptr, IDI_APPLICATION);
    WindowClass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    WindowClass.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
    WindowClass.lpszMenuName  = nullptr;
    WindowClass.lpszClassName = L"YFEngineWindowClass";

    ATOM RegisterAtom = RegisterClassEx(&WindowClass);
    if(!RegisterAtom)
    {
        EG_LOG_ERROR("Error: RegisterClassEx FWindowsEngine Failed")
        MessageBox(nullptr, L"RegisterClassEx FWindowsEngine Failed", L"Error", MB_OK);
        return false;
    }

    RECT WindowsRect = {0, 0, FEngineRenderConfig::GetEngineRenderConfig()->ScreenWidth, FEngineRenderConfig::GetEngineRenderConfig()->ScreenWidth};

    AdjustWindowRect(&WindowsRect, WS_OVERLAPPED | WS_CAPTION, false);

    int WindowsWidth  = WindowsRect.right - WindowsRect.left;
    int WindowsHeight = WindowsRect.bottom - WindowsRect.top;

    HWND hWnd = CreateWindowEx
    (
        0,                          // 扩展样式
        L"YFEngineWindowClass",     // 窗口类名（必须与注册时的 lpszClassName 一致）
        L"YFEngine",                // 窗口标题
        WS_OVERLAPPEDWINDOW,        // 窗口样式
        CW_USEDEFAULT,              // x
        CW_USEDEFAULT,              // y
        WindowsWidth,               // 宽
        WindowsHeight,              // 高
        nullptr,                    // 父窗口
        nullptr,                    // 菜单
        InParameters.Instance,      // 实例句柄
        nullptr                     // 额外参数
    );
    if (!hWnd)
    {
        EG_LOG_ERROR("Error: CreateWindowEx FWindowsEngine Failed")
        MessageBox(nullptr, L"CreateWindowEx FWindowsEngine Failed", L"Error", MB_OK);
        return false;
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    
    MainWindowsHandle = hWnd;
    return true;
}

bool FWindowsEngine::InitDirect3D()
{
    //D3DDebug
    ComPtr<ID3D12Debug> D3D12Debug = nullptr;
    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug))))
    {
        D3D12Debug->EnableDebugLayer();
    }
    
    CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory));
    /*
         D3D_FEATURE_LEVEL_11_0  支持Direct3D 11.0 ， 包含Shader model 5
     */
    if(FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice))))
    {
        //传 null 会自动选默认的物理显卡创建设备，获取失败创建软件渲染适配器 WARP (Windows Advanced Rasterization Platform) 软件渲染器
        ComPtr<IDXGIAdapter1> WARPAdapter;
        DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&WARPAdapter));
        D3D12CreateDevice(WARPAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice));
    }
    /*
        D3D12_FENCE_FLAG_NONE = 0 — 创建一个 CPU 可见、单设备使用 的普通围栏
        D3D12_FENCE_FLAG_SHARED — 多个设备（或进程）能共享这个围栏，跨 D3DDevice 同步
        D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER — 跨显卡（如集显+独显）同步，需要显卡支持
        D3D12_FENCE_FLAG_NON_MONITORED_TYPE — 不需要 CPU 侧通过事件监听（少见）
        初学 DX12 的帧同步场景，99% 的情况都用 D3D12_FENCE_FLAG_NONE，后面的几个标记等到真的需要跨设备、跨显卡同步时才用得上。
     */
    HRESULT FenceCreateResult = D3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&D3DFence));
    if(FAILED(FenceCreateResult))
    {
        EG_LOG_ERROR("Error: CreateFence FWindowsEngine Failed")
        ANALYSIS_HRESULT(FenceCreateResult);
        return false;
    }
    D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
    QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //类型
    QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // flag 标记
    QueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; //优先级
    QueueDesc.NodeMask = 0; 
    
    D3DDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&D3DCommandQueue));
    D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&D3DCommandAllocator));
    HRESULT CMListResult = D3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3DCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&D3DCommandList));
    if(FAILED(CMListResult))
    {
        EG_LOG_ERROR("Error: CreateCommandList FWindowsEngine Failed")
        ANALYSIS_HRESULT(FenceCreateResult);
        return false;
    }
    D3DCommandList->Close();

    // Create SwapChain
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = FEngineRenderConfig::GetEngineRenderConfig()->ScreenWidth;
    SwapChainDesc.BufferDesc.Height = FEngineRenderConfig::GetEngineRenderConfig()->ScreenHeight;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = FEngineRenderConfig::GetEngineRenderConfig()->RefreshRate;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferDesc.Format = BackBufferFormat;
    SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//Buffer刷新扫描顺序
    SwapChainDesc.BufferCount = FEngineRenderConfig::GetEngineRenderConfig()->SwapChainBufferCount;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //作为渲染目标
    SwapChainDesc.Windowed = true; // 是否启用窗口模式
    SwapChainDesc.OutputWindow = MainWindowsHandle;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //对交换的Buffer做Discard处理
    SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //允许SwapChain进行切换的标记
    //多重采样
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MultiSamplingQualityLevels = {};
    MultiSamplingQualityLevels.NumQualityLevels = 0;
    MultiSamplingQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    MultiSamplingQualityLevels.SampleCount = 4;
    D3DDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MultiSamplingQualityLevels, sizeof(MultiSamplingQualityLevels));
    MSAA4xNumQualityLevels = MultiSamplingQualityLevels.NumQualityLevels;
    SwapChainDesc.SampleDesc.Count = bEnableMSAA4x ? 4 : 1;
    SwapChainDesc.SampleDesc.Quality = bEnableMSAA4x ? MSAA4xNumQualityLevels - 1 : 0;
    //SwapChain.Reset();
    HRESULT SCResult = DXGIFactory->CreateSwapChain(D3DCommandQueue.Get(), &SwapChainDesc, &SwapChain);
    if(FAILED(SCResult))
    {
        EG_LOG_ERROR("Error: CreateSwapChain FWindowsEngine Failed")
        ANALYSIS_HRESULT(SCResult);
        return false;
    }
    //创建资源描述符堆
    D3D12_DESCRIPTOR_HEAP_DESC RTVDesc = {};
    RTVDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    RTVDesc.NumDescriptors = 2;
    RTVDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    RTVDesc.NodeMask = 0;
    D3DDevice->CreateDescriptorHeap(&RTVDesc, IID_PPV_ARGS(&RTVDescriptorHeap));
    
    D3D12_DESCRIPTOR_HEAP_DESC DSVDesc = {};
    DSVDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DSVDesc.NumDescriptors = 1;
    DSVDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    DSVDesc.NodeMask = 0;
    D3DDevice->CreateDescriptorHeap(&DSVDesc, IID_PPV_ARGS(&DSVDescriptorHeap));
    return true;
}
#endif 

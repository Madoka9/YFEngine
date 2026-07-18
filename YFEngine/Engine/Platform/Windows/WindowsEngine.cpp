#include "WindowsEngine.h"

#include "../../Config/EngineRenderConfig.h"
#include "../Engine/Debug/EngineDebug.h"
#if defined(_WIN32)
#include "WindowsMessageProcessing.h"

FWindowsEngine::FWindowsEngine()
    : MSAA4xNumQualityLevels(0), bEnableMSAA4x(false), SwapChainBufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
{
}

int FWindowsEngine::PreInit(const FWinMainCommandParameters& InParameters)
{
    constexpr char LogPath[] = "../log";
    init_log_system(LogPath);
    EG_LOG("Engine Pre Initializing...")
    if(!InitWindows(InParameters))
    {
        return -1;
    }
    if(!InitDirect3D())
    {
        return -1;
    }
    EG_LOG("Engine Pre Initializing Complete!")
    return 0;
}

int FWindowsEngine::Init()
{
    EG_LOG("Engine Initializing Complete!")
    return 0;
}

int FWindowsEngine::PostInit()
{
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
    CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory));
    /*
        Feature Level 是 DX 版本的功能子集划分。
        ComPtr<ID3D12Device> D3DDevice;
        for (auto Level : FeatureLevels)
        {
            if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), Level, IID_PPV_ARGS(&D3DDevice))))
            {
                // D3DDevice 已创建，Feature Level 为 Level
                break;
            }
        }
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
    D3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&D3DFence));

    D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
    QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //类型
    QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // flag 标记
    QueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; //优先级
    QueueDesc.NodeMask = 0; 
    
    D3DDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&D3DCommandQueue));
    D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&D3DCommandAllocator));
    D3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3DCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&D3DCommandList));
    D3DCommandList->Close();

    // Create SwapChain
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = FEngineRenderConfig::GetEngineRenderConfig()->ScreenWidth;
    SwapChainDesc.BufferDesc.Height = FEngineRenderConfig::GetEngineRenderConfig()->ScreenHeight;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = FEngineRenderConfig::GetEngineRenderConfig()->RefreshRate;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferDesc.Format = SwapChainBufferFormat;
    SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//Buffer刷新扫描顺序
    SwapChainDesc.BufferCount = FEngineRenderConfig::GetEngineRenderConfig()->SwapChainBufferCount;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //作为渲染目标
    SwapChainDesc.Windowed = true; // 是否启用窗口模式
    SwapChainDesc.OutputWindow = MainWindowsHandle;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //对交换的Buffer做Discard处理
    SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //允许SwapChain进行切换的标记
    //多重采样
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MultiSamplingQualityLevels = {};
    MultiSamplingQualityLevels.NumQualityLevels = 4; // 4xMSAA
    MultiSamplingQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    D3DDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MultiSamplingQualityLevels, sizeof(MultiSamplingQualityLevels));
    MSAA4xNumQualityLevels = MultiSamplingQualityLevels.NumQualityLevels;
    SwapChainDesc.SampleDesc.Count = bEnableMSAA4x ? 4 : 1;
    SwapChainDesc.SampleDesc.Quality = bEnableMSAA4x ? MSAA4xNumQualityLevels - 1 : 0;

    
    //SwapChain.Reset();
    DXGIFactory->CreateSwapChain(D3DCommandQueue.Get(), &SwapChainDesc, &SwapChain);
    
    return false;
}
#endif 

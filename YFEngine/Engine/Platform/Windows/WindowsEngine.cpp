#include "WindowsEngine.h"
#if defined(_WIN32)
#include "WindowsMessageProcessing.h"
int FWindowsEngine::PreInit(const FWinMainCommandParameters& InParameters)
{
    if(!InitWindows(InParameters))
    {
        return -1;
    }
    return 0;
}

int FWindowsEngine::Init()
{
    return 0;
}

int FWindowsEngine::PostInit()
{
    return 0;
}

void FWindowsEngine::Tick()
{
}

int FWindowsEngine::PreExit()
{
    return 0;
}

int FWindowsEngine::Exit()
{
    return 0;
}

int FWindowsEngine::PostExit()
{
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
    WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    WindowClass.lpszMenuName  = nullptr;
    WindowClass.lpszClassName = L"YFEngineWindowClass";

    ATOM RegisterAtom = RegisterClassEx(&WindowClass);
    if(!RegisterAtom)
    {
        MessageBox(nullptr, L"RegisterClassEx FWindowsEngine Failed", L"Error", MB_OK);
        return false;
    }

    RECT WindowsRect = {0, 0, 1280, 720};

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
        MessageBox(nullptr, L"CreateWindowEx FWindowsEngine Failed", L"Error", MB_OK);
        return false;
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    
    m_hWnd = hWnd;
    return true;
}
#endif 

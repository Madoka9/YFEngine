#include "EngineMinimal.h"
#include <windows.h>
#include "Engine/Debug/Log/SimpleLog.h"
#include "EngineFactory.h"

int Init(FEngine* InEngine, _In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPSTR lpCmdLine,_In_ int nCmdShow)
{
    int ReturnValue = 0;
    ReturnValue = InEngine->PreInit(
#if defined(_WIN32)
FWinMainCommandParameters(hInstance,hPrevInstance,lpCmdLine,nCmdShow)
#elif 
#endif
    );
    if(ReturnValue != 0)
    {
        EG_LOG_ERROR("[%d] FEngine::PreInit Failed", ReturnValue)
        return ReturnValue;
    }
        
    ReturnValue = InEngine->Init();
    if(ReturnValue != 0)
    {
        EG_LOG_ERROR("[%d] FEngine::Init Failed", ReturnValue)
        return ReturnValue;
    }
        
    ReturnValue = InEngine->PostInit();
    if(ReturnValue != 0)
    {
        EG_LOG_ERROR("[%d] FEngine::PostInit Failed", ReturnValue)
        return ReturnValue;
    }
    return ReturnValue;
}

void Tick(FEngine* InEngine)
{
    InEngine->Tick(); 
}

int Exit(FEngine* InEngine)
{
    int ReturnValue = 0;
    ReturnValue = InEngine->PostExit();
    if(ReturnValue != 0)
    {
        EG_LOG_ERROR("[%d] FEngine::PostExit Failed", ReturnValue)
        return ReturnValue;
    }
        
    ReturnValue = InEngine->Exit();
    if(ReturnValue != 0)
    {
        EG_LOG_ERROR("[%d] FEngine::ExitFailed", ReturnValue)
        return ReturnValue;
    }
        
    ReturnValue = InEngine->PostExit();
    if(ReturnValue != 0)
    {
        EG_LOG_ERROR("[%d] FEngine::PostExit Failed", ReturnValue)
        return ReturnValue;
    }
    return ReturnValue;
}
/*| 常量 | 值 | 含义 |
|---|---|---|
| `SW_HIDE`             | 0 | 隐藏 |
| `SW_SHOWNORMAL`       | 1 | 正常显示 |
| `SW_SHOWMINIMIZED`    | 2 | 最小化启动 |
| `SW_SHOWMAXIMIZED`    | 3 | 最大化启动 |
| `SW_SHOW`             | 5 | 当前大小显示 |
| `SW_SHOWDEFAULT`      | 10 | 用系统中 `STARTUPINFO.wShowWindow` 的设定 |*/
// WinMain —— Windows 桌面应用入口点
//   - hInstance     : 当前应用程序实例句柄
//   - hPrevInstance : 不再使用，始终为 nullptr (Win32 历史遗留)
//   - lpCmdLine     : 命令行参数字符串（不含程序名）
//   - nCmdShow      : 窗口显示方式（最大化/最小化/正常等）
int WINAPI WinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPSTR lpCmdLine,_In_ int nCmdShow)
{
    int ReturnValue = 0;
    if(FEngine* Engine = FEngineFactory::CreateFEngine())
    {
        ReturnValue = Init(Engine,hInstance,hPrevInstance,lpCmdLine,nCmdShow);
        
        while (true)
        {
            Tick(Engine);
        }
        
        ReturnValue = Exit(Engine);
    }
    else
    {
        ReturnValue = 1;
    }
    return ReturnValue;
}
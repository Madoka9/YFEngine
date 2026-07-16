#include "EngineMinimal.h"
#include <windows.h>

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
int WINAPI WinMain(_In_ HINSTANCE hInstance,
                   _In_opt_ HINSTANCE hPrevInstance,
                   _In_ LPSTR lpCmdLine,
                   _In_ int nCmdShow)
{
    while (true)
    {
        
    }
    return 0;
}
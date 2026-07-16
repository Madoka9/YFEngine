#include "EngineMinimal.h"
#include <windows.h>

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
    // 占位：后续创建窗口、初始化 DX12、进入消息循环
    return 0;
}
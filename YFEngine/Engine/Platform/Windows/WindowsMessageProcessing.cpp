#include "WindowsMessageProcessing.h"

LRESULT EngineWindowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    case WM_SIZE:
        // DX12 swap chain resize
            // 见下面说明
                return 0;

    case WM_KEYDOWN:
        // 键盘输入
            return 0;

    case WM_SYSKEYDOWN:
        // 系统按键，比如 ALT+ENTER
            if (wParam == VK_RETURN && (lParam & 0x20000000))
            {
                // 切换全屏
                // ToggleFullscreen();
            }
        return 0;

    case WM_ACTIVATE:
        // 窗口激活/失活，可以暂停/恢复引擎
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

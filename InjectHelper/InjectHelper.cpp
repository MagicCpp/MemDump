// InjectHelper.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include <windows.h>
#include <tchar.h>
#include "InjectHelper.h"


LRESULT CALLBACK KeyboardProc(
    __in  int code,
    __in  WPARAM wParam,
    __in  LPARAM lParam
    )
{
    if (wParam == VK_HOME && (lParam & (1<<31)) == 0)
    {
        ::LoadLibrary(_T("G:\\HackTools\\MemDump\\Debug\\MemDumpDll.dll"));
    }
    return ::CallNextHookEx(0, code, wParam, lParam);
}
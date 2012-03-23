// MemDumpDll.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MemDumpDll.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CMemDumpDllApp

BEGIN_MESSAGE_MAP(CMemDumpDllApp, CWinApp)
END_MESSAGE_MAP()


// CMemDumpDllApp 构造

CMemDumpDllApp::CMemDumpDllApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMemDumpDllApp 对象

CMemDumpDllApp theApp;


// CMemDumpDllApp 初始化

BOOL CMemDumpDllApp::InitInstance()
{
	CWinApp::InitInstance();

    // 若要创建主窗口，此代码将创建新的框架窗口
    // 对象，然后将其设置为应用程序的主窗口对象
    CMainFrame* pFrame = new CMainFrame;
    if (!pFrame)
        return FALSE;
    m_pMainWnd = pFrame;
    // 创建并加载框架及其资源
    pFrame->LoadFrame(IDR_MAINFRAME,
        WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
        NULL);






    // 唯一的一个窗口已初始化，因此显示它并对其进行更新
    pFrame->ShowWindow(SW_SHOW);
    pFrame->UpdateWindow();
	return TRUE;
}

int CMemDumpDllApp::ExitInstance()
{
    return CWinApp::ExitInstance();
    ::FreeLibrary(AfxGetInstanceHandle());
}

// MemDumpDll.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "MemDumpDll.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CMemDumpDllApp

BEGIN_MESSAGE_MAP(CMemDumpDllApp, CWinApp)
END_MESSAGE_MAP()


// CMemDumpDllApp ����

CMemDumpDllApp::CMemDumpDllApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMemDumpDllApp ����

CMemDumpDllApp theApp;


// CMemDumpDllApp ��ʼ��

BOOL CMemDumpDllApp::InitInstance()
{
	CWinApp::InitInstance();

    // ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
    // ����Ȼ��������ΪӦ�ó���������ڶ���
    CMainFrame* pFrame = new CMainFrame;
    if (!pFrame)
        return FALSE;
    m_pMainWnd = pFrame;
    // ���������ؿ�ܼ�����Դ
    pFrame->LoadFrame(IDR_MAINFRAME,
        WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
        NULL);






    // Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
    pFrame->ShowWindow(SW_SHOW);
    pFrame->UpdateWindow();
	return TRUE;
}

int CMemDumpDllApp::ExitInstance()
{
    return CWinApp::ExitInstance();
    ::FreeLibrary(AfxGetInstanceHandle());
}

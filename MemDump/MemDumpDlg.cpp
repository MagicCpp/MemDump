
// MemDumpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MemDump.h"
#include "MemDumpDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void FormatErrorMsg(LPCTSTR pszErr, LPTSTR szErrMsg);
DWORD GetMainThreadByPID(DWORD dwProcessID);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMemDumpDlg 对话框




CMemDumpDlg::CMemDumpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMemDumpDlg::IDD, pParent)
    , m_hHook(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMemDumpDlg::~CMemDumpDlg()
{
    if (m_hHook)
        ::UnhookWindowsHookEx(m_hHook);
}

void CMemDumpDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_PROCESS, m_ctProcessList);
}

BEGIN_MESSAGE_MAP(CMemDumpDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_BTN_REFRESH, &CMemDumpDlg::OnBnClickedBtnRefresh)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROCESS, &CMemDumpDlg::OnNMDblclkListProcess)
    ON_BN_CLICKED(ID_BTN_INJECT, &CMemDumpDlg::OnBnClickedBtnInject)
END_MESSAGE_MAP()


// CMemDumpDlg 消息处理程序

BOOL CMemDumpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    m_ctProcessList.InsertColumn(0, _T("进程名"), LVCFMT_LEFT, 100);
    m_ctProcessList.InsertColumn(1, _T("PID"), LVCFMT_LEFT, 100);
    m_ctProcessList.InsertColumn(2, _T("路径"), LVCFMT_LEFT, 300);

    DWORD dwExStyle = m_ctProcessList.GetExtendedStyle();
    dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW;
    m_ctProcessList.SetExtendedStyle(dwExStyle);

    OnBnClickedBtnRefresh();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMemDumpDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMemDumpDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMemDumpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMemDumpDlg::OnBnClickedBtnRefresh()
{
    HANDLE hProcessSnap;
    HANDLE hModuleSnap;
    PROCESSENTRY32 pe32;
    MODULEENTRY32 me32;
    TCHAR szErrMsg[256] = {0};

    m_ctProcessList.DeleteAllItems();

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        FormatErrorMsg(_T("CreateToolhelp32Snapshot (of processes)"), szErrMsg);
        MessageBox(szErrMsg, _T("FAILED!"), MB_OK | MB_ICONERROR);
        return ;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        FormatErrorMsg(_T("Process32First"), szErrMsg); // show cause of failure
        MessageBox(szErrMsg, _T("FAILED!"), MB_OK | MB_ICONERROR);
        CloseHandle( hProcessSnap );          // clean the snapshot object
        return ;
    }

    // Now walk the snapshot of processes, and
    // fine notepad.exe process.
    do
    {
        LVITEM lvItem;
        TCHAR szText[256] = {0};

        lvItem.mask = LVIF_TEXT | LVIF_PARAM;
        lvItem.iItem = 0;
        lvItem.iSubItem = 0;
        lvItem.pszText = pe32.szExeFile;
        lvItem.lParam = pe32.th32ProcessID;
        m_ctProcessList.InsertItem(&lvItem);

        _stprintf(szText, _T("0x%08X"), pe32.th32ProcessID);
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = 0;
        lvItem.iSubItem = 1;
        lvItem.pszText = szText;
        m_ctProcessList.SetItem(&lvItem);

        if (pe32.th32ProcessID == 0)
        {
            continue;
        }

        hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pe32.th32ProcessID ); 
        if( hModuleSnap == INVALID_HANDLE_VALUE ) 
        { 
            continue;
        }

        //  Set the size of the structure before using it. 
        me32.dwSize = sizeof( MODULEENTRY32 ); 

        //  Retrieve information about the first module, 
        //  and exit if unsuccessful 
        if( !Module32First( hModuleSnap, &me32 ) ) 
        { 
            CloseHandle( hModuleSnap );     // Must clean up the snapshot object! 
            continue;
        } 

        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = 0;
        lvItem.iSubItem = 2;
        lvItem.pszText = me32.szExePath;
        m_ctProcessList.SetItem(&lvItem);

    } while( Process32Next(hProcessSnap, &pe32));

    CloseHandle( hProcessSnap );
}

void FormatErrorMsg(LPCTSTR pszErr, LPTSTR szErrMsg)
{
    TCHAR sysMsg[256];
    DWORD eNum = GetLastError( );
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, eNum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        sysMsg, 256, NULL );
    _stprintf(szErrMsg, _T("\n  WARNING: %s failed with error %d (%s)"), pszErr, eNum, sysMsg);
}

//查找主线程PID
DWORD GetMainThreadByPID(DWORD dwProcessID)  
{  
    THREADENTRY32 ThreadEntry;  
    ThreadEntry.dwSize = sizeof(THREADENTRY32);  
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessID);  
    if(Thread32First(hThreadSnapshot, &ThreadEntry))  
    {  
        do  
        {  
            if(ThreadEntry.th32OwnerProcessID == dwProcessID)  
            {  
                CloseHandle(hThreadSnapshot);  
                return ThreadEntry.th32ThreadID;  
            }  

        } while(Thread32Next(hThreadSnapshot, &ThreadEntry));  
        CloseHandle(hThreadSnapshot);  
    }  

    return NULL;  
}

void CMemDumpDlg::OnNMDblclkListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    // TODO: 在此添加控件通知处理程序代码
    OnBnClickedBtnInject();

    *pResult = 0;
}


void CMemDumpDlg::OnBnClickedBtnInject()
{
    int iItem = -1;
    DWORD dwProcessId = 0;
    DWORD dwThreadId = 0;

    POSITION pos = m_ctProcessList.GetFirstSelectedItemPosition();
    if (!pos) return ;

    iItem = m_ctProcessList.GetNextSelectedItem(pos);

    dwProcessId = m_ctProcessList.GetItemData(iItem);
    if (dwProcessId == 0) return ;
    
    dwThreadId = GetMainThreadByPID(dwProcessId);
    if (dwThreadId == 0) return ;

    HMODULE hDll = ::LoadLibrary(_T("InjectHelper.dll"));
    HOOKPROC pfntHook = (HOOKPROC) ::GetProcAddress(hDll, "KeyboardProc");
    if (pfntHook)
    {
        m_hHook = ::SetWindowsHookEx(WH_KEYBOARD, pfntHook, hDll, dwThreadId);
        if (m_hHook)
        {
            MessageBox(_T("注入成功"), _T("SUCCEED!"), MB_OK);
        }
    }
    if (hDll)
    {
        ::FreeLibrary(hDll);
    }
}


// MemDumpDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMemDumpDlg �Ի���




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


// CMemDumpDlg ��Ϣ�������

BOOL CMemDumpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

    // TODO: �ڴ���Ӷ���ĳ�ʼ������
    m_ctProcessList.InsertColumn(0, _T("������"), LVCFMT_LEFT, 100);
    m_ctProcessList.InsertColumn(1, _T("PID"), LVCFMT_LEFT, 100);
    m_ctProcessList.InsertColumn(2, _T("·��"), LVCFMT_LEFT, 300);

    DWORD dwExStyle = m_ctProcessList.GetExtendedStyle();
    dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW;
    m_ctProcessList.SetExtendedStyle(dwExStyle);

    OnBnClickedBtnRefresh();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMemDumpDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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

//�������߳�PID
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

    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
            MessageBox(_T("ע��ɹ�"), _T("SUCCEED!"), MB_OK);
        }
    }
    if (hDll)
    {
        ::FreeLibrary(hDll);
    }
}


// MemDumpDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CMemDumpDlg 对话框
class CMemDumpDlg : public CDialog
{
// 构造
public:
	CMemDumpDlg(CWnd* pParent = NULL);	// 标准构造函数
    ~CMemDumpDlg();

// 对话框数据
	enum { IDD = IDD_MEMDUMP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
    HHOOK m_hHook;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_ctProcessList;
    afx_msg void OnBnClickedBtnRefresh();
    afx_msg void OnNMDblclkListProcess(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedBtnInject();
};


// MemDumpDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CMemDumpDlg �Ի���
class CMemDumpDlg : public CDialog
{
// ����
public:
	CMemDumpDlg(CWnd* pParent = NULL);	// ��׼���캯��
    ~CMemDumpDlg();

// �Ի�������
	enum { IDD = IDD_MEMDUMP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
    HHOOK m_hHook;

	// ���ɵ���Ϣӳ�亯��
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

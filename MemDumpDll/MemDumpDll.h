// MemDumpDll.h : MemDumpDll DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������



// CMemDumpDllApp
// �йش���ʵ�ֵ���Ϣ������� MemDumpDll.cpp
//

class CMemDumpDllApp : public CWinApp
{
public:
	CMemDumpDllApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
    virtual int ExitInstance();
};


// MemDump.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMemDumpApp:
// �йش����ʵ�֣������ MemDump.cpp
//

class CMemDumpApp : public CWinApp
{
public:
	CMemDumpApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMemDumpApp theApp;
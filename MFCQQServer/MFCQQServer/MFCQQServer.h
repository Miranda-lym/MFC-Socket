
// MFCQQServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCQQServerApp: 
// �йش����ʵ�֣������ MFCQQServer.cpp
//

class CMFCQQServerApp : public CWinApp
{
public:
	CMFCQQServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCQQServerApp theApp;
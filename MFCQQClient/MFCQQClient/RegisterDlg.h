#pragma once

#include "MFCQQClientDlg.h"
// RegisterDlg �Ի���

class RegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RegisterDlg)

public:
	RegisterDlg(CMFCQQClientDlg * _pMainDlg);   // ��׼���캯��
	virtual ~RegisterDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Register };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    CString pwd;
    CString pwd2;
    CString userName;
    CMFCQQClientDlg * pMainDlg;
    afx_msg void OnBnClickedOk();
};

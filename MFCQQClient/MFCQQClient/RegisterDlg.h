#pragma once

#include "MFCQQClientDlg.h"
// RegisterDlg �Ի���

class RegisterDlg : public CMyDialog
{
	DECLARE_DYNAMIC(RegisterDlg)

public:
	RegisterDlg(CMFCQQClientDlg * _pMainDlg);   // ��׼���캯��
	virtual ~RegisterDlg();
    virtual void receData();

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
    ClientSocket* sock;
    CMFCQQClientDlg * pMainDlg;
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
};

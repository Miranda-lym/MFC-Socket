#pragma once

#include "MFCQQClientDlg.h"
// RegisterDlg 对话框

class RegisterDlg : public CMyDialog
{
	DECLARE_DYNAMIC(RegisterDlg)

public:
	RegisterDlg(CMFCQQClientDlg * _pMainDlg);   // 标准构造函数
	virtual ~RegisterDlg();
    virtual void receData();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Register };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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

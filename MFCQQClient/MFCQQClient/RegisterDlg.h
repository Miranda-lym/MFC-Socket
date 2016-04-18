#pragma once


// RegisterDlg 对话框

class RegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RegisterDlg)

public:
	RegisterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RegisterDlg();

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
    afx_msg void OnBnClickedOk();
};

#pragma once


// LoginDlg 对话框
class CMFCQQClientDlg;
class LoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LoginDlg)

public:
	//LoginDlg(CWnd* pParent = NULL);   // 标准构造函数
    LoginDlg(CMFCQQClientDlg* _pMainDlg);
	virtual ~LoginDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    CString userName;
    CString pwd;
    bool timeOut;
    bool loginFail;
    CMFCQQClientDlg* pMainDlg;
    static CString serverIP; //服务器的ip地址
    static unsigned serverPort; //服务器的端口号
    
    afx_msg void OnBnClickedOk();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedRegister();
};

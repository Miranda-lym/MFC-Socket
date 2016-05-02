#pragma once


// LoginDlg �Ի���
class CMFCQQClientDlg;
class LoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LoginDlg)

public:
	//LoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
    LoginDlg(CMFCQQClientDlg* _pMainDlg);
	virtual ~LoginDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    CString userName;
    CString pwd;
    bool timeOut;
    bool loginFail;
    CMFCQQClientDlg* pMainDlg;
    static CString serverIP; //��������ip��ַ
    static unsigned serverPort; //�������Ķ˿ں�
    
    afx_msg void OnBnClickedOk();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedRegister();
};

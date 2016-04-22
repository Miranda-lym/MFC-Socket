
// MFCQQClientDlg.h : 头文件
//

#pragma once
#include "ClientSocket.h"
#include "MyMsg.h"
#include "LoginDlg.h"
#include "afxwin.h"

// CMFCQQClientDlg 对话框
class CMFCQQClientDlg : public CDialogEx
{
    ClientSocket* pSock;
    bool m_connected; //标记是否已连接
    MyMsg msg;
    CString userName;
    CString pwd;
    LoginDlg login;
// 构造
public:
	CMFCQQClientDlg(CWnd* pParent = NULL);	// 标准构造函数
    ~CMFCQQClientDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCQQCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
public:
    void receData();
    int sendMsg(const CString & data, ClientSocket* sock);
    void updateEvent(const CString & title, const CString & content);
    void modifyStatus(const CString & status, bool _sleep=1);
    void showLoginDlg();
    CString getDateTime(bool haveDate=0);
    friend void LoginDlg::OnBnClickedOk();
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CComboBox m_cbMsgTo;
    afx_msg void OnBnClickedLogout();
    afx_msg void OnBnClickedSendMessage();
    virtual void OnOK();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSelChangeMsgTo();
};

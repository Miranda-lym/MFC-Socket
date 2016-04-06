
// MFCQQClientDlg.h : 头文件
//

#pragma once
#include "ClientSocket.h"

// CMFCQQClientDlg 对话框
class CMFCQQClientDlg : public CDialogEx
{
    ClientSocket* pSock;
    bool m_connected; //标记是否已连接
// 构造
public:
	CMFCQQClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCQQCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
public:
    void receData();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    void connectServer();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedConnectserver();
    CString m_receive;
    CString m_send;
    afx_msg void OnBnClickedSendMessage();
    virtual void OnOK();
};

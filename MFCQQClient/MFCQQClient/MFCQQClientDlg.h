
// MFCQQClientDlg.h : ͷ�ļ�
//

#pragma once
#include "ClientSocket.h"

// CMFCQQClientDlg �Ի���
class CMFCQQClientDlg : public CDialogEx
{
    ClientSocket* pSock;
    bool m_connected; //����Ƿ�������
// ����
public:
	CMFCQQClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCQQCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
public:
    void receData();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

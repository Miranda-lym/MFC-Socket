
// MFCQQServerDlg.h : ͷ�ļ�
//

#pragma once
#include"ServerSocket.h"


// CMFCQQServerDlg �Ի���
class CMFCQQServerDlg : public CDialogEx
{
// ����
public:
	CMFCQQServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCQQSERVER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
public:
    void addClient();
    void receData();
private:
    ServerSocket* listenSocket;
    UINT m_port; //�˿ں�
    static const int UserNumMax = 100; //�������û���

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOpenserver();
};

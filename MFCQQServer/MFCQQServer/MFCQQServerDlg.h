
// MFCQQServerDlg.h : ͷ�ļ�
//

#pragma once
#include"ServerSocket.h"
#include "../../MFCQQClient/MFCQQClient/MyMsg.h"
#include"mysql.h"
#include<map>


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
    bool isUserInfoValid(const CString & user,const CString & pwd);
private:
    ServerSocket* listenSocket;
    ServerSocket* lastClientSocket;
    UINT m_port; //�˿ں�
    static const int UserNumMax = 100; //�������û���
    MyMsg msg;
    MYSQL *conn;
    std::map<CString, CString> userInfoMap; //���û��������룩ӳ���
    CString userList; //�û��б����������û������û���¼ʱ���͹�ȥ
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
    CString sendData;
    afx_msg void OnBnClickedSendMsg();
    CString m_receiveData;
};

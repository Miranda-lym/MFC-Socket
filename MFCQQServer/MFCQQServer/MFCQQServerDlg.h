
// MFCQQServerDlg.h : 头文件
//

#pragma once
#include"ServerSocket.h"
#include "../../MFCQQClient/MFCQQClient/MyMsg.h"
#include "../../MFCQQClient/MFCQQClient/DB_Msg.hpp"
#include"mysql.h"
#include<map>


// CMFCQQServerDlg 对话框
class CMFCQQServerDlg : public CDialogEx
{
    // 构造
public:
    CMFCQQServerDlg(CWnd* pParent = NULL);	// 标准构造函数

    ~CMFCQQServerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum {
        IDD = IDD_MFCQQSERVER_DIALOG
    };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
public:
    void addClient();
    void receData(ServerSocket* sock);
    bool isUserInfoValid(const CString & user, const CString & pwd);
    int sendMsg(const CString & data, ServerSocket* sock);
    void updateEvent(const CString &title, const CString & content);
    CString getDateTime(bool haveDate = 0);
    void modifyStatus(const CString &status, bool _sleep);
private:
    typedef struct {
        ServerSocket* sock;
        bool heartbeat; //是否有心跳
    }S_UserInfo;
    ServerSocket* listenSocket;
    UINT m_port; //端口号
    static const int UserNumMax = 100; //最大监听用户量
    MyMsg msg;
    DB_Connector *pDB_UserInfo;
    std::map<CString, CString> userInfoMap; //（用户名，密码）映射表
    std::map<CString, S_UserInfo> userSockMap;//(用户名,用户附加信息（socket描述符，心跳）)的映射表
    CString userList; //用户列表，包含所有用户名，用户登录时发送过去
    DB_OfflineMsg* p_offlineMsg;
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
    int m_onlineNum;
    afx_msg void OnBnClickedOpenserver();
    afx_msg void OnBnClickedSendMsg();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};

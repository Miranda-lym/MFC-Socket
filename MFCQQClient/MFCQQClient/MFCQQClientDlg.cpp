
// MFCQQClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCQQClient.h"
#include "MFCQQClientDlg.h"
#include "afxdialogex.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum {
        IDD = IDD_ABOUTBOX
    };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCQQClientDlg 对话框



CMFCQQClientDlg::CMFCQQClientDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_MFCQQCLIENT_DIALOG, pParent)
    , m_receive(_T(""))
    , m_send(_T(""))
    , login(this)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    pSock = NULL;
    m_connected = false;
}

CMFCQQClientDlg::~CMFCQQClientDlg()
{
    CString dataToSend = msg.join("", TYPE[Logout], userName, "", "", pwd);
    pSock->Send(dataToSend, dataToSend.GetLength() + 1);//消息内容，消息长度
    pSock->Close();
    delete pSock;
}

void CMFCQQClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_Rece, m_receive);
    DDX_Text(pDX, IDC_Send, m_send);
    DDX_Control(pDX, IDC_msgTo, m_cbMsgTo);
}

void CMFCQQClientDlg::receData()
{
    char buffer[10000];
    if (pSock->Receive(buffer, sizeof(buffer)) != SOCKET_ERROR) {
        msg.load(buffer);
        if (msg.type == TYPE[UserList]) { //表示登录成功
            m_connected = true;
            for (int i; (i = msg.data.Find(";")) != -1;) { //即找不到；了，即用户列表结束
                CString user = msg.data.Left(i); //取左边i个字符组成的子串
                if (user != login.userName) {
                    m_cbMsgTo.AddString(user); //将用户名添加到下拉列表
                }
                msg.data = MyMsg::rightN(msg.data, i + 1);
            }
            m_cbMsgTo.SetCurSel(0);
            updateEvent("已连上服务器", msg.data);
        }
        else if (msg.type == TYPE[LoginFail]) {
            login.loginFail = true;
        }
        else if (msg.type == TYPE[ChatMsg]) {
            updateEvent(msg.fromUser, msg.data);
        }
        else {
            updateEvent("未知消息", buffer);
        }
    }
}

void CMFCQQClientDlg::updateEvent(const CString & title, const CString & content)
{
    CString str;
    if (content == "") {
        str=title + "\r\n";
    }
    else {
        str= title + ": " + content + "\r\n";
    }
    m_receive += str;
    //UpdateData(false); //因为要让它滚动到最后一行，UpdateData是实现不了的
    //用下面这四句实现，这里若不注释，那么每条消息会出现两次
    CEdit* pEvent = (CEdit*)GetDlgItem(IDC_Rece); //获取到界面中一个控件，控件ID由我们自己写
    int lastLine = pEvent->LineIndex(pEvent->GetLineCount()-1); //获取编辑框最后一行索引，该函数的参数必须是有效的索引值（下标），如有5行则有效的下标是0~4
    pEvent->SetSel(lastLine + 1, lastLine + 2, 0); //选择编辑框最后一行
    pEvent->ReplaceSel(str); //替换所选那一行的内容，本来下一行是没有内容的
}

BEGIN_MESSAGE_MAP(CMFCQQClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_ConnectServer, &CMFCQQClientDlg::OnBnClickedLogout)
    ON_BN_CLICKED(IDC_SendMessage, &CMFCQQClientDlg::OnBnClickedSendMessage)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CMFCQQClientDlg 消息处理程序

BOOL CMFCQQClientDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    showLoginDlg();

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCQQClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCQQClientDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCQQClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CMFCQQClientDlg::showLoginDlg()
{
    ShowWindow(SW_HIDE);
    if (login.DoModal() != IDOK) {
        exit(1);
    }
    ShowWindow(SW_SHOW);
    userName = login.userName;
    pwd = login.pwd;
}


void CMFCQQClientDlg::OnBnClickedLogout()
{
    CString dataToSend = msg.join("", TYPE[Logout], userName, "", "", pwd);
    pSock->Send(dataToSend, dataToSend.GetLength() + 1);//消息内容，消息长度
    pSock->Close();
    delete pSock;
    pSock = NULL;
    showLoginDlg();
}


void CMFCQQClientDlg::OnBnClickedSendMessage()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_connected) {
        UpdateData(true);
        if (m_send == "") {
            MessageBox("请输入要发送的消息", "温馨提示");
            return;
        }
        CString toUser;
        m_cbMsgTo.GetLBText(m_cbMsgTo.GetCurSel(),toUser);
        CString dataToSend = msg.join(m_send, TYPE[ChatMsg], userName, "", toUser);
        if (pSock->Send(dataToSend, dataToSend.GetLength() + 1) == SOCKET_ERROR) {
            MessageBox("发送失败", "温馨提示");
        }
        else {
            CString buffer = m_send;
            m_send = "";
            UpdateData(false);
            updateEvent("我", buffer);
        }
    }
    else {
        MessageBox("请先连接上服务器", "温馨提示");
    }
}


void CMFCQQClientDlg::OnOK()
{
    OnBnClickedSendMessage();
}


void CMFCQQClientDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    switch (nIDEvent) {
        case 0: //登录延时的检测
            KillTimer(0);
            MessageBox("登录超时", "温馨提示");
            showLoginDlg();
            break;
    }
    CDialogEx::OnTimer(nIDEvent);
}

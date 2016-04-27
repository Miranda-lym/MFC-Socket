
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
    , login(this)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    pSock = NULL;
    m_connected = false;
}

CMFCQQClientDlg::~CMFCQQClientDlg()
{
    CString dataToSend = msg.join("", TYPE[Logout], userName, "", "", pwd);
    sendMsg(dataToSend, pSock);
    pSock->Close();
    delete pSock;
}

void CMFCQQClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_msgTo, m_cbMsgTo);
}

void CMFCQQClientDlg::receData()
{
    char buffer[10000];
    if (pSock->Receive(buffer, sizeof(buffer)) != SOCKET_ERROR) {
        msg.load(buffer);
        if (msg.type == TYPE[UserList]) { //表示登录成功
            m_connected = true;
            m_cbMsgTo.ResetContent(); //清空下拉列表所有内容
            for (int i; (i = msg.data.Find(";")) != -1;) { //即找不到；了，即用户列表结束
                CString user = msg.data.Left(i); //取左边i个字符组成的子串
                if (user != login.userName) {
                    m_cbMsgTo.AddString(user); //将用户名添加到下拉列表
                }
                msg.data = MyMsg::rightN(msg.data, i + 1);
            }
            m_cbMsgTo.SetCurSel(0);
            modifyStatus("已连上服务器");
            SetTimer(1, 3000, NULL); // 设置心跳包
        }
        else if (msg.type == TYPE[LoginFail]) {
            login.loginFail = true;
        }
        else if (msg.type == TYPE[ChatMsg]) {
            updateEvent(msg.fromUser, msg.data);
        }
        else if (msg.type == TYPE[AddUserList]) {
            m_cbMsgTo.AddString(msg.data); //增加一条用户信息
        }
        else if (msg.type == TYPE[Squezze]) {
            KillTimer(1);
            pSock->Close();
            delete pSock;
            pSock = NULL;
            MessageBox("由于您在另外一处登录，现在将被强制下线", "温馨提示");
            showLoginDlg();
        }
        else if (msg.type == TYPE[OnlineState]) {
            if (msg.data == "1") {
                modifyStatus("该用户在线，您可直接发送消息！");
            }
            else {
                modifyStatus("该用户当前不在线，您可给他发送离线消息！");
            }
        }
        else if (msg.type == TYPE[Server_is_closed]) {
            KillTimer(1);
            modifyStatus("服务器已停止运行，请稍后重新连接至服务器！");
            MessageBox("服务器已停止运行，请稍后重新连接至服务器！");
            showLoginDlg();
        }
        else if (msg.type == TYPE[OfflineMsg]) {
            while (msg.data != "") {
                CString _from, _time, _content;
                int i = msg.data.Find(seperator);
                _from = msg.data.Left(i);
                msg.data = MyMsg::rightN(msg.data, i + 2);
                i = msg.data.Find(seperator);
                _time = msg.data.Left(i);
                msg.data = MyMsg::rightN(msg.data, i + 2);
                i = msg.data.Find(seperator);
                _content = msg.data.Left(i);
                msg.data = MyMsg::rightN(msg.data, i + 2);
                updateEvent(_from, _content, _time);
            }
            updateEvent("—————以上是离线消息—————", "", "");
        }
        else {
            updateEvent("未知消息", buffer);
        }
    }
    else {
        MessageBox(ClientSocket::getLastErrorStr(), "错误提示", MB_ICONERROR);
    }
}

int CMFCQQClientDlg::sendMsg(const CString & data, ClientSocket * sock)
{
    if (sock->Send(data, data.GetLength() + 1) == SOCKET_ERROR) {
        KillTimer(1); //和服务器连接出错，无法发送消息时关闭心跳包的发送
        MessageBox("发送消息失败：" + ClientSocket::getLastErrorStr(), "错误提示", MB_ICONERROR);
        return SOCKET_ERROR;
    }
    return 0;
}

void CMFCQQClientDlg::updateEvent(const CString & title, const CString & content, const CString setTime)
{
    CString curData;
    GetDlgItemText(IDC_Rece, curData); //将空间IDC_Rece中的内容放到变量curData中
    bool haveDate = (curData == ""); //若此时接收区为空，则加上日期
    CString str = getDateTime(haveDate) + " ";
    if (setTime != "-") {
        str = setTime + " ";
    }
    if (content == "") {
        str += title + "\r\n";
    }
    else {
        str += title + ": " + content + "\r\n";
    }
    //UpdateData(false); //因为要让它滚动到最后一行，UpdateData是实现不了的
    //用下面这四句实现，这里若不注释，那么每条消息会出现两次
    CEdit* pEvent = (CEdit*)GetDlgItem(IDC_Rece); //获取到界面中一个控件，控件ID由我们自己写
    int lastLine = pEvent->LineIndex(pEvent->GetLineCount() - 1); //获取编辑框最后一行索引，该函数的参数必须是有效的索引值（下标），如有5行则有效的下标是0~4
    pEvent->SetSel(lastLine + 1, lastLine + 2, 0); //选择编辑框最后一行
    pEvent->ReplaceSel(str); //替换所选那一行的内容，本来下一行是没有内容的
}

void CMFCQQClientDlg::modifyStatus(const CString & status, bool _sleep)
{
    HWND h = CreateStatusWindow(WS_CHILD | WS_VISIBLE, status, m_hWnd, 0);
    if (_sleep) {
        Sleep(50);
    }
    ::SendMessage(h, SB_SETBKCOLOR, 0, RGB(0, 125, 205)); //全局函数,则不会调用dlg类的成员函数，而是全局的成员函数
}

BEGIN_MESSAGE_MAP(CMFCQQClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_ConnectServer, &CMFCQQClientDlg::OnBnClickedLogout)
    ON_BN_CLICKED(IDC_SendMessage, &CMFCQQClientDlg::OnBnClickedSendMessage)
    ON_WM_TIMER()
    ON_CBN_SELCHANGE(IDC_msgTo, &CMFCQQClientDlg::OnSelChangeMsgTo)
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
    GetDlgItem(IDC_Send)->SetFocus(); //设置焦点在发送编辑框，即ID为IDC_Send
    return FALSE;  // 除非将焦点设置到控件，否则返回 TRUE
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
    SetDlgItemText(IDC_Rece, ""); //未绑定变量，所以通过控件的ID来清空内容
    SetDlgItemText(IDC_Send, ""); //未绑定变量，所以通过控件的ID来清空内容
    ShowWindow(SW_HIDE);
    if (login.DoModal() != IDOK) {
        exit(1);
    }
    ShowWindow(SW_SHOW);
    userName = login.userName;
    pwd = login.pwd;
    SetWindowText("客户端 - " + userName);
}

CString CMFCQQClientDlg::getDateTime(bool haveDate)
{
    if (haveDate) {
        return CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
    }
    else {
        return CTime::GetCurrentTime().Format("%H:%M:%S");
    }
}


void CMFCQQClientDlg::OnBnClickedLogout()
{
    KillTimer(1); //即将注销，所以此时关闭心跳包的发送，否则将会导致3秒内崩溃
    CString dataToSend = msg.join("", TYPE[Logout], userName, "", "", pwd);
    sendMsg(dataToSend, pSock);
    pSock->Close();
    delete pSock;
    pSock = NULL;
    showLoginDlg();
}


void CMFCQQClientDlg::OnBnClickedSendMessage()
{
    if (m_connected) {
        CString send;
        GetDlgItemText(IDC_Send, send);
        if (send == "") {
            MessageBox("请输入要发送的消息", "温馨提示");
            return;
        }
        CString toUser;
        m_cbMsgTo.GetLBText(m_cbMsgTo.GetCurSel(), toUser);
        CString dataToSend = msg.join(send, TYPE[ChatMsg], userName, "", toUser);
        if (sendMsg(dataToSend, pSock) != SOCKET_ERROR) {
            updateEvent("我", send);
            SetDlgItemText(IDC_Send, "");
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
        case 1:
            CString dataToSend = msg.join("", TYPE[I_am_online], userName);
            sendMsg(dataToSend, pSock);
            break;
    }
    CDialogEx::OnTimer(nIDEvent);
}


void CMFCQQClientDlg::OnSelChangeMsgTo()
{
    CString who; //选定userList中的哪个用户
    m_cbMsgTo.GetLBText(m_cbMsgTo.GetCurSel(), who);
    //组织消息并发送给服务器，咨询该用户是否是在线
    CString dataToSend = msg.join(who, TYPE[OnlineState], userName);
    sendMsg(dataToSend, pSock);
}


BOOL CMFCQQClientDlg::PreTranslateMessage(MSG* pMsg)
{
    int cont_ID = GetWindowLong(pMsg->hwnd, GWL_ID); //获取响应控件消息的ID
    if (cont_ID == IDC_Send) { //判断是否是要处理的控件的ID
        if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) { //若按下键盘且为enter键
            if (GetKeyState(VK_CONTROL) & 0x80) { //在已经按下enter的前提下又按下了ctrl键（这里0x80是对所有按键的检测）
                //添加换行
                CString str;
                GetDlgItemText(IDC_Send, str);
                SetDlgItemText(IDC_Send, str + "\r\n");
                //设置光标位置，将光标移至最后一个位置
                CEdit* pEdit = (CEdit*)GetDlgItem(IDC_Send);
                int len = str.GetLength() + 2;
                pEdit->SetSel(len - 1, len); //SetSel()用来选中，两参数分别代表选中的起始位置和最终位置
            }
            else {
                OnBnClickedSendMessage();
            }
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg); //默认处理方式
}

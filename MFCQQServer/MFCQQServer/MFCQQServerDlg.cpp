
// MFCQQServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCQQServer.h"
#include "MFCQQServerDlg.h"
#include "afxdialogex.h"
#include "tlhelp32.h"

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


// CMFCQQServerDlg 对话框
bool findProcessByName(const CString &name) {
    PROCESSENTRY32 pe32 = { sizeof(pe32) };
    HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    for (BOOL find = Process32First(hp, &pe32); find != 0; find = Process32Next(hp, &pe32)) {
        if (name == pe32.szExeFile)
            return true;
    }
    return false;//整个结束了还没有返回，即代表未找到该进程，所以返回false
}


CMFCQQServerDlg::CMFCQQServerDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_MFCQQSERVER_DIALOG, pParent)
    , sendData(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    listenSocket = NULL;
    m_port = 22783;
    if (!findProcessByName("mysqld.exe")) {
        ShellExecute(0, "open", "mysqld", 0, 0, SW_HIDE);
    }
    conn = mysql_init(0);
    if (mysql_real_connect(conn, "localhost", "root", "123456", "test", 0, 0, 0) != 0) {
        mysql_query(conn, "set names 'GBK'");
        if (mysql_query(conn, "select userName,passwd from UserInfo") == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            MYSQL_ROW row;
            userList = "";
            while ((row = mysql_fetch_row(res)) != NULL) {
                userInfoMap[row[0]] = row[1];
                userList += row[0] + (CString)";";
            }
            mysql_free_result(res);
        }
        else
            MessageBox(mysql_error(conn), "Error on MySQL:", MB_ICONERROR);
    }
    else {
        MessageBox(mysql_error(conn), "Error on MySQL:", MB_ICONERROR);
        exit(1);
    }
}

void CMFCQQServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_SendData, sendData);
}

void CMFCQQServerDlg::addClient()
{
    ServerSocket* pSocket = new ServerSocket(this);
    listenSocket->Accept(*pSocket);
    pSocket->AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE);
}

void CMFCQQServerDlg::receData(ServerSocket* sock)
{
    char buffer[10000];
    if (sock->Receive(buffer, sizeof(buffer)) != SOCKET_ERROR) {
        msg.load(buffer);
        if (msg.type == TYPE[Login]) {
            if (isUserInfoValid(msg.userId, msg.pw)) {
                if (userSockMap.find(msg.userId) == userSockMap.end()) { //说明此时还未保存该用户的socket描述符，可以正常登录
                    CString dataToSend = msg.join(userList, TYPE[UserList], msg.userId);
                    sendMsg(dataToSend, sock);
                    userSockMap[msg.userId] = sock;
                    updateEvent(msg.userId + "已上线", "");
                }else{
                    CString dataToSend = msg.join(userList, TYPE[UserList], msg.userId);
                    sendMsg(dataToSend, sock);
                    dataToSend = msg.join("", TYPE[Sequze], msg.userId);
                    sendMsg(dataToSend, userSockMap[msg.userId]);
                    userSockMap[msg.userId]->Close();
                    delete userSockMap[msg.userId];
                    userSockMap[msg.userId] = sock;
                    updateEvent(msg.userId + "已在另一处登录", "");
                }
            }
            else {
                CString dataToSend = msg.join("", TYPE[LoginFail], msg.userId);
                sendMsg(dataToSend, sock);
            }
        }
        else if (msg.type == TYPE[Logout]) {
            auto it = userSockMap.find(msg.userId);
            if (it != userSockMap.end()) {
                userSockMap[msg.userId]->Close();
                delete userSockMap[msg.userId];
                userSockMap.erase(it);
                updateEvent(msg.userId + "已下线", "");
            }
        }
        else if (msg.type == TYPE[ChatMsg]) {
            if (msg.toUser == "服务器") {
                updateEvent(msg.userId, msg.data);
            }
            else {
                auto it = userSockMap.find(msg.toUser);
                if (it != userSockMap.end()) {
                    CString dataToSend = msg.join(msg.data, TYPE[ChatMsg], msg.toUser, msg.userId); //发送的内容 消息类型 消息发给谁 发送者是谁
                    sendMsg(dataToSend, userSockMap[msg.toUser]);
                    updateEvent(msg.userId + "给" + msg.toUser, msg.data);
                }
                else {
                    CString dataToSend = msg.join(msg.toUser + "不在线,请稍后重试", TYPE[ChatMsg], msg.userId, "服务器");
                    sendMsg(dataToSend, userSockMap[msg.userId]);
                    updateEvent(msg.userId + "给" + msg.toUser, msg.data + "（发送失败，该用户暂不在线！）");
                }
            }
        }
        else if (msg.type == TYPE[Register]) {
            if (userInfoMap.find(msg.userId) == userInfoMap.end()) {
                CString str = "insert into userinfo(userName,passwd) values('" + msg.userId + "','" + msg.pw + "')";
                mysql_query(conn,str);
                userInfoMap[msg.userId] = msg.pw; //在map中新增一项，将该用户名和密码对存入map中
                userList += msg.userId + ";"; //在用户列表中新增一条用户名
                for (auto &elem:userSockMap) {
                    CString dataToSend = msg.join(msg.userId, TYPE[AddUserList], elem.first); //格式：消息内容 消息类型 谁能收到（这里针对服务器端来说，就是谁能收到，如果是针对客户端，即谁在发送）从哪里来 去哪里 密码是
                    sendMsg(dataToSend, elem.second);
                }
            }
        }
        else {
            updateEvent("未知消息", buffer);
        }
    }
}

bool CMFCQQServerDlg::isUserInfoValid(const CString & user, const CString & pwd)
{
    //std::map<CString, CString>::iterator i = userInfoMap.find(user);
    auto i = userInfoMap.find(user);
    if (i != userInfoMap.end())
        return userInfoMap[user] == pwd;
    return false;
}

void CMFCQQServerDlg::sendMsg(const CString & data, ServerSocket * sock)
{
    if (sock->Send(data, data.GetLength() + 1) != SOCKET_ERROR) { //即发送成功
        sendData = "";
        UpdateData(false);
    }
}

void CMFCQQServerDlg::updateEvent(const CString & title, const CString & content)
{
    CString str;
    if (content == "") {
        str = title + "\r\n";
    }
    else {
        str = title + ": " + content + "\r\n";
    }
    CEdit* pEvent = (CEdit*)GetDlgItem(IDC_RECE_DATA); //获取到界面中一个控件，控件ID由我们自己写
    int lastLine = pEvent->LineIndex(pEvent->GetLineCount() - 1); //获取编辑框最后一行索引，该函数的参数必须是有效的索引值（下标），如有5行则有效的下标是0~4
    pEvent->SetSel(lastLine + 1, lastLine + 2, 0); //选择编辑框最后一行
    pEvent->ReplaceSel(str); //替换所选那一行的内容，本来下一行是没有内容的
}

BEGIN_MESSAGE_MAP(CMFCQQServerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_OpenServer, &CMFCQQServerDlg::OnBnClickedOpenserver)
    ON_BN_CLICKED(IDC_SendMsg, &CMFCQQServerDlg::OnBnClickedSendMsg)
END_MESSAGE_MAP()


// CMFCQQServerDlg 消息处理程序

BOOL CMFCQQServerDlg::OnInitDialog()
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
    OnBnClickedOpenserver();
    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCQQServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCQQServerDlg::OnPaint()
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
HCURSOR CMFCQQServerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CMFCQQServerDlg::OnBnClickedOpenserver()
{
    // TODO: 在此添加控件通知处理程序代码
    if (listenSocket != NULL) {
        listenSocket->Close();
        delete listenSocket;
        listenSocket = NULL;
    }
    static bool firstOpen = 1;
    listenSocket = new ServerSocket(this);
    if (!listenSocket->Create(m_port, SOCK_STREAM)) {
        if (!firstOpen) {
            MessageBox("创建套接字失败", "温馨提示");
        }
        firstOpen = 0;
        return;
    }
    if (listenSocket->Listen(UserNumMax)) {
        GetDlgItem(ID_OpenServer)->EnableWindow(0);
        if (!firstOpen)
            MessageBox("开启成功", "温馨提示");
        else
            firstOpen = 0;
    }
    else
        if (!firstOpen)
            MessageBox("开启失败，请重试", "温馨提示");
        else
            firstOpen = 0;
}


void CMFCQQServerDlg::OnBnClickedSendMsg()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(true);
    if (sendData == "") {
        MessageBox("请先输入消息", "温馨提示");
        return;
    }
    for (auto &elem : userSockMap) {
        sendMsg(sendData, elem.second);
    }
    //lastClientSocket->Send(sendData, sendData.GetLength()+1);

}

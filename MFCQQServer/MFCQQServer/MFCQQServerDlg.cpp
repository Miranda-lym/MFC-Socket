
// MFCQQServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCQQServer.h"
#include "MFCQQServerDlg.h"
#include "afxdialogex.h"

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
	enum { IDD = IDD_ABOUTBOX };
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



CMFCQQServerDlg::CMFCQQServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCQQSERVER_DIALOG, pParent)
    , sendData(_T(""))
    , m_receiveData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    listenSocket = NULL;
    lastClientSocket = NULL;
    m_port = 22783;
    conn = mysql_init(0);
    if (mysql_real_connect(conn, "localhost", "root", "123456", "test", 0, 0, 0) != 0) {
        mysql_query(conn, "set name 'GBK'");
        if (mysql_query(conn, "select userName,passwd from UserInfo") == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            MYSQL_ROW row;
            userList = "";
            while ((row = mysql_fetch_row(res)) != NULL) {
                userInfoMap[row[0]] = row[1];
                userList += row[0] + (CString)";";
            }
            mysql_free_result(res);
        }else
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
    DDX_Text(pDX, IDC_RECE_DATA, m_receiveData);
}

void CMFCQQServerDlg::addClient()
{
    ServerSocket* pSocket = new ServerSocket(this);
    listenSocket->Accept(*pSocket);
    pSocket->AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE);
    pSocket->Send(userList, 70);
    lastClientSocket = pSocket;
}

void CMFCQQServerDlg::receData()
{
    char buffer[10000];
    if (lastClientSocket->Receive(buffer, sizeof(buffer)) != SOCKET_ERROR) {
        msg.load(buffer);
        if (msg.type == TYPE[Login]) {
            if(isUserInfoValid(msg.userId,msg.pw)){}

        }

        //m_receiveData += CString("\r\n") + buffer;
        UpdateData(false);
    }
}

bool CMFCQQServerDlg::isUserInfoValid(const CString & user, const CString & pwd)
{
    auto i = userInfoMap.find(user);
    if (i != userInfoMap.end())
        return userInfoMap[user] == pwd;
    return false;
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
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
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
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCQQServerDlg::OnPaint()
{
	if (IsIconic())
	{
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
	else
	{
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
        if(!firstOpen){
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
    }else
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
    lastClientSocket->Send(sendData, sendData.GetLength()+1);
    
}

// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCQQClient.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "ClientSocket.h"
#include "MFCQQClientDlg.h"
#include "RegisterDlg.h"


// LoginDlg 对话框

IMPLEMENT_DYNAMIC(LoginDlg, CDialogEx)



LoginDlg::LoginDlg(CMFCQQClientDlg * _pMainDlg)
    :pMainDlg(_pMainDlg), CDialogEx(IDD_DIALOG1)
    , userName(_T(""))
    , pwd(_T(""))
{
    timeOut = 0;//默认是没有超时的
}

LoginDlg::~LoginDlg()
{
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_USER_NAME, userName);
    DDX_Text(pDX, IDC_PWD, pwd);
}


BEGIN_MESSAGE_MAP(LoginDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &LoginDlg::OnBnClickedOk)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_Register, &LoginDlg::OnBnClickedRegister)
END_MESSAGE_MAP()


// LoginDlg 消息处理程序


void LoginDlg::OnBnClickedOk()
{
    UpdateData(true);
    if (userName == "") {
        MessageBox("请输入用户名", "温馨提示");
        return;
    }
    if (pwd == "") {
        MessageBox("请输入密码", "温馨提示");
        return;
    }
    AfxGetApp()->WriteProfileString("Login", "userName", userName);
    AfxGetApp()->WriteProfileString("Login", "pwd", pwd);

    pMainDlg->pSock = new ClientSocket(pMainDlg);
    if (!pMainDlg->pSock->Create()) {
        MessageBox("创建套接字失败！", "温馨提示", MB_ICONERROR);
        return;
    }
    if (!pMainDlg->pSock->Connect("127.0.0.1", 22783)) {
        MessageBox("连接服务器失败！" + ClientSocket::getLastErrorStr(), "提示", MB_ICONERROR);
        return;
    }

    CString dataToSend = pMainDlg->msg.join("", TYPE[Login], userName, "", "", pwd);
    if(pMainDlg->sendMsg(dataToSend,pMainDlg->pSock)==SOCKET_ERROR) {
        pMainDlg->pSock->Close();
        delete pMainDlg->pSock;
        pMainDlg->pSock = NULL;
        return;
    }
    SetTimer(0, 1000, NULL);
    pMainDlg->m_connected = loginFail = timeOut = false;
    while (!timeOut) {
        MSG m; //避免出现界面假死
        if (PeekMessage(&m, (HWND)NULL, 0, 0, PM_REMOVE))
            ::SendMessage(m.hwnd, m.message, m.wParam, m.lParam);

        Sleep(50);
        if (pMainDlg->m_connected) {
            CDialogEx::OnOK();
            break;
        }
        if (loginFail) {
            MessageBox("用户名或密码错误", "登录失败", MB_ICONERROR);
            break;
        }
    }
    if (!pMainDlg->m_connected && !loginFail && timeOut) {
        MessageBox("登录超时", "温馨提示");
    }
}


void LoginDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent) {
        case 0:
            KillTimer(0);
            timeOut = 1;
            break;
    }
    CDialogEx::OnTimer(nIDEvent);
}


BOOL LoginDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    userName = AfxGetApp()->GetProfileString("Login", "userName");
    pwd = AfxGetApp()->GetProfileString("Login", "pwd");
    UpdateData(false);
    GetDlgItem(IDC_USER_NAME)->SetFocus();
    ((CEdit*)GetDlgItem(IDC_USER_NAME))->SetSel(0, -1); //->的优先级比强转类型的优先级高，所以要加括号，这里0指起始位置，-1指中止位置（因为不知道用户名会有多长）
    return FALSE; 
}


void LoginDlg::OnBnClickedRegister()
{
    ShowWindow(SW_HIDE);
    RegisterDlg regDlg(pMainDlg);
    if (regDlg.DoModal() == IDOK) {
        userName = regDlg.userName;
        pwd = regDlg.pwd;
        UpdateData(FALSE);
        OnBnClickedOk();
    }
    ShowWindow(SW_SHOW);
}

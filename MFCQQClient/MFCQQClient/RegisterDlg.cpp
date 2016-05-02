// RegisterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCQQClient.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"
#include "ClientSocket.h"
#include "MyMsg.h"


// RegisterDlg 对话框

IMPLEMENT_DYNAMIC(RegisterDlg, CDialogEx)

RegisterDlg::RegisterDlg(CMFCQQClientDlg * _pMainDlg)
    : pMainDlg(_pMainDlg)
    , CMyDialog(IDD_Register)
    , pwd(_T(""))
    , pwd2(_T(""))
    , userName(_T(""))
{

}

RegisterDlg::~RegisterDlg()
{
    delete sock;
}

void RegisterDlg::receData()
{
    char buffer[3];
    if (sock->Receive(buffer, sizeof(buffer)) != SOCKET_ERROR) {
        sock->Close();
        if (buffer[0] == '0') {
            MessageBox(" 注册失败,该用户名已存在，请更换用户名再重试！", "温馨提示",MB_ICONERROR);
        }
        else if (buffer[0] == '1') {
            MessageBox(" 注册成功，即将进入登录界面", "恭喜你");
            CDialogEx::OnOK();
        }
    }
}

void RegisterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_PWD, pwd);
    DDX_Text(pDX, IDC_PWD2, pwd2);
    DDX_Text(pDX, IDC_UserName, userName);
}


BEGIN_MESSAGE_MAP(RegisterDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &RegisterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// RegisterDlg 消息处理程序


void RegisterDlg::OnBnClickedOk()
{
    UpdateData(TRUE);
    if (userName == "") {
        MessageBox("请输入用户名");
        GetDlgItem(IDC_UserName)->SetFocus();
        return;
    }
    if (pwd == "") {
        MessageBox("请输入密码");
        GetDlgItem(IDC_PWD)->SetFocus();
        return;
    }
    if (pwd2 == "") {
        MessageBox("请确认密码");
        GetDlgItem(IDC_PWD2)->SetFocus();
        return;
    }
    if (pwd != pwd2) {
        MessageBox("两次密码不一致，请重新输入","温馨提示",MB_ICONERROR);
        pwd2 = "";
        UpdateData(FALSE);
        GetDlgItem(IDC_PWD2)->SetFocus();
        return;
    }

    if (!sock->Create()) {
        MessageBox("创建套接字失败！", "温馨提示", MB_ICONERROR);
        return;
    }
    if (!sock->Connect(LoginDlg::serverIP, LoginDlg::serverPort)) {
        CString str;
        str.Format("错误代码：%d", GetLastError());
        MessageBox("连接服务器失败！" + str, "提示", MB_ICONERROR);
        return;
    }
    MyMsg msg;
    CString dataToSend = msg.join("", TYPE[Register], userName, "", "", pwd);
    pMainDlg->sendMsg(dataToSend, sock);
}


BOOL RegisterDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    GetDlgItem(IDC_UserName)->SetFocus();
    sock = new ClientSocket(this);

    return FALSE;  
}

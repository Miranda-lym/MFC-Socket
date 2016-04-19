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
    , CDialogEx(IDD_Register)
    , pwd(_T(""))
    , pwd2(_T(""))
    , userName(_T(""))
{

}

RegisterDlg::~RegisterDlg()
{
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
        return;
    }
    if (pwd == "") {
        MessageBox("请输入密码");
        return;
    }
    if (pwd2 == "") {
        MessageBox("请确认密码");
        return;
    }
    if (pwd != pwd2) {
        MessageBox("两次密码不一致，请重新输入","温馨提示",MB_ICONERROR);
        return;
    }
    ClientSocket sock(0);

    if (!sock.Create()) {
        MessageBox("创建套接字失败！", "温馨提示", MB_ICONERROR);
        return;
    }
    if (!sock.Connect("127.0.0.1", 22783)) {
        CString str;
        str.Format("错误代码：%d", GetLastError());
        MessageBox("连接服务器失败！" + str, "提示", MB_ICONERROR);
        return;
    }
    MyMsg msg;
    CString dataToSend = msg.join("", TYPE[Register], userName, "", "", pwd);
    pMainDlg->sendMsg(dataToSend, &sock);
    sock.Send(dataToSend, dataToSend.GetLength() + 1);
    sock.Close();
    MessageBox("注册信息已发送，审核通过后即可进行登录！", "温馨提示");
    CDialogEx::OnOK();
}

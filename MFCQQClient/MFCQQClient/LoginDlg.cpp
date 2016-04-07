// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCQQClient.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


// LoginDlg 对话框

IMPLEMENT_DYNAMIC(LoginDlg, CDialogEx)

LoginDlg::LoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
    , userName(_T(""))
    , pwd(_T(""))
{

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
END_MESSAGE_MAP()


// LoginDlg 消息处理程序


void LoginDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(true);
    if (userName == "") {
        MessageBox("请输入用户名", "温馨提示");
        return;
    }
    if(pwd=="") {
        MessageBox("请输入密码", "温馨提示");
        return;
    }


    CDialogEx::OnOK();
}

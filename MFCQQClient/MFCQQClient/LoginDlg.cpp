// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCQQClient.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "ClientSocket.h"
#include "MFCQQClientDlg.h"
#include "RegisterDlg.h"


// LoginDlg �Ի���

IMPLEMENT_DYNAMIC(LoginDlg, CDialogEx)



LoginDlg::LoginDlg(CMFCQQClientDlg * _pMainDlg)
    :pMainDlg(_pMainDlg), CDialogEx(IDD_DIALOG1)
    , userName(_T(""))
    , pwd(_T(""))
{
    timeOut = 0;//Ĭ����û�г�ʱ��
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


// LoginDlg ��Ϣ�������


void LoginDlg::OnBnClickedOk()
{
    UpdateData(true);
    if (userName == "") {
        MessageBox("�������û���", "��ܰ��ʾ");
        return;
    }
    if (pwd == "") {
        MessageBox("����������", "��ܰ��ʾ");
        return;
    }
    AfxGetApp()->WriteProfileString("Login", "userName", userName);
    AfxGetApp()->WriteProfileString("Login", "pwd", pwd);

    pMainDlg->pSock = new ClientSocket(pMainDlg);
    if (!pMainDlg->pSock->Create()) {
        MessageBox("�����׽���ʧ�ܣ�", "��ܰ��ʾ", MB_ICONERROR);
        return;
    }
    if (!pMainDlg->pSock->Connect("127.0.0.1", 22783)) {
        CString str;
        str.Format("������룺%d", GetLastError());
        MessageBox("���ӷ�����ʧ�ܣ�" + str, "��ʾ", MB_ICONERROR);
        return;
    }

    CString dataToSend = pMainDlg->msg.join("", TYPE[Login], userName, "", "", pwd);
    pMainDlg->pSock->Send(dataToSend, dataToSend.GetLength() + 1);
    SetTimer(0, 1000, NULL);
    pMainDlg->m_connected = loginFail = timeOut = false;
    while (!timeOut) {
        MSG m; //������ֽ������
        if (PeekMessage(&m, (HWND)NULL, 0, 0, PM_REMOVE))
            ::SendMessage(m.hwnd, m.message, m.wParam, m.lParam);

        Sleep(50);
        if (pMainDlg->m_connected) {
            CDialogEx::OnOK();
            break;
        }
        if (loginFail) {
            MessageBox("�û������������", "��¼ʧ��", MB_ICONERROR);
            break;
        }
    }
    if (!pMainDlg->m_connected && !loginFail && timeOut) {
        MessageBox("��¼��ʱ", "��ܰ��ʾ");
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

    return FALSE; 
}


void LoginDlg::OnBnClickedRegister()
{
    ShowWindow(SW_HIDE);
    RegisterDlg regDlg;
    regDlg.DoModal();
    ShowWindow(SW_SHOW);
}

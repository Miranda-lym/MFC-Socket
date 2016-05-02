// RegisterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCQQClient.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"
#include "ClientSocket.h"
#include "MyMsg.h"


// RegisterDlg �Ի���

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
            MessageBox(" ע��ʧ��,���û����Ѵ��ڣ�������û��������ԣ�", "��ܰ��ʾ",MB_ICONERROR);
        }
        else if (buffer[0] == '1') {
            MessageBox(" ע��ɹ������������¼����", "��ϲ��");
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


// RegisterDlg ��Ϣ�������


void RegisterDlg::OnBnClickedOk()
{
    UpdateData(TRUE);
    if (userName == "") {
        MessageBox("�������û���");
        GetDlgItem(IDC_UserName)->SetFocus();
        return;
    }
    if (pwd == "") {
        MessageBox("����������");
        GetDlgItem(IDC_PWD)->SetFocus();
        return;
    }
    if (pwd2 == "") {
        MessageBox("��ȷ������");
        GetDlgItem(IDC_PWD2)->SetFocus();
        return;
    }
    if (pwd != pwd2) {
        MessageBox("�������벻һ�£�����������","��ܰ��ʾ",MB_ICONERROR);
        pwd2 = "";
        UpdateData(FALSE);
        GetDlgItem(IDC_PWD2)->SetFocus();
        return;
    }

    if (!sock->Create()) {
        MessageBox("�����׽���ʧ�ܣ�", "��ܰ��ʾ", MB_ICONERROR);
        return;
    }
    if (!sock->Connect(LoginDlg::serverIP, LoginDlg::serverPort)) {
        CString str;
        str.Format("������룺%d", GetLastError());
        MessageBox("���ӷ�����ʧ�ܣ�" + str, "��ʾ", MB_ICONERROR);
        return;
    }
    MyMsg msg;
    CString dataToSend = msg.join("", TYPE[Register], userName, "", "", pwd);
    pMainDlg->sendMsg(dataToSend, sock);
}


BOOL RegisterDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    GetDlgItem(IDC_UserName)->SetFocus();
    sock = new ClientSocket(this);

    return FALSE;  
}

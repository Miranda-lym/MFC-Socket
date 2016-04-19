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


// RegisterDlg ��Ϣ�������


void RegisterDlg::OnBnClickedOk()
{
    UpdateData(TRUE);
    if (userName == "") {
        MessageBox("�������û���");
        return;
    }
    if (pwd == "") {
        MessageBox("����������");
        return;
    }
    if (pwd2 == "") {
        MessageBox("��ȷ������");
        return;
    }
    if (pwd != pwd2) {
        MessageBox("�������벻һ�£�����������","��ܰ��ʾ",MB_ICONERROR);
        return;
    }
    ClientSocket sock(0);

    if (!sock.Create()) {
        MessageBox("�����׽���ʧ�ܣ�", "��ܰ��ʾ", MB_ICONERROR);
        return;
    }
    if (!sock.Connect("127.0.0.1", 22783)) {
        CString str;
        str.Format("������룺%d", GetLastError());
        MessageBox("���ӷ�����ʧ�ܣ�" + str, "��ʾ", MB_ICONERROR);
        return;
    }
    MyMsg msg;
    CString dataToSend = msg.join("", TYPE[Register], userName, "", "", pwd);
    pMainDlg->sendMsg(dataToSend, &sock);
    sock.Send(dataToSend, dataToSend.GetLength() + 1);
    sock.Close();
    MessageBox("ע����Ϣ�ѷ��ͣ����ͨ���󼴿ɽ��е�¼��", "��ܰ��ʾ");
    CDialogEx::OnOK();
}

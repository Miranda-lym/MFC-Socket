#include "stdafx.h"
#include "ServerSocket.h"
#include "MFCQQServerDlg.h"


ServerSocket::ServerSocket(CMFCQQServerDlg * _pDlg)
{
    pDlg = _pDlg;
}

void ServerSocket::OnAccept(int nErrorCode)
{
    // TODO: �ڴ����ר�ô����/����û���
    pDlg->addClient();
    CSocket::OnAccept(nErrorCode);
}


void ServerSocket::OnReceive(int nErrorCode)
{
    // TODO: �ڴ����ר�ô����/����û���
    pDlg->receData();
    CSocket::OnReceive(nErrorCode);
}

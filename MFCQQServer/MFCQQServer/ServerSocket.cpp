#include "stdafx.h"
#include "ServerSocket.h"
#include "MFCQQServerDlg.h"


ServerSocket::ServerSocket(CMFCQQServerDlg * _pDlg)
{
    pDlg = _pDlg;
}

void ServerSocket::OnAccept(int nErrorCode)
{
    // TODO: 在此添加专用代码和/或调用基类
    pDlg->addClient();
    CSocket::OnAccept(nErrorCode);
}


void ServerSocket::OnReceive(int nErrorCode)
{
    // TODO: 在此添加专用代码和/或调用基类
    pDlg->receData();
    CSocket::OnReceive(nErrorCode);
}

#include "stdafx.h"
#include "ClientSocket.h"
#include "MFCQQClientDlg.h"

ClientSocket::ClientSocket(CMFCQQClientDlg* _pDlg)
{
    pDlg = _pDlg;
}


ClientSocket::~ClientSocket()
{
}


void ClientSocket::OnReceive(int nErrorCode)
{
    // TODO: 在此添加专用代码和/或调用基类
    pDlg->receData();
    CSocket::OnReceive(nErrorCode);
}

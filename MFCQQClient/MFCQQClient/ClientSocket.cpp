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
    // TODO: �ڴ����ר�ô����/����û���
    pDlg->receData();
    CSocket::OnReceive(nErrorCode);
}

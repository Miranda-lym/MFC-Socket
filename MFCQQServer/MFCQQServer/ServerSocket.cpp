#include "stdafx.h"
#include "ServerSocket.h"
#include "MFCQQServerDlg.h"


ServerSocket::ServerSocket(CMFCQQServerDlg * _pDlg)
{
    pDlg = _pDlg;
}

void ServerSocket::OnAccept(int nErrorCode)
{
    pDlg->addClient();
    CSocket::OnAccept(nErrorCode);
}

CString ServerSocket::getLastErrorStr()
{
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
                  | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
                  , 0, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
                  , (LPTSTR)&lpMsgBuf, 0, 0);
    CString errStr = (LPCTSTR)lpMsgBuf;
    LocalFree(lpMsgBuf);
    return errStr;
}

void ServerSocket::OnReceive(int nErrorCode)
{
    pDlg->receData(this);
    CSocket::OnReceive(nErrorCode);
}

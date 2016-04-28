#include "stdafx.h"
#include "ClientSocket.h"
#include "MFCQQClientDlg.h"

ClientSocket::ClientSocket(CMyDialog* _pDlg)
{
    pDlg = _pDlg;
}

CString ClientSocket::getLastErrorStr()
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

//当接受缓冲区有消息时，自动响应
void ClientSocket::OnReceive(int nErrorCode)
{
    pDlg->receData();
    CSocket::OnReceive(nErrorCode);
}

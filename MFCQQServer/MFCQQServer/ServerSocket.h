#pragma once
#include "afxsock.h"
class CMFCQQServerDlg;
class ServerSocket :
    public CSocket
{
    CMFCQQServerDlg* pDlg;
public:
    ServerSocket(CMFCQQServerDlg* _pDlg);

    virtual void OnAccept(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
};


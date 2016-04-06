#pragma once
#include "afxsock.h"

class CMFCQQClientDlg;
class ClientSocket :
    public CSocket
{
    CMFCQQClientDlg* pDlg;
public:
    ClientSocket(CMFCQQClientDlg* _pDlg);
    ~ClientSocket();
    virtual void OnReceive(int nErrorCode);
};


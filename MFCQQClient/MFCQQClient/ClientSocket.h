#pragma once
#include "afxsock.h"

class CMFCQQClientDlg;
class ClientSocket :
    public CSocket
{
    CMFCQQClientDlg* pDlg;
public:
    ClientSocket(CMFCQQClientDlg* _pDlg);
    static CString getLastErrorStr();
    virtual void OnReceive(int nErrorCode);
};


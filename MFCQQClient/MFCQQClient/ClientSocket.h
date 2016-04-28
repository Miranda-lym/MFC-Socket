#pragma once
#include "afxsock.h"

class CMyDialog;
class ClientSocket :
    public CSocket
{
    CMyDialog* pDlg;
public:
    ClientSocket(CMyDialog* _pDlg);
    static CString getLastErrorStr();
    virtual void OnReceive(int nErrorCode);
};


#pragma once
#ifndef _MYMSG_H_
#define _MYMSG_H_

#include "afxstr.h"

extern const CString TYPE[31];
extern const char seperator[3];

class MyMsg
{
public:
    CString userId; //�û���
    CString pw; //����
    CString fromUser; //��Ϣ����
    CString toUser; //��Ϣȥ��
    CString type; //��Ϣ����
    CString data; //��Ϣ����

    explicit MyMsg(CString str = "");
    CString load(CString str);
    CString join(CString _data = "", CString _type = "", CString _user = "", CString _from = "", CString  _to = "", CString _pw = "")const;
    static CString rightN(const CString & str, int n) {
        return str.Right(str.GetLength() - n);
    }
};

#endif // !_MYMSG_H_

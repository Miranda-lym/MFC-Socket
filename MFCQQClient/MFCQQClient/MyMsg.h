#pragma once
#ifndef _MYMSG_H_
#define _MYMSG_H_

#include "afxstr.h"

extern const CString TYPE[30];
extern const char seperator[3];

class MyMsg
{
public:
    CString userId; //用户名
    CString pw; //密码
    CString type; //消息类型
    CString fromUser; //消息来自
    CString toUser; //消息去向
    CString data; //消息内容

    explicit MyMsg(CString str = "");
    CString load(CString str);
    CString join(CString _data = "", CString _type = "", CString _user = "", CString _from = "", CString  _to = "", CString _pw = "")const;
    static CString rightN(const CString & str, int n) {
        return str.Right(str.GetLength() - n);
    }
};

#endif // !_MYMSG_H_

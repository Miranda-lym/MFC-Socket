#include "stdafx.h"
#include "MyMsg.h"

//消息类型
const CString TYPE[31] = { TYPE_ChatMsg , TYPE_Server_is_closed , TYPE_UserList , TYPE_OnlineState , TYPE_FileSend , TYPE_FileData , TYPE_AskFileData , TYPE_File_NO , TYPE_File_Over , TYPE_File_Fail , TYPE_LoginFail , TYPE_UserIsOnline , TYPE_OfflineMsg , TYPE_AllUser , TYPE_AddUserList , TYPE_I_am_online , TYPE_Logout , TYPE_Login , TYPE_Register , TYPE_Status , TYPE_Sequze };
const char seperator[3] = { 0x1F,0x7C };

MyMsg::MyMsg(CString str)
{
    if (str != "")
        load(str);
}

CString MyMsg::load(CString str)
{
    CString tempStr[6] = { "" };
    int index = 0, i;
    for (index = 0; index < 5; ++index) {
        i = str.Find(seperator);
        tempStr[index] = str.Left(i);
        str = rightN(str, i + 2);
        if (str == "") //为了鲁棒性
            break;
    }
    tempStr[5] = str;
    userId = tempStr[0];
    pw = tempStr[1];
    fromUser = tempStr[2];
    toUser = tempStr[3];
    type = tempStr[4];
    data = tempStr[5];
    return str;
}

CString MyMsg::join(CString _data, CString _type, CString _user, CString _from, CString _to, CString _pw) const
{
    if (_user == "")
        _user = userId;
    return _user + seperator + _pw + seperator + _from + seperator + _to + seperator + _type + seperator + _data;
}



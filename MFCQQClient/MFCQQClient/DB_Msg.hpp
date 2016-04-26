#pragma once
#ifndef _DB_MSG_HPP_
#define _DB_MSG_HPP_

#include <windows.h>
#include "mysql.h"
#include <string>
#include <vector>
#include <fstream>

using std::string;
using std::vector;

class DB_Connector {
protected:
    MYSQL* mysql; //mysql���ӵľ��
public:
    static string host_, user_, passwd_, db_;
public:
    inline DB_Connector();
    inline virtual ~DB_Connector();
    inline int query(const string& sql); //ִ��sql��䣬�ɹ��ͷ���0��ʧ���򷵻ط�0
    inline string error();
    inline vector<vector<string>> getResult();
};

inline DB_Connector::DB_Connector()
{
    mysql = mysql_init(NULL);
    if (mysql_real_connect(mysql, host_.c_str(), user_.c_str(), passwd_.c_str(), db_.c_str(), 0, 0, 0) == 0) { //����ʧ��
        string err = "Failed to connect database server: " + error();
        throw std::logic_error(err);
    }
    query("set names 'GBK'"); //�����ַ�������ֹ���ĳ�������
}

inline DB_Connector::~DB_Connector()
{
    mysql_close(mysql); //�ر�mysql����
}

inline int DB_Connector::query(const string & sql)
{
    return mysql_query(mysql, sql.c_str()); //ִ��sql��䣬�ɹ��򷵻�0��ʧ���򷵻ط�0
}

inline string DB_Connector::error()
{
    return mysql_error(mysql); //������һ�εĴ�����ʾ
}

inline vector<vector<string>> DB_Connector::getResult()
{
    vector<vector<string>>result;
    MYSQL_RES* res = mysql_store_result(mysql); //�����ڴ���query()���Һ�����
    unsigned num = mysql_num_fields(res); //�����������
    for (MYSQL_ROW row; (row = mysql_fetch_row(res)) != NULL;) {
        result.push_back(vector<string>(num)); //Ԥ��һ�пյģ�������һ���н������и�ֵ
        for (size_t cur = result.size() - 1, i = 0; i < num; ++i) { //result.size()ָ��ǰ����
            result[cur][i] = row[i];
        }
    }
    mysql_free_result(res);
    return result;
}

///��Ϣ������
class DB_Msg {
protected:
    DB_Connector db_conn; //���ݿ�����������
    const string tbName; //Ҫ�����ı���
    std::ofstream fsLog; //��־��¼�ļ���
public:
    inline DB_Msg(const string& _tableName, const string& _logFileName);
    inline virtual ~DB_Msg();
    //�����Ϣ
    inline void push(const string& from, const string& to, const string& content);
    //��ȡִ��ָ����sql���Ľ��
    inline vector<vector<string>> getResBySql(const string& sql);
    //����ָ����sql���ɾ����Ӧ�ļ�¼
    inline void delResBySql(const string& sql);
    // ��ȡ��ǰ�����ں�ʱ�䣬����������־
    inline static string getTime(); //�������ڶ�������ڣ�һ��ֻҪ�ú���δ�õ����е����ݳ�Աʱ���Ϳ�������Ϊ��̬��Ա����
protected:
    //�����洢�����¼�ı�
    inline void createTable();
};

inline DB_Msg::DB_Msg(const string& _tableName, const string& _logFileName)
{

}
inline DB_Msg::~DB_Msg()
{

}

inline void DB_Msg::push(const string & from, const string & to, const string & content)
{
}

inline vector<vector<string>> DB_Msg::getResBySql(const string & sql)
{
    return vector<vector<string>>();
}

inline void DB_Msg::delResBySql(const string & sql)
{
}

inline string DB_Msg::getTime()
{
    return string();
}

inline void DB_Msg::createTable()
{
}

#endif

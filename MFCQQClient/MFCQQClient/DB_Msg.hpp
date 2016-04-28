#pragma once
#ifndef _DB_MSG_HPP_
#define _DB_MSG_HPP_

#include <windows.h>
#include "mysql.h"
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

using std::string;
using std::vector;
using std::endl;

//���ݿ���������
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

//���캯�����Զ�����������ݿ⹦��
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

//��ѯ���ݿ�
inline int DB_Connector::query(const string & sql)
{
    return mysql_query(mysql, sql.c_str()); //ִ��sql��䣬�ɹ��򷵻�0��ʧ���򷵻ط�0
}

//�������ݿ�����д�����ʾ�������ӡ���ѯ��ɾ�����޸ĵȴ���
inline string DB_Connector::error()
{
    return mysql_error(mysql); //������һ�εĴ�����ʾ
}

//�õ���ѯ��Ľ�������Ǹ���ά���飩
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
    mysql_free_result(res); //�ͷŽ������store()�����free_result()
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
    :tbName(_tableName)
{
    if (_logFileName != "") {
        fsLog.open(_logFileName, std::ios::app); //append,ָ����ԭlog�ļ���׷��
    } //����־��Ϊ���ַ�����������Ҫ��־�ļ�����ô�Ͳ������open()����ôfsLog����������ʧЧ�ģ��൱��false��
    createTable();
}
inline DB_Msg::~DB_Msg()
{
    if (fsLog) { //�ļ�������
        fsLog.close();
    }
}

inline void DB_Msg::push(const string & from, const string & to, const string & content)
{
    string sql = "insert into " + tbName + "(fromUser,toUser,msg) values('" + from + "','"
        + to + "','" + content + "')";
    if (fsLog) {
        fsLog << getTime() << ">>>execute: " + sql << endl;
    }
    if (db_conn.query(sql) != 0 && fsLog) { //ִ��sql���ʧ�ܲ�����־�ļ���������д��־��ִ��sql���ʧ���򷵻ط�0��
        fsLog << "Failed to insert: " << db_conn.error() << endl;
    }
}

inline vector<vector<string>> DB_Msg::getResBySql(const string & sql)
{
    vector<vector<string>> result;
    if (fsLog) {
        fsLog << getTime() << ">>>execute: " + sql << endl;
    }
    if (db_conn.query(sql) == 0) { //ִ�гɹ���ѽ�����
        result = db_conn.getResult();
    }
    else if (fsLog) {
        fsLog << "Failed to search: " << db_conn.error() << endl;
    }
    return result;
}

inline void DB_Msg::delResBySql(const string & sql)
{
    if (fsLog) {
        fsLog << getTime() << ">>>execute: " << sql << endl;
    }
    if (db_conn.query(sql) != 0 && fsLog) { //��ѯʧ�ܲ��ļ�������
        fsLog << "Failed to delete: " << db_conn.error() << endl;
    }
}

inline string DB_Msg::getTime()
{
    char time_buf[64];
    time_t now_time = time(NULL); //����һ����ȡһ����ǰʱ���1970��1��1�յ�����
    strftime(time_buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&now_time));
    return time_buf;
}

inline void DB_Msg::createTable()
{
    db_conn.query("show tables like '" + tbName + "'");
    auto res = db_conn.getResult(); //res�Ƕ�άstring���͵�vector����
    if (res.size() > 0) {
        return;
    }
    string sql = "CREATE TABLE `" + tbName + "` ("
        "`id` tinyint(4) NOT NULL AUTO_INCREMENT,"
        "`fromUser` varchar(20) NOT NULL,"
        "`toUser` varchar(20) NOT NULL,"
        "`time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "`msg` text NOT NULL,"
        "PRIMARY KEY (`id`)"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
    if (fsLog) {
        fsLog << getTime() << ">>>execute: " << sql << endl;
    }
    if (db_conn.query(sql) != 0 && fsLog) {
        fsLog << "Failed to create: " << db_conn.error() << endl;
    }
}

///������Ϣ������-�����ڷ�������
class DB_OfflineMsg :public DB_Msg {
public:
    DB_OfflineMsg(const string& _tableName, const string _logFileName = "")
        : DB_Msg(_tableName, _logFileName)
    {}
    //��������Ϣ�ı��еõ����з��͸�user����Ϣ����ȡ��֮������ݿ�ɾ��
    inline vector<vector<string>> pop(const string& user);
};
//��������Ϣ�ı��еõ����з��͸�user����Ϣ����ȡ��֮������ݿ�ɾ��
inline vector<vector<string>> DB_OfflineMsg::pop(const string& user) {
    string sql = "select fromUser,time,msg from " + tbName + " where toUser='" + user + "'";
    auto result = getResBySql(sql);
    //�ҵ��˷���user����ؼ�¼����size()>0,��ɾ������user��������Ϣ
    if (result.size() > 0) {
        sql = "delete from " + tbName + " where toUser='" + user + "'";
        delResBySql(sql);
    }
    return result;
}

///

#endif

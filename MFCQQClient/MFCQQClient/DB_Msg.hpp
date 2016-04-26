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
    MYSQL* mysql; //mysql连接的句柄
public:
    static string host_, user_, passwd_, db_;
public:
    inline DB_Connector();
    inline virtual ~DB_Connector();
    inline int query(const string& sql); //执行sql语句，成功就返回0，失败则返回非0
    inline string error();
    inline vector<vector<string>> getResult();
};

inline DB_Connector::DB_Connector()
{
    mysql = mysql_init(NULL);
    if (mysql_real_connect(mysql, host_.c_str(), user_.c_str(), passwd_.c_str(), db_.c_str(), 0, 0, 0) == 0) { //连接失败
        string err = "Failed to connect database server: " + error();
        throw std::logic_error(err);
    }
    query("set names 'GBK'"); //设置字符集，防止中文出现乱码
}

inline DB_Connector::~DB_Connector()
{
    mysql_close(mysql); //关闭mysql连接
}

inline int DB_Connector::query(const string & sql)
{
    return mysql_query(mysql, sql.c_str()); //执行sql语句，成功则返回0，失败则返回非0
}

inline string DB_Connector::error()
{
    return mysql_error(mysql); //返回上一次的错误提示
}

inline vector<vector<string>> DB_Connector::getResult()
{
    vector<vector<string>>result;
    MYSQL_RES* res = mysql_store_result(mysql); //分配内存存放query()查找后结果集
    unsigned num = mysql_num_fields(res); //结果集的列数
    for (MYSQL_ROW row; (row = mysql_fetch_row(res)) != NULL;) {
        result.push_back(vector<string>(num)); //预存一行空的，在下面一句中将给该行赋值
        for (size_t cur = result.size() - 1, i = 0; i < num; ++i) { //result.size()指当前行数
            result[cur][i] = row[i];
        }
    }
    mysql_free_result(res);
    return result;
}

///消息管理类
class DB_Msg {
protected:
    DB_Connector db_conn; //数据库连接器对象
    const string tbName; //要操作的表名
    std::ofstream fsLog; //日志记录文件流
public:
    inline DB_Msg(const string& _tableName, const string& _logFileName);
    inline virtual ~DB_Msg();
    //添加消息
    inline void push(const string& from, const string& to, const string& content);
    //获取执行指定的sql语句的结果
    inline vector<vector<string>> getResBySql(const string& sql);
    //根据指定的sql语句删除相应的记录
    inline void delResBySql(const string& sql);
    // 获取当前的日期和时间，用来更新日志
    inline static string getTime(); //不依赖于对象而存在，一般只要该函数未用到类中的数据成员时，就可以声明为静态成员函数
protected:
    //创建存储聊天记录的表
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

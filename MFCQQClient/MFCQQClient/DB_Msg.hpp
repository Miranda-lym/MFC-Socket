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

//数据库连接器类
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

//构造函数中自动完成连接数据库功能
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

//查询数据库
inline int DB_Connector::query(const string & sql)
{
    return mysql_query(mysql, sql.c_str()); //执行sql语句，成功则返回0，失败则返回非0
}

//返回数据库的所有错误提示（如连接、查询、删除、修改等错误）
inline string DB_Connector::error()
{
    return mysql_error(mysql); //返回上一次的错误提示
}

//得到查询后的结果集（是个二维数组）
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
    mysql_free_result(res); //释放结果集，store()后必须free_result()
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
    :tbName(_tableName)
{
    if (_logFileName != "") {
        fsLog.open(_logFileName, std::ios::app); //append,指可在原log文件后追加
    } //若日志名为空字符串，即不想要日志文件，那么就不会调用open()，那么fsLog这个对象就是失效的（相当于false）
    createTable();
}
inline DB_Msg::~DB_Msg()
{
    if (fsLog) { //文件流正常
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
    if (db_conn.query(sql) != 0 && fsLog) { //执行sql语句失败并且日志文件正常，则写日志（执行sql语句失败则返回非0）
        fsLog << "Failed to insert: " << db_conn.error() << endl;
    }
}

inline vector<vector<string>> DB_Msg::getResBySql(const string & sql)
{
    vector<vector<string>> result;
    if (fsLog) {
        fsLog << getTime() << ">>>execute: " + sql << endl;
    }
    if (db_conn.query(sql) == 0) { //执行成功则把结果输出
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
    if (db_conn.query(sql) != 0 && fsLog) { //查询失败并文件流正常
        fsLog << "Failed to delete: " << db_conn.error() << endl;
    }
}

inline string DB_Msg::getTime()
{
    char time_buf[64];
    time_t now_time = time(NULL); //定义一个获取一个当前时间距1970年1月1日的秒数
    strftime(time_buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&now_time));
    return time_buf;
}

inline void DB_Msg::createTable()
{
    db_conn.query("show tables like '" + tbName + "'");
    auto res = db_conn.getResult(); //res是二维string类型的vector数组
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

///离线消息管理类-仅用于服务器端
class DB_OfflineMsg :public DB_Msg {
public:
    DB_OfflineMsg(const string& _tableName, const string _logFileName = "")
        : DB_Msg(_tableName, _logFileName)
    {}
    //从离线消息的表中得到所有发送给user的消息，提取完之后从数据库删除
    inline vector<vector<string>> pop(const string& user);
};
//从离线消息的表中得到所有发送给user的消息，提取完之后从数据库删除
inline vector<vector<string>> DB_OfflineMsg::pop(const string& user) {
    string sql = "select fromUser,time,msg from " + tbName + " where toUser='" + user + "'";
    auto result = getResBySql(sql);
    //找到了发给user的相关记录，则size()>0,则删除发给user的所有消息
    if (result.size() > 0) {
        sql = "delete from " + tbName + " where toUser='" + user + "'";
        delResBySql(sql);
    }
    return result;
}

///

#endif

#pragma once
#include <iostream>
#include <string>

#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>

#include "room_player.hpp"
using namespace std;

#define JUDGE_VALUE(p) ((p != NULL) ? p : "")

class DataBaseSvr
{
    public:
        DataBaseSvr()
        {
            //初始化mysql操作句柄
            mysql_init(&mysql_);
        }

        ~DataBaseSvr()
        {
            //关闭数据库连接
            mysql_close(&mysql_);
        }

        bool Connect2Mysql()
        {
            //连接mqsql服务端
            //操作句柄 ip地址 数据库用户 数据库密码 数据库名称 端口 本地域套接字 unsigned long clientflag
            if(!mysql_real_connect(&mysql_, "49.232.211.4", "lixin", "123", "webgobang",  3306, NULL, CLIENT_FOUND_ROWS))
            {
                return false;
            }
            return true;
        }

        //注册校验接口
        int QueryUserExist(Json::Value& v)
        {
            //1.组织sql语句
            string email = v["email"].asString();
            string password = v["password"].asString();

#define GET_USER_INFO "select * from sys_user where email=\'%s\';"

            char sql[1024] = {0};
            snprintf(sql, sizeof(sql) - 1, GET_USER_INFO, email.c_str());

            //2.查询
            MYSQL_RES* res = NULL;
            if(ExecuteSql(sql, &res) == false)
            {
                return -2;
            }

            //3.针对查询的结果进行处理
            if(mysql_num_rows(res) != 1)
            {
                printf("No Data: sql is %s\n", sql);
                return -3;
            }

			//获取结果集当中的一行数据
            MYSQL_ROW row = mysql_fetch_row(res);
            string passwd_db = JUDGE_VALUE(row[4]);

            if(password != passwd_db)
            {
                return -4;
            }

            mysql_free_result(res);
            return atoi(row[0]);
        }

        int InsertRoomInfo(const Room& r)
        {
            //插入对局当中的落子
            if(ExecuteSql("start transaction;") == false)
            {
                return -1;
            }

            if(ExecuteSql("savepoint start_insert;") == false)
            {
                return -2;
            }

#define INSERT_ROOM_INFO "insert into game_info values(%d, %d, %d, %d, %d, \'%s\');"

            for(size_t i = 0; i < r.step_vec_.size(); i++)
            {
                char sql[1024] = {0};
                snprintf(sql, sizeof(sql) - 1, INSERT_ROOM_INFO, r.room_id_, r.p1_, r.p2_, r.winner_, r.p1_, r.step_vec_[i].c_str());

                if(ExecuteSql(sql) == false)
                {
                    ExecuteSql("rollback to start_insert;");
                    ExecuteSql("commit;");
                    return -3;
                }
            }

            ExecuteSql("commit;");
            return 0;
        }

        bool ExecuteSql(const string& sql, MYSQL_RES** res)
        {
            //1.设置当前客户的字符集
            mysql_query(&mysql_, "set names utf8");

            if(mysql_query(&mysql_, sql.c_str()) != 0)
            {
                return false;
            }

            *res = mysql_store_result(&mysql_);
            if(res == NULL)
            {
                return false;
            }
            return true;
        }

        bool ExecuteSql(const string& sql)
        {
            mysql_query(&mysql_, "set names utf8");

            if(mysql_query(&mysql_, sql.c_str()) != 0)
            {
                return false;
            }
            return true;
        }

        bool AddUser(Json::Value& v)
        {
            string name = v["name"].asString();
            string password = v["passwd"].asString();
            string email = v["email"].asString();
            string phone_num = v["phonenum"].asString();

#define ADD_USER "insert into sys_user(name, email, phone_num, passwd) values(\'%s\', \'%s\', \'%s\', \'%s\');"
            char sql[1024] = {0};
            snprintf(sql, sizeof(sql) - 1, ADD_USER, name.c_str(), email.c_str(), phone_num.c_str(), password.c_str());
            return ExecuteSql(sql);
        }

    private:
        MYSQL mysql_;
};

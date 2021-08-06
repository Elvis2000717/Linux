/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：webgoang.hpp
*   创 建 者：wudu
*   创建日期：2021年08月04日
*   描    述：
*
================================================================*/

#include <iostream>
#include <string>

#include <jsoncpp/json/json.h>
#include "httplib.h"
#include "database.hpp"


using namespace httplib;
using namespace std;

class WebGobang
{
    public:
        WebGobang()
        {
            db_svr_ = NULL;
        }

        ~WebGobang()
        {
            if(db_svr_)
            {
                delete db_svr_;
            }
        }

        /*
         * 初始化用户管理模块的类指针
         * 初始化数据库模块的类指针
         * ...
         * */
        int Init()
        {
            db_svr_ = new DataBaseSvr();
            if(db_svr_ == NULL)
            {
                return -1;
            }

            return 0;
        }

        int StartWebGobang()
        {
            http_svr_.Post("/login", [](const Request& res, Response& resp){
                   cout << res.body << endl; 
                   Json::Reader r;
                   Json::Value v;
                   r.parse(res.body, v);

                   cout <<"v[\"email\"]: " << v["email"] << endl;
                   cout << "v[\"password\"]: "<<v["password"] << endl;

                    });

            http_svr_.set_mount_point("/", "./www");
            http_svr_.listen("0.0.0.0", 37878);
        }
    private:
       Server http_svr_; 

       DataBaseSvr* db_svr_;
};

#include <iostream>
#include <cstdio>
#include <json/json.h>
#include "httplib.h"

#include "oj_model.hpp"
#include "oj_view.hpp"
#include "compile.hpp"
#include "tools.hpp"

int main()
{
    using namespace httplib;
    OjModel model;
    //1.初始化httplib库的server对象
    Server svr;

    //2.提供三个接口，分别处理三个请求
    //2.1 获取整个试题列表 get
    svr.Get("/all_questions", [&model](const Request& req, Response& resp){
            //1.返回试题列表
            std::vector<Question> questions;
            model.GetAllQuestion(&questions);

            //for(int i = 0; i < questions.size(); i++)
            //{
            //    std::cout << questions[i].id_ << " " << questions[i].title_ << " " << questions[i].star_ << " " << questions[i].path_ << std::endl;
            //}

            std::string html;
            OjView::DrawAllQuestions(questions, &html);

            resp.set_content(html, "text/html");
            });

    //2.2 获取单个试题
    // 如何标识浏览器想要获取的是哪一个试题？
    // \d：表示一个数字
    // \d+：表示多位数字
    svr.Get(R"(/question/(\d+))", [&model](const Request& req, Response& resp){
            //1.获取url当中关于试题的数字 & 获取单个试题的信息
            //std::cout << req.matches[0] << " " << req.matches[1] << std::endl;
            Question ques;
            model.GetOneQuestion(req.matches[1].str(), &ques);
            
            //2.渲染模板html文件
            std::string html;
            OjView::DrawOneQuestion(ques, &html); 

            resp.set_content(html,"text/html");
            });

    //2.3 编译运行
    //目前还没有区分到底是哪一个试题 
    svr.Post(R"(/compile/(\d+))", [&model](const Request& req, Response& resp){
            //1.获取试题编号 & 获取试题内容
            Question ques;
            model.GetOneQuestion(req.matches[1].str(), &ques);
            //ques.tail_cpp_ ==> main函数调用+测试用例
            
            //post 方法在提交代码的时候，是经过encode的，想要正常获取浏览器提交的内容，需要进行decode，使用decode完成的代码和tail.cpp进行合并，产生待编译的源码
            //key：value
            //code：xcsnasucnbjske
            //UrlUtil::UrlDecode(req.body);
            
            std::unordered_map<std::string, std::string> body_kv;
            UrlUtil::PraseBody(req.body, &body_kv);

            //std::vector<std::string> vec;
            //StringUtil::Split(UrlUtil::UrlDecode(req.body), "=", &vec);
            //for(int i = 0; i < vec.size(); i++)
            //{
            //    std::cout << vec[i] << std::endl;
            //}
            
            std::cout << body_kv["code"] << std::endl;
            
            std::string user_code = body_kv["code"];

            //2.构造json对象，交给编译运行模块
            Json::Value req_json;
            req_json["code"] = user_code + ques.tail_cpp_;
            req_json["stdin"] = "";

            std::cout << req_json["code"].asString() << std::endl;

            Json::Value resp_json;
            Compiler::CompileAndRun(req_json, &resp_json);

            //获取的返回结果都在 resp_json当中
            std::string err_no = resp_json["errorno"].asString();
            std::string case_result = resp_json["stdout"].asString();
            std::string reason = resp_json["reason"].asString();

            std::string html;
            OjView::DrawCaseResult(err_no, case_result, reason, &html);
            resp.set_content(html, "text/html");

            });

    LOG(INFO, "listen_port") << ":17878" << std::endl;
    //当前服务器的逻辑根目录
    svr.set_base_dir("./www");
    svr.listen("0.0.0.0", 17878);

    return 0;
}

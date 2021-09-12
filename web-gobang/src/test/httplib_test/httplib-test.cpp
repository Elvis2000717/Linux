#include <unistd.h>
#include <stdio.h>

#include <iostream>

#include "httplib.h"

using namespace httplib;

void CallBack(const Request& res, Response& resp)
{
    (void)res;
    resp.set_content("hello", "text/plain");
}

int main()
{
    Server http_svr;
    http_svr.Get("/abc", CallBack);

    http_svr.set_mount_point("/", "./www");
    http_svr.listen("0.0.0.0", 28989);
    return 0;
}


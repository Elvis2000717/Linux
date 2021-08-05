/*================================================================
*   Copyright (C) 2021 Sangfor Ltd. All rights reserved.
*   
*   文件名称：webgoang.cpp
*   创 建 者：wudu
*   创建日期：2021年08月04日
*   描    述：
*
================================================================*/


#include "webgobang.hpp"

int main()
{
    WebGobang* wg = new WebGobang();
    if(wg == NULL)
    {
        printf("init web gobang failed\n");
        return 0;
    }

    if(wg->Init() < 0)
    {
       return 0; 
    }

    wg->StartWebGobang();

    delete wg;
    return 0;
}

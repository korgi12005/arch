#define _CRT_SECURE_NO_WARNINGS
#include "AppClient.h"
#include "helpers/SocketClient.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"
#include <sys/stat.h>
#include <map>
#include <chrono>
#include <iostream>
//#include <Windows.h>

bool Client::send(const std::string& url, const std::string& msg)
{
    struct _stat buf;
    int result;
    SocketClient s;
    int time = 0;
    while ((!s.init() || !s.connect(url))&&time<30) {
        time+=2;
        Sleep(10);
    }
    result = _stat(msg.c_str(), &buf);
    printf("%i \n",result);
    int len=0;
    printf("sending text message \"%s\"\n", msg.c_str());
    if (result < 0) {
         len = s.sendStr(msg);
         printf("str");
    }
    else {
        printf("asfasfaksg");
        len = s.sendFile(msg);
    }
    printf("sent %d bytes\n", len);
    return len > 0;
}

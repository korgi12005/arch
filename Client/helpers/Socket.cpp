#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <winsock2.h>
#include <WS2tcpip.h>
#include "Socket.h"
#include "UtilString.h"
#include <experimental/filesystem>
#include <fstream>
#include <sys/stat.h>
#include <iostream>
#include "UtilFile.h"

static int sSocketId = 0;

Socket::Socket()
{
    if(++sSocketId == 1)
    {
        WSAData wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData); // this is executed once during app startup
    }
}

Socket::~Socket()
{
    close();

    if(--sSocketId == 0)
        WSACleanup(); // this is executed once during app teardown
}

bool Socket::init(DWORD timeout)
{
    close();
    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket error\n");
        return false;
    }

    if(timeout)
        setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(DWORD));
    m_timeout = timeout;
    return true;
}

bool Socket::isValid()
{
    return m_socket >= 0;
}

int Socket::send(const char* msg, int len)
{
    if(len <= 0)
        return 0;
    printf("SEND\n----------\n%s\n----------\n", msg);
    return ::send(m_socket, msg, len, 0);
}

int Socket::sendStr(const std::string& str)
{
    std::string mes = "S ";
    mes += str;
    return send(mes.c_str(), mes.length());
}

int Socket::sendFile(const std::string& str)
{

    std::string mes = "F ";
    mes += std::experimental::filesystem::path(str).filename().string() + " ";
    std::ifstream input_file(str, std::ios::binary);
    std::string file_contents((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    mes += file_contents;

    return send(mes.c_str(), mes.length());
}

int Socket::recv()
{
    int cur = 0;
    int total = 0;
    const int CHUNK = 1024;
    do
    {
        if(m_recv.size() < total + CHUNK)
            m_recv.resize(m_recv.size() + CHUNK);
        cur = ::recv(m_socket, m_recv.data()+total, m_recv.size()-total-1, 0);
        if(cur > 0)
            total += cur;
    }
    while(cur > 0);
    m_recv[total] = 0;
    return total;
}

char* Socket::data()
{
    return m_recv.data();
}

void Socket::close()
{
    if(m_socket != -1)
    {
        shutdown(m_socket, SD_BOTH);
        closesocket(m_socket);
    }
}

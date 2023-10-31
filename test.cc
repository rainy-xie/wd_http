#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <memory>

class TcpConn
{
public:
    TcpConn(const char *ip, const char *port) : _ip(ip), _port(port)
    {
    }
    int Start()
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(_ip);
        addr.sin_port = htons(atoi(_port));
        int reuse = 1;
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
        int ret = bind(_sockfd, (struct sockaddr *)&addr, sizeof(addr));
        listen(_sockfd, 10);
        return 0;
    }
    int RecvAndShow()
    {
        int netfd = accept(_sockfd, NULL, NULL); // 更好的方式是使用RAII，将accept放入构造函数中，将close放入析构函数中
        std::unique_ptr<char[]> buf(new char[4096]);
        bzero(buf.get(), 4096);
        int ret = recv(netfd, buf.get(), 4096, 0);
        fprintf(stderr, "%s\n", buf.get());
        std::string firstLine = "HTTP/1.1 200 OK\r\n";
        send(netfd, firstLine.c_str(), firstLine.size(), 0);
        std::string type = "Content-Type:text/plain\r\n"
                           "Content-Length:5\r\n";
        send(netfd, type.c_str(), type.size(), 0);
        std::string emptyline = "\r\n";
        send(netfd, emptyline.c_str(), emptyline.size(), 0);
        std::string content = "hello";
        send(netfd, content.c_str(), content.size(), 0);
        close(netfd);
        fprintf(stderr, "closed\n");
        return 0;
    }

private:
    const char *_ip;
    const char *_port;
    int _sockfd;
};
int main()
{
    TcpConn conn("172.27.29.195", "1280");
    conn.Start();
    while (1)
    {
        conn.RecvAndShow();
    }
}
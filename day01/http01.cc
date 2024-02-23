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

    int start()
    {
        // 把套接字和地址bind到一起后listen
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(_ip);
        addr.sin_port = htons(atoi(_port));

        // 调整套接字的属性
        int reuse = 1;
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
        int ret = bind(_sockfd, (struct sockaddr *)&addr, sizeof(addr));
        listen(_sockfd, 10);
        return 0;
    }

    int RecAndShow()
    {
        // 取出一条连接
        //  netfd为监听到的客户端的fd  https://blog.csdn.net/qq_33573235/article/details/79292679
        int netfd = accept(_sockfd, NULL, NULL);
        // 使用RAII管理缓冲区
        // 更好的方式是使用RAII，将accept放入构造函数中，将close放入析构函数中
        std::unique_ptr<char[]> buf(new char[4096]);
        bzero(buf.get(), 4096);

        // 读取客户端的请求内容
        int ret = recv(netfd, buf.get(), 4096, 0);
        fprintf(stderr, "%s\n", buf.get());

        // 发送火车头
        std::string firstLine = "HTTP/1.1 200 OK\r\n";
        send(netfd, firstLine.c_str(), firstLine.size(), 0);
        std::string headers = "Content-Type:text/html\r\n"
                              "Content-Length:229\r\n";
        send(netfd, headers.c_str(), headers.size(), 0);

        int fd = open("post.html", O_RDONLY);
        bzero(buf.get(), 4096);
        int ret1 = read(fd, buf.get(), 4096);
        send(netfd, buf.get(), ret1, 0);
        close(fd);
        // // TCP链接的半关闭，使得hello可以被接收
        // shutdown(netfd, SHUT_WR);
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
    conn.start();
    while (1)
    {
        conn.RecAndShow();
    }
}

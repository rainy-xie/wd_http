#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/WFHttpServer.h>
#include <workflow/HttpUtil.h>
static WFFacilities::WaitGroup waitGroup(1);
void sigHandler(int num){
    waitGroup.done();
    fprintf(stderr,"wait group is done\n");
}
void process(WFHttpTask *serverTask){
    auto resp = serverTask->get_resp();
    resp->set_http_version("HTTP/1.1");
    resp->set_status_code("200");
    resp->set_reason_phrase("OK");
    resp->set_header_pair("Content-Type","text/plain");
    resp->append_output_body("hello");
    serverTask->set_callback([](WFHttpTask *serverTask){
        fprintf(stderr,"serverTask Callback is running!\n");
    });
    fprintf(stderr,"process is running!\n");

    auto req = serverTask->get_req();
    fprintf(stderr,"%s %s %s\n", req->get_method(),
                                 req->get_request_uri(),
                                 req->get_http_version());
    protocol::HttpHeaderCursor cursor(req);
    std::string name;
    std::string value;
    while(cursor.next(name,value)){
        fprintf(stderr,"%s:%s\n",name.c_str(),value.c_str());
    }

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    serverTask->get_peer_addr((sockaddr *)&addr,&len);
    if(addr.sin_family == AF_INET){
        fprintf(stderr,"sin_family:AF_INET\n");
        fprintf(stderr,"ip:%s\n", inet_ntoa(addr.sin_addr));
        fprintf(stderr,"port:%d\n",ntohs(addr.sin_port));
    }
}
int main(){
    signal(SIGINT,sigHandler);
    WFHttpServer server(process);
    if(server.start(1234) == 0){
        waitGroup.wait();
        server.stop();
    }
    else{
        perror("server start failed\n");
        return -1;
    }
    return 0;   
}
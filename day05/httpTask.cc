#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/HttpMessage.h>
#include <workflow/HttpUtil.h>
static WFFacilities::WaitGroup waitGroup(1);
void sigHandler(int num){
    waitGroup.done();
    fprintf(stderr,"wait group is done\n");
}
void callback(WFHttpTask *httpTask){
    //在回调函数中，可以获取任务的所有信息的
    protocol::HttpRequest *req =  httpTask->get_req();
    protocol::HttpResponse *resp = httpTask->get_resp();
    int state = httpTask->get_state();
    int error = httpTask->get_error();
    switch (state)
    {
    case WFT_STATE_SYS_ERROR:
        fprintf(stderr,"system error: %s\n", strerror(error));
        break;
    case WFT_STATE_DNS_ERROR:
        fprintf(stderr,"dns error: %s\n", gai_strerror(error));
        break;
    case WFT_STATE_SUCCESS:
        break;
    }
    if(state != WFT_STATE_SUCCESS){
        fprintf(stderr,"Failed\n");
        return;
    }
    else{
        fprintf(stderr,"Success!\n");
    }

    fprintf(stderr,"request\r\n %s %s %s\r\n", req->get_method(),
                                               req->get_request_uri(),
                                               req->get_http_version());

    //使用迭代器来遍历首部字段
    std::string name;
    std::string value;
    protocol::HttpHeaderCursor reqCursor(req);
    while(reqCursor.next(name,value)){
        fprintf(stderr,"%s:%s\r\n",name.c_str(),value.c_str());
    }
    fprintf(stderr,"\r\n");

    fprintf(stderr,"response\r\n %s %s %s\r\n", resp->get_http_version(),
                                                resp->get_status_code(),
                                                resp->get_reason_phrase());
    protocol::HttpHeaderCursor respCursor(resp);
    while(respCursor.next(name,value)){
        fprintf(stderr,"%s:%s\r\n",name.c_str(),value.c_str());
    }
    fprintf(stderr,"\r\n");

    //print response body
    const void *body;
    size_t size;
    resp->get_parsed_body(&body,&size);
    //get_parsed_body方法会修改指针变量body的指向
    fwrite(body,1,size,stderr);                                   
}
//异步+回调
int main(){
    signal(SIGINT,sigHandler);
    //WFHttpTask *httpTask = WFTaskFactory::create_http_task("http://172.27.29.195:1235", 0, 0, callback);
    WFHttpTask *httpTask = WFTaskFactory::create_http_task("http://baidu.com", 0, 0, callback);
    protocol::HttpRequest *req =  httpTask->get_req();
    req->add_header_pair("Accept","*/*");
    req->add_header_pair("User-Agent","myHttpTask");
    req->set_header_pair("Connection","close");
    httpTask->start();
    waitGroup.wait();
}
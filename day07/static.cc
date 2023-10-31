#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/WFHttpServer.h>
#include <workflow/HttpUtil.h>
static WFFacilities::WaitGroup waitGroup(1);
void sigHandler(int num){
    waitGroup.done();
    fprintf(stderr,"wait group is done\n");
}
struct SeriesContext{
    WFHttpTask *serverTask;
    int fd;
    char *buf;
    size_t filesize;
};
void IOCallback(WFFileIOTask *IOTask){
    SeriesContext *context = static_cast<SeriesContext *>(series_of(IOTask)->get_context());
    auto resp2client = context->serverTask->get_resp();
    resp2client->add_header_pair("Content-Type","text/html");
    resp2client->append_output_body(context->buf,context->filesize);
}
void process(WFHttpTask *serverTask){
    // 1 创建文件IO任务
    size_t filesize = 614;
    int fd = open("postform.html",O_RDONLY);
    char *buf = new char[filesize];
    auto IOTask = WFTaskFactory::create_pread_task(fd,buf,filesize,0,IOCallback);
    // 2 把文件IO任务加入到序列中
    series_of(serverTask)->push_back(IOTask);
    // 3 创建传递给IOTask的context
    SeriesContext *context = new SeriesContext;
    context->serverTask =  serverTask;
    context->fd = fd;
    context->buf = buf;
    context->filesize = filesize;
    series_of(serverTask)->set_context(context);
    // 4 设置序列的回调函数，释放所有资源
    series_of(serverTask)->set_callback([](const SeriesWork *series){
        fprintf(stderr,"series callback\n");
        SeriesContext * context = static_cast<SeriesContext *>(series->get_context());
        delete[] context->buf;
        close(context->fd);
        delete context;
    });
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
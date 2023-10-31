#include "linuxheader.h"
#include <vector>
#include <workflow/WFFacilities.h>
#include <workflow/Workflow.h>
#include <workflow/HttpUtil.h>
static WFFacilities::WaitGroup waitGroup(1);
struct SeriesContext{
    std::string url;
    int state;
    int error;
    protocol::HttpResponse resp;//响应报文的完整内容
};
void sigHandler(int num){
    waitGroup.done();
    fprintf(stderr,"wait group is done\n");
}
void parallelCallback(const ParallelWork *pwork){
    fprintf(stderr,"pwork callback!\n");
    SeriesContext *context;
    for(size_t i = 0; i != pwork->size(); ++i){
        context = static_cast<SeriesContext *>(pwork->series_at(i)->get_context());
        fprintf(stderr,"url = %s\n", context->url.c_str());
        if(context->state == WFT_STATE_SUCCESS){
            const void *body;
            size_t size;
            context->resp.get_parsed_body(&body,&size);
            fwrite(body,1,size,stderr); 
            fprintf(stderr,"\n");
        }
        else{
            fprintf(stderr,"Error, state = %d, error = %d\n", context->state, context->error);
        }
        delete context;
    }
}
void httpCallback(WFHttpTask *httpTask){
    SeriesContext *context = static_cast<SeriesContext *>(series_of(httpTask)->get_context());
    fprintf(stderr,"httpTask callback, url = %s\n", context->url.c_str());
    context->state = httpTask->get_state();
    context->error = httpTask->get_error();
    context->resp = std::move(*httpTask->get_resp());
}
int main(){
    signal(SIGINT,sigHandler);
    //使用工厂函数，创建一个并行任务
    ParallelWork *pwork = Workflow::create_parallel_work(parallelCallback);//Workflow::create_parallel_work

    std::vector<std::string> urlVec = {"http://www.bing.com", "http://172.27.29.195", "http://172.27.29.195"};
    for(size_t i = 0; i != urlVec.size() ; ++i){
        //创建若干个任务
        // WFTaskFactory::create_http_task
        std::string url = urlVec[i];
        auto httpTask = WFTaskFactory::create_http_task(url,0,5,httpCallback);

        // 修改任务的属性
        auto req = httpTask->get_req();
        req->add_header_pair("Accept","*/*");
        req->add_header_pair("User-Agent","myHttpTask");
        req->set_header_pair("Connection", "Close");
        //为响应的内容申请一片堆空间
        SeriesContext *context = new SeriesContext;
        context->url = std::move(url);
        // 为每个任务创建一个序列
        auto series = Workflow::create_series_work(httpTask,nullptr);
        // 把存储响应内容的指针 拷贝到序列的context当中。
        series->set_context(context);
        //把序列加入到并行任务中
        // add_series
        pwork->add_series(series);
    }
    
    Workflow::start_series_work(pwork,nullptr);
    

    //启动并行任务
    waitGroup.wait();
}
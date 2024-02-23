#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/Workflow.h>
#include <workflow/HttpUtil.h>
#include <vector>

static WFFacilities::WaitGroup waitGroup(1);

struct SeriesContext
{
    std::string url;
    int state;
    int error;
    protocol::HttpResponse resp;
};

void sigHander(int num)
{
    waitGroup.done();
    fprintf(stderr, "wait group is done\n");
}

void parallelCallback(const ParallelWork *pwork)
{
    fprintf(stderr, "pwork callback!\n");
    SeriesContext *context;
    for (size_t i = 0; i != pwork->size(); i++)
    {
        context = static_cast<SeriesContext *>(pwork->series_at(i)->get_context());
        fprintf(stderr, "url = %s\n", context->url.c_str());
        if (context->state == WFT_STATE_SUCCESS)
        {
            const void *body;
            size_t size;
            context->resp.get_parsed_body(&body, &size);
            fwrite(body, 1, size, stderr);
            fprintf(stderr, "\n");
        }
        else
        {
            fprintf(stderr, "Error, state = %d, error = %d\n", context->state, context->error);
        }
        delete context;
    }
}

void httpCallback(WFHttpTask *httpTask)
{
    SeriesContext *context = static_cast<SeriesContext *>(series_of(httpTask)->get_context());
    fprintf(stderr, "httpTask callback, url = %s\n", context->url.c_str());
    context->state = httpTask->get_state();
    context->error = httpTask->get_error();
    context->resp = std::move(*httpTask->get_resp());
}

int main()
{
    signal(SIGINT, sigHander);

    ParallelWork *pwork = Workflow::create_parallel_work(parallelCallback);
    std::vector<std::string> urlVec = {"http://baidu.com", "http://jd.com", "http://bing.com"};
    for (size_t i = 0; i != urlVec.size(); i++)
    {
        std::string url = urlVec[i];
        auto httpTask = WFTaskFactory::create_http_task(url, 0, 5, httpCallback);

        auto req = httpTask->get_req();
        req->add_header_pair("Accept", "*/*");
        req->add_header_pair("User-Agent", "myHttpTask");
        req->add_header_pair("Connection", "Close");

        SeriesContext *context = new SeriesContext;
        context->url = std::move(url);
        auto series = Workflow::create_series_work(httpTask, nullptr);
        series->set_context(context);
        pwork->add_series(series);
    }
    Workflow::start_series_work(pwork, nullptr);

    waitGroup.wait();
}

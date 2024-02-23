//反向代理
#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/WFHttpServer.h>
#include <workflow/HttpUtil.h>
static WFFacilities::WaitGroup waitGroup(1);
void sigHandler(int num)
{
    waitGroup.done();
    fprintf(stderr, "wait group is done\n");
}

struct SeriesContext
{
    WFHttpTask *serverTask;
};

void httpCallback(WFHttpTask *httpTask)
{
    protocol::HttpRequest *req = httpTask->get_req();
    protocol::HttpResponse *resp = httpTask->get_resp();
    int state = httpTask->get_state();
    int error = httpTask->get_error();
    switch (state)
    {
    case WFT_STATE_SYS_ERROR:
        fprintf(stderr, "system error: %s\n", strerror(error));
        break;
    case WFT_STATE_DNS_ERROR:
        fprintf(stderr, "dns error: %s\n", gai_strerror(error));
        break;
    case WFT_STATE_SUCCESS:
        break;
    }
    if (state != WFT_STATE_SUCCESS)
    {
        fprintf(stderr, "Failed\n");
        return;
    }
    else
    {
        fprintf(stderr, "Success!\n");
    }

    const void *body;
    size_t size;
    resp->get_parsed_body(&body, &size);

    SeriesContext *context = static_cast<SeriesContext *>(series_of(httpTask)->get_context());
    protocol::HttpResponse *resp2client = context->serverTask->get_resp();
    *resp2client = std::move(*resp);
}

void process(WFHttpTask *serverTask)
{
    auto httpTask = WFTaskFactory::create_http_task("http://rainy-xie.github.io/", 0, 0, httpCallback);
    series_of(serverTask)->push_back(httpTask);
    SeriesContext *context = new SeriesContext;
    context->serverTask = serverTask;
    series_of(serverTask)->set_context(context);
    serverTask->set_callback([](WFHttpTask *serverTask)
                             { fprintf(stderr, "server task callback\n"); });
    series_of(serverTask)->set_callback([](const SeriesWork *series)
                                        { 
            fprintf(stderr, "series callback\n");
            SeriesContext* context = static_cast<SeriesContext*>(series->get_context());
            delete context; });
}

int main()
{
    signal(SIGINT, sigHandler);
    WFHttpServer server(process);
    if (server.start(1237) == 0)
    {
        waitGroup.wait();
        server.stop();
    }
    else
    {
        perror("server start failed\n");
        return -1;
    }
    return 0;
}
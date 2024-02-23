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
    int fd;
    char *buf;
    size_t filesize;
};

void IOCallback(WFFileIOTask *IOTask)
{
    SeriesContext *context = static_cast<SeriesContext *>(series_of(IOTask)->get_context());
    auto resp2client = context->serverTask->get_resp();
    resp2client->add_header_pair("Content-Type", "text/html");
    resp2client->append_output_body(context->buf, context->filesize);
    fprintf(stderr, "IO Callback\n");
}

void process(WFHttpTask *serverTask)
{
    size_t filesize = 614;
    int fd = open("postform.html", O_RDONLY);
    char *buf = new char[filesize];
    auto IOTask = WFTaskFactory::create_pread_task(fd, buf, filesize, 0, IOCallback);

    series_of(serverTask)->push_back(IOTask);

    SeriesContext *context = new SeriesContext;
    context->serverTask = serverTask;
    context->buf = buf;
    context->fd = fd;
    context->filesize = filesize;
    series_of(serverTask)->set_context(context);

    series_of(serverTask)->set_callback([](const SeriesWork *series)
                                        { fprintf(stderr, "series callback\n");
                                        SeriesContext* context = static_cast<SeriesContext*>(series->get_context());
                                        delete[] context->buf;
                                        close(context->fd);
                                        delete context; });
}

int main()
{
    signal(SIGINT, sigHandler);
    WFHttpServer server(process);
    if (server.start(1239) == 0)
    {
        waitGroup.wait();
        server.stop();
    }
    else
    {
        perror("server start failed\n");
    }
    waitGroup.wait();
}
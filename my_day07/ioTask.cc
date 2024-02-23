#include "linuxheader.h"
#include <workflow/WFFacilities.h>
static WFFacilities::WaitGroup waitGroup(1);
struct FileData
{
    char *buf;
    int fd;
};

void callback(WFFileIOTask *IOTask)
{
    FileData *filedata = static_cast<FileData *>(IOTask->user_data);
    fprintf(stderr, "buf = %s\n", filedata->buf);
    delete[] filedata->buf;
    close(filedata->fd);
    delete filedata;
}

void sigHandler(int num)
{
    waitGroup.done();
    fprintf(stderr, "wait group is done\n");
}

int main()
{
    signal(SIGINT, sigHandler);
    int fd = open("file", O_RDONLY);
    char *buf = new char[10];
    FileData *filedata = new FileData;
    filedata->buf = buf;
    filedata->fd = fd;
    WFFileIOTask *IOTask = WFTaskFactory::create_pread_task(fd, buf, 5, 0, callback);
    IOTask->user_data = filedata;
    IOTask->start();
    waitGroup.wait();
}
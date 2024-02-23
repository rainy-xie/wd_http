#include "linuxheader.h"
#include <workflow/WFFacilities.h>
static WFFacilities::WaitGroup waitGroup(1);
void callback(WFTimerTask *timeTask)
{
    fprintf(stderr, "timer callback\n");
}
void sigHandler(int num)
{
    waitGroup.done();
    fprintf(stderr, "wait group is done\n");
}
int main()
{
    signal(SIGINT, sigHandler);

    WFTimerTask *timerTask = WFTaskFactory::create_timer_task(2 * 1000000, callback);
    timerTask->start();
    waitGroup.wait();
}
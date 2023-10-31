#include "linuxheader.h"
#include <workflow/WFFacilities.h>
static WFFacilities::WaitGroup waitGroup(1);
void callback(WFTimerTask *timerTask){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    printf("callback %ld.%06ld\n", tv.tv_sec, tv.tv_usec);
    WFTimerTask *nextTask = WFTaskFactory::create_timer_task(2*1000000,callback);
    series_of(timerTask)->push_back(nextTask);
}
void sigHandler(int num){
    waitGroup.done();
    fprintf(stderr,"wait group is done\n");
}
int main(){
    signal(SIGINT,sigHandler);
    WFTimerTask *timerTask = WFTaskFactory::create_timer_task(2*1000000,callback);
    timerTask->start();
    struct timeval tv;
    gettimeofday(&tv,NULL);
    printf("main %ld.%06ld\n", tv.tv_sec, tv.tv_usec);
    waitGroup.wait();
}
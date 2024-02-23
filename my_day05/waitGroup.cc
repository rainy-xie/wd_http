#include "linuxheader.h"
#include <workflow/WFFacilities.h>

static WFFacilities::WaitGroup waitGroup(1);
void callback(){}
void sigHander(int num)
{
    waitGroup.done();
    fprintf(stderr, "wait group is done\n");
}

int main()
{
    signal(SIGINT, sigHander);
    waitGroup.wait();
}

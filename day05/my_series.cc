#include "linuxheader.h"
#include <workflow/WFFacilities.h>
static WFFacilities::WaitGroup waitGroup(1);
void sigHander(int num)
{

}

void seriesCallback()
{

}

void callBack()
{

}
int main()
{
    signal(SIGINT, sigHander);
    std::string *pkey = new std::string("43key2");
    
}

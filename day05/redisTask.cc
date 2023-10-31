#include "linuxheader.h"
#include <workflow/WFFacilities.h>
static WFFacilities::WaitGroup waitGroup(1);
void callback(){}
void sigHandler(int num){
    waitGroup.done();
    fprintf(stderr,"wait group is done\n");
}
int main(){
    signal(SIGINT,sigHandler);
    //创建redis任务
    WFRedisTask *redisTask = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379",0,[](WFRedisTask *redisTask){
        protocol::RedisRequest *req = redisTask->get_req();
        protocol::RedisResponse *resp = redisTask->get_resp();
        int state = redisTask->get_state();
        int error = redisTask->get_error();
        protocol::RedisValue value;//value对象专门用来存储redis任务的结果
        switch (state){
        case WFT_STATE_SYS_ERROR:
            fprintf(stderr,"system error: %s\n", strerror(error));
            break;
        case WFT_STATE_DNS_ERROR:
            fprintf(stderr,"dns error: %s\n", gai_strerror(error));
            break;
        case WFT_STATE_SUCCESS:
            resp->get_result(value);
            if(value.is_error()){
                fprintf(stderr,"redis error\n");
                state = WFT_STATE_TASK_ERROR;
            }
            break;
        }

        if (state != WFT_STATE_SUCCESS){
            fprintf(stderr, "Failed\n");
            return;
        }
        else{
            fprintf(stderr, "Success!\n");
        }

        std::string cmd;
        req->get_command(cmd);
        fprintf(stderr,"redis request, cmd = %s\n", cmd.c_str());
        if(value.is_string()){
            fprintf(stderr,"value is a string, value = %s\n", value.string_value().c_str());
        }
        else if(value.is_array()){
            fprintf(stderr,"value is string array\n");
            for(size_t i = 0; i < value.arr_size(); ++i){
                fprintf(stderr,"value at %lu = %s\n", i, value.arr_at(i).string_value().c_str());
            } 
        }

    });
    //设置redis任务的属性
    protocol::RedisRequest *req = redisTask->get_req();
    //req->set_request("SET",{"43key", "100"});
    //req->set_request("GET",{"43key"});
    req->set_request("HGETALL",{"43t"});
    //启动redis任务
    redisTask->start();
    waitGroup.wait();
}
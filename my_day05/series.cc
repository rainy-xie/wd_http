#include "linuxheader.h"
#include <workflow/WFFacilities.h>

static WFFacilities::WaitGroup waitGroup(1);

void seriesCallback(const SeriesWork *series)
{
    fprintf(stderr, "series callback, free pkey\n");
    std::string *pkey = static_cast<std::string *>(series->get_context());
    delete pkey;
}
void callback(WFRedisTask *redisTask)
{
    int state = redisTask->get_state();
    int error = redisTask->get_error();
    protocol::RedisValue value; // value专门存储redis任务的结果
    protocol::RedisRequest *req = redisTask->get_req();
    protocol::RedisResponse *resp = redisTask->get_resp();

    switch (state)
    {
    case WFT_STATE_SYS_ERROR:
        fprintf(stderr, "system error: %s\n", strerror(error));
        break;
    case WFT_STATE_DNS_ERROR:
        fprintf(stderr, "dns error: %s\n", strerror(error));
    case WFT_STATE_SUCCESS:
        resp->get_result(value);
        if (value.is_error())
        {
            state = WFT_STATE_TASK_ERROR;
            fprintf(stderr, "redis error\n");
        }
        break;
    }
    if (state != WFT_STATE_SUCCESS)
    {
        fprintf(stderr, "Failed\n");
        return;
    }
    else
    {
        fprintf(stderr, "success!\n");
    }

    std::string cmd;
    req->get_command(cmd);
    if (cmd == "SET")
    {
        // firstTask
        fprintf(stderr, "first task callback begins\n");
        std::string *pkey = static_cast<std::string *>(redisTask->user_data);
        WFRedisTask *secondTask = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379", 0, callback);
        protocol::RedisRequest *req = secondTask->get_req();
        req->set_request("GET", {*pkey});
        SeriesWork *series = series_of(redisTask);
        series->set_context(static_cast<void *>(pkey));
        series->set_callback(seriesCallback);
        series->push_back(secondTask);
        fprintf(stderr, "first task callback ends\n");
    }
    else
    {
        // secondTask
        fprintf(stderr, "second task callback begins\n");
        fprintf(stderr, "redis request, cmd = %s\n", cmd.c_str());
        if (value.is_string())
        {
            fprintf(stderr, "value is a string, value = %s\n", value.string_value().c_str());
        }
        else if (value.is_array())
        {
            fprintf(stderr, "value is string array\n");
            for (size_t i = 0; i < value.arr_size(); ++i)
            {
                fprintf(stderr, "value at %lu = %s\n", i, value.arr_at(i).string_value().c_str());
            }
        }
        fprintf(stderr, "second task callback ends\n");
    }
}
void sigHander(int num)
{
    waitGroup.done();
    fprintf(stderr, "wait group is done\n");
}

int main()
{
    signal(SIGINT, sigHander);

    // 创建redis任务
    std::string *pkey = new std::string("43key2");
    WFRedisTask *firstTask = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379", 0, callback);

    // 设置redis任务的属性
    protocol::RedisRequest *req = firstTask->get_req();
    req->set_request("SET", {*pkey, "200"});
    firstTask->user_data = static_cast<void *>(pkey);

    // 启动redis任务
    firstTask->start();
    waitGroup.wait();
}
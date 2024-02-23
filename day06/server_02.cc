#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/WFHttpServer.h>
#include <workflow/HttpUtil.h>
static WFFacilities::WaitGroup waitGroup(1);
struct SeriesContext
{
    WFHttpTask *serverTask;
    std::string name;
    std::string key;
};
void sigHandler(int num)
{
    waitGroup.done();
    fprintf(stderr, "wait group is done\n");
}
void redisCallback(WFRedisTask *redisTask)
{
    protocol::RedisRequest *req = redisTask->get_req();
    protocol::RedisResponse *resp = redisTask->get_resp();
    int state = redisTask->get_state();
    int error = redisTask->get_error();
    protocol::RedisValue value; // value对象专门用来存储redis任务的结果
    switch (state)
    {
    case WFT_STATE_SYS_ERROR:
        fprintf(stderr, "system error: %s\n", strerror(error));
        break;
    case WFT_STATE_DNS_ERROR:
        fprintf(stderr, "dns error: %s\n", gai_strerror(error));
        break;
    case WFT_STATE_SUCCESS:
        resp->get_result(value);
        if (value.is_error())
        {
            fprintf(stderr, "redis error\n");
            state = WFT_STATE_TASK_ERROR;
        }
        break;
    }
    if (state != WFT_STATE_SUCCESS)
    {
        fprintf(stderr, "redis task Failed\n");
        return;
    }
    else
    {
        fprintf(stderr, "redis task Success!\n");
    }
    SeriesContext *context = static_cast<SeriesContext *>(series_of(redisTask)->get_context());
    std::string name = context->name;
    std::string key = context->key;
    WFHttpTask *serverTask = context->serverTask;
    if (value.is_nil())
    {
        auto resp2client = serverTask->get_resp();
        resp2client->add_header_pair("Content-Type", "text/plain");
        resp2client->append_output_body("you are not login yet!");
    }
    else
    {
        std::string storedKey = value.string_value();
        if (storedKey == key)
        {
            auto resp2client = serverTask->get_resp();
            resp2client->add_header_pair("Content-Type", "text/plain");
            resp2client->append_output_body("login check success!");
        }
        else
        {
            auto resp2client = serverTask->get_resp();
            resp2client->add_header_pair("Content-Type", "text/plain");
            resp2client->append_output_body("login check fail!");
        }
    }
    // delete context;
    fprintf(stderr, "redis callback is done\n");
}
void process(WFHttpTask *serverTask)
{
    auto resp = serverTask->get_resp();
    auto req = serverTask->get_req();
    // 解析客户端的请求
    std::string requestUri = req->get_request_uri();
    // /login?name=test&key=123
    // /login ?name=test&key=123
    std::string query = requestUri.substr(requestUri.find("?") + 1);
    // fprintf(stderr,"query = %s\n", query.c_str());
    std::string nameKV = query.substr(0, query.find("&"));
    std::string keyKV = query.substr(query.find("&") + 1);
    // fprintf(stderr,"nameKV = %s, keyKV = %s\n", nameKV.c_str(), keyKV.c_str());
    std::string name = nameKV.substr(nameKV.find("=") + 1);
    std::string key = keyKV.substr(keyKV.find("=") + 1);
    // fprintf(stderr,"name = %s, key = %s\n", name.c_str(), key.c_str());

    // 创建redis任务
    WFRedisTask *redisTask = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379", 0, redisCallback);
    auto redisReq = redisTask->get_req();
    redisReq->set_request("GET", {name});

    // 把redisTask加入到serverTask所在的序列
    series_of(serverTask)->push_back(redisTask);
    SeriesContext *context = new SeriesContext;
    context->serverTask = serverTask;
    context->name = name;
    context->key = key;
    series_of(serverTask)->set_context(context);

    serverTask->set_callback([context](WFHttpTask *serverTask)
                             {
        fprintf(stderr,"server callback is done\n");
        delete context; });
}
int main()
{
    signal(SIGINT, sigHandler);
    WFHttpServer server(process);
    if (server.start(1234) == 0)
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
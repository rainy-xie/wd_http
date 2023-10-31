#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/WFHttpServer.h>
#include <workflow/HttpUtil.h>
#include <wfrest/json.hpp>
using Json = nlohmann::json;
static WFFacilities::WaitGroup waitGroup(1);
void sigHandler(int num){
    waitGroup.done();
    fprintf(stderr,"wait group is done\n");
}

void process(WFHttpTask *serverTask){
    //解析url，分派任务
    auto req = serverTask->get_req();
    auto resp = serverTask->get_resp();
    std::string uri = req->get_request_uri();//路径+查询
    std::string path = uri.substr(0,uri.find("?"));
    std::string query = uri.substr(uri.find("?")+1);
    std::string method = req->get_method();

    if(method == "POST" && path == "/file/mupload/init"){
        //初始化
        // 1 读取请求报文，获取请求报文体
        const void *body;
        size_t size;
        req->get_parsed_body(&body,&size);
        // 2 将报文体解析成json对象
        Json fileInfo = Json::parse(static_cast<const char *>(body));
        std::string filename = fileInfo["filename"];
        std::string filehash = fileInfo["filehash"];
        int filesize = fileInfo["filesize"];
        // printf("filename = %s\n filehash = %s\n filesize = %d\n", filename.c_str(),filehash.c_str(),filesize);
        // 3 初始化分块信息 uploadID 分块
        // uploadID = username + time
        std::string uploadID = "liao";
        time_t now = time(nullptr);
        struct tm *ptm = localtime(&now);
        char timeStamp[20] = {0};
        snprintf(timeStamp,20,"%02d:%02d", ptm->tm_hour,ptm->tm_min);
        uploadID += timeStamp;
        //fprintf(stderr,"uploadID = %s\n", uploadID.c_str());
        // 生成分块的信息
        int chunkcount;
        int chunksize = 5*1024*1024;
        chunkcount = filesize/chunksize + (filesize%chunksize != 0);
        //fprintf(stderr,"chunkcount = %d, chunksize = %d\n", chunkcount,chunksize);
        // 4 生成对客户端的响应
        Json uppartInfo;
        uppartInfo["status"] = "OK";
        uppartInfo["uploadID"] = uploadID;
        uppartInfo["chunkcount"] = chunkcount;
        uppartInfo["filesize"] = filesize;
        uppartInfo["chunksize"] = chunksize;
        uppartInfo["filehash"] = filehash;
        // 5 将一些信息写入缓存
        std::vector<std::vector<std::string>> argsVec = {
            {"MP_"+uploadID, "chunkcount", std::to_string(chunkcount)},
            {"MP_"+uploadID, "filehash", filehash},
            {"MP_"+uploadID, "filesize", std::to_string(filesize)}
        };
        for(int i = 0; i < 3; ++i){
            auto redisTask = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379",2,nullptr);
            redisTask->get_req()->set_request("HSET", argsVec[i]);
            redisTask->start();
        }
        resp->append_output_body(uppartInfo.dump());
        
    }
    else if(method == "POST" && path == "/file/mupload/uppart"){
        //上传单个分块
        // 1 解析用户请求 提取出 uploadID和chkidx
        // uploadID=MP_liao15:33&chkidx=1
        std::string uploadIDKV = query.substr(0,query.find("&"));
        std::string chkidxKV = query.substr(query.find("&") + 1);
        std::string uploadID = uploadIDKV.substr(uploadIDKV.find("=")+1);
        std::string chkidx = chkidxKV.substr(chkidxKV.find("=")+1);
        // 2 获取文件的hash，创建目录，写入分块
        // HGET uploadID filehash
        auto redisTaskHGET = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379",2,[chkidx,req](WFRedisTask *redisTask){
            protocol::RedisRequest *redisReq = redisTask->get_req();
            protocol::RedisResponse *redisResp = redisTask->get_resp();
            protocol::RedisValue value;
            redisResp->get_result(value);
            //创建空文件
            std::string filehash = value.string_value();
            mkdir(filehash.c_str(),0777);
            std::string filepath = filehash + "/" + chkidx;
            int fd = open(filepath.c_str(),O_RDWR|O_CREAT,0666);
            //将文件内容进行写入
            const void *body;
            size_t size;
            req->get_parsed_body(&body,&size);
            fprintf(stderr,"body = %s\nsize = %lu\n", (char *)body,size);
            write(fd,body,size);
            close(fd);
        });
        redisTaskHGET->get_req()->set_request("HGET",{uploadID,"filehash"});
        series_of(serverTask)->push_back(redisTaskHGET);
        // 3 写入分块完成之后，将上传的进度存入缓存中
        auto redisTaskHSET = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379",2,nullptr);
        redisTaskHSET->get_req()->set_request("HSET",{uploadID,"chkidx_"+chkidx,"1"});
        series_of(serverTask)->push_back(redisTaskHSET);
        // 4 回复响应
        resp->append_output_body("OK");
    }
    else if(method == "GET" && path == "/file/mupload/complete"){
        //合并分块
        // 1 解析用户请求 提取出uploadID
        std::string uploadID = query.substr(query.find("=")+1);
        
        // 2 根据upload查询进度 HGETALL uploadID
        auto redisTask = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379",2,[resp](WFRedisTask *redisTask){
            protocol::RedisRequest *redisReq = redisTask->get_req();
            protocol::RedisResponse *redisResp = redisTask->get_resp();
            protocol::RedisValue value;
            redisResp->get_result(value);
            // 3 chunkcount chkidx_*
            int chunkcount;
            int chunknow = 0;
            for(int i = 0; i < value.arr_size(); i+=2){
                std::string key = value.arr_at(i).string_value();
                std::string val = value.arr_at(i+1).string_value();
                if(key == "chunkcount"){
                    chunkcount = std::stoi(val);
                }
                else if(key.substr(0,7) == "chkidx_"){
                    ++chunknow;
                }
            }
            fprintf(stderr,"chunkcount = %d\nchunknow = %d\n",chunkcount,chunknow);
            // 4 比较大小
            if(chunknow == chunkcount){
                resp->append_output_body("SUCCESS");
            }
            else{
                resp->append_output_body("FAIL");
            }
        });
        redisTask->get_req()->set_request("HGETALL",{uploadID});
        series_of(serverTask)->push_back(redisTask);
        
    } 
}
int main(){
    signal(SIGINT,sigHandler);
    WFHttpServer server(process);
    if(server.start(1234) == 0){
        waitGroup.wait();
        server.stop();
    }
    else{
        perror("server start failed\n");
        return -1;
    }
    return 0;   
}
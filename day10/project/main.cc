#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/HttpServer.h>
#include <wfrest/json.hpp>
#include "FileUtil.h"
static WFFacilities::WaitGroup waitGroup(1);
using Json = nlohmann::json;
void sigHandler(int num){
    waitGroup.done();
    fprintf(stderr,"wait group is done\n");
}

int main(){
    signal(SIGINT,sigHandler);
    wfrest::HttpServer server;
    server.GET("/file/upload",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        resp->File("static/view/index.html");
    });
    server.POST("/file/upload",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        //  读取文件内容 解析form-data类型的请求报文
        using Form = std::map<std::string, std::pair<std::string, std::string>>;
        Form &form = req->form();
        std::pair<std::string, std::string> fileInfo = form["file"];
        // fileInfo.first fileInfo.second
        std::string filepath = "tmp/" + fileInfo.first;
        int fd = open(filepath.c_str(),O_RDWR|O_CREAT,0666);
        if(fd < 0){
            resp->set_status_code("500");
            return;
        }
        // try {} catch () 
        // 方案 1 write
        int ret = write(fd,fileInfo.second.c_str(),fileInfo.second.size());

        close(fd);
        
        /* 方案 2 pwrite的任务
        auto pwriteTask = WFTaskFactory::create_pwrite_task(fd,fileInfo.second.c_str(),fileInfo.second.size(),0,callback);
        //把上传文件之后的逻辑写到callback当中
        series->push_back(pwriteTask)
        */

        std::string sql = "INSERT INTO cloudisk.tbl_file (file_sha1,file_name,file_size,file_addr,status) VALUES('" 
                   + FileUtil::sha1File(filepath.c_str()) + "','"
                   + fileInfo.first + "'," 
                   + std::to_string(fileInfo.second.size()) + ",'"
                   + filepath + "', 0);";
        resp->MySQL("mysql://root:123@localhost",sql,[](Json *pjson){
            fprintf(stderr,"out = %s\n", pjson->dump().c_str());
        });
        //fprintf(stderr,"sql = %s\n", sql.c_str());
        resp->set_status_code("302");
        resp->headers["Location"] = "/file/upload/success";
    });
    server.GET("/file/upload/success",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        resp->String("Upload success");
    });
    server.GET("/file/download",[](const wfrest::HttpReq *req, wfrest::HttpResp *resp){
        // // /file/download?filehash=aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d&filename=1.txt&filesize=5
        auto fileInfo = req->query_list();
        std::string filesha1 = fileInfo["filehash"];
        std::string filename = fileInfo["filename"];
        int filesize = std::stoi(fileInfo["filesize"]);
        std::string filepath = "tmp/"+filename;

        // int fd = open(filepath.c_str(),O_RDONLY);
        // int size = lseek(fd,0,SEEK_END);
        // lseek(fd,0,SEEK_SET);
        // std::unique_ptr<char []> buf(new char[size]);
        // read(fd,buf.get(),size);

        // resp->append_output_body(buf.get(),size);
        // resp->headers["Content-Type"] = "application/octect-stream";
        // resp->headers["content-disposition"] = "attachment;filename="+filename;
        resp->set_status_code("302");
        resp->headers["Location"] = "http://192.168.135.129:1235/"+filename;
    });
    if(server.track().start(1234) == 0){
        server.list_routes();
        waitGroup.wait();
        server.stop();
    }
    else{
        fprintf(stderr,"can not start server!\n");
        return -1;
    }
    
}
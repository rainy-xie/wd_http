#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
#include <wfrest/HttpServer.h>
#include <wfrest/json.hpp>
#include "User.h"
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
#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <wfrest/HttpServer.h>
#include <wfrest/json.hpp>
static WFFacilities::WaitGroup waitGroup(1);
using Json = nlohmann::json;

void callback(WFTimerTask *timeTask)
{
}
void sigHandler(int num)
{
    waitGroup.done();
    fprintf(stderr, "wait group is done\n");
}
int main()
{
    signal(SIGINT, sigHandler);
    wfrest::HttpServer server;
    server.GET("/test", [](const wfrest::HttpReq *req, wfrest::HttpResp *resp)
               { resp->String("Hello"); });
    server.GET("/redirect", [](const wfrest::HttpReq *req, wfrest::HttpResp *resp)
               {
                    resp->set_status_code("302");
                    resp->headers["Location"] = "/test"; });
    server.GET("/test1", [](const wfrest::HttpReq *req, wfrest::HttpResp *resp)
               {
                   std::map<std::string, std::string> queryMap = req->query_list();
                   for(auto it:queryMap)
                   {
                       fprintf(stderr, "first = %s, second = %s\n", it.first.c_str(), it.second.c_str());
                   } });

    server.GET("/post1.html", [](const wfrest::HttpReq *req, wfrest::HttpResp *resp)
               {
                   int fd = open("post1.html", O_RDONLY);
                   std::unique_ptr<char []> buf(new char[6943]);
                   read(fd,buf.get(),6943);
                   resp->append_output_body(buf.get(),6943);
                   resp->headers["Content-Type"] = "text/html"; });

    server.POST("/test", [](const wfrest::HttpReq *req, wfrest::HttpResp *resp)
                { resp->String("Hello"); });

    server.POST("/login", [](const wfrest::HttpReq *req, wfrest::HttpResp *resp)
                {
                    if (req->content_type() != wfrest::APPLICATION_URLENCODED)
                    {
                        resp->set_status_code("500");
                        return;
                    }
                    std::map<std::string, std::string> queryMap = req->query_list();
                    for (auto it : queryMap)
                    {
                        fprintf(stderr, "first = %s, second = %s\n", it.first.c_str(), it.second.c_str());
                    }
                    // const std::string &username = req->query("username");
                    // const std::string &password = req->query("password");
                    // fprintf(stderr,"username = %s, password = %s\n",username.c_str(),password.c_str());
                });
    server.POST("/post1.html", [](const wfrest::HttpReq *req, wfrest::HttpResp *resp)
                {
                    if (req->content_type() != wfrest::MULTIPART_FORM_DATA)
                    {
                        resp->set_status_code("500");
                        return;
                    }
                    using Form = std::map<std::string, std::pair<std::string, std::string>>;
                    const Form &form = req->form();
                    for (auto it : form)
                    {
                        fprintf(stderr, "it.first = %s, it.second.first = %s, it.second.second = %s\n", it.first.c_str(), it.second.first.c_str(), it.second.second.c_str());
                    } });
    server.POST("/formdata", [](const wfrest::HttpReq *req, wfrest::HttpResp *resp)
                {
                    if (req->content_type() != wfrest::MULTIPART_FORM_DATA)
                    {
                        resp->set_status_code("500");
                        return;
                    }
                    using Form = std::map<std::string, std::pair<std::string, std::string>>;
                    const Form &form = req->form();
                    for (auto it : form)
                    {
                        fprintf(stderr, "it.first = %s, it.second.first = %s, it.second.second = %s\n", it.first.c_str(), it.second.first.c_str(), it.second.second.c_str());
                    } });
    if (server.track().start(1234) == 0)
    {
        server.list_routes();
        waitGroup.wait();
        server.stop();
    }
    else
    {
        fprintf(stderr, "can not start server!\n");
    }
}
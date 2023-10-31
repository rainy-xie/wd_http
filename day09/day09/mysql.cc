#include "linuxheader.h"
#include <workflow/WFFacilities.h>
#include <workflow/MySQLUtil.h>
#include <workflow/MySQLResult.h>
#include <workflow/MySQLMessage.h>
static WFFacilities::WaitGroup waitGroup(1);
void callback(WFMySQLTask *mysqlTask)
{
    // 检查连接错误
    if (mysqlTask->get_state() != WFT_STATE_SUCCESS)
    {
        fprintf(stderr, "error msg:%s\n", WFGlobal::get_error_string(mysqlTask->get_state(), mysqlTask->get_error()));
        return;
    }

    protocol::MySQLResponse *resp = mysqlTask->get_resp();
    protocol::MySQLResultCursor cursor(resp);

    // 检查语法错误
    if (resp->get_packet_type() == MYSQL_PACKET_ERROR)
    {
        fprintf(stderr, "error_code = %d msg = %s\n", resp->get_error_code(), resp->get_error_msg().c_str());
    }

    do
    {
        if (cursor.get_cursor_status() == MYSQL_STATUS_OK)
        {
            // 写指令，执行成功
            fprintf(stderr, "OK. %llu rows affected. %d warnings. insert_id = %llu.\n",
                    cursor.get_affected_rows(), cursor.get_warnings(), cursor.get_insert_id());
        }
        else if (cursor.get_cursor_status() == MYSQL_STATUS_GET_RESULT)
        {
            // 读指令，执行成功
            // 把所有域信息构成一个数组
            const protocol::MySQLField *const *fields = cursor.fetch_fields();
            for (int i = 0; i < cursor.get_field_count(); ++i)
            {
                // db table name type
                fprintf(stderr, "db = %s, table = %s, name = %s, type = %s\n", fields[i]->get_db().c_str(),
                        fields[i]->get_table().c_str(),
                        fields[i]->get_name().c_str(),
                        datatype2str(fields[i]->get_data_type()));
            }
            std::vector<std::vector<protocol::MySQLCell>> rows;
            cursor.fetch_all(rows);
            for (auto &row : rows)
            {
                for (auto &cell : row)
                {
                    if (cell.is_int())
                    {
                        printf("[%d]", cell.as_int());
                    }
                    else if (cell.is_ulonglong())
                    {
                        printf("[%llu]", cell.as_ulonglong());
                    }
                    else if (cell.is_string())
                    {
                        printf("[%s]", cell.as_string().c_str());
                    }
                    else if (cell.is_datetime())
                    {
                        printf("[%s]", cell.as_datetime().c_str());
                    }
                }
                printf("\n");
            }
            // cursor.fetch_row()
        }
    } while (cursor.next_result_set());
}
void sigHandler(int num)
{
    waitGroup.done();
    fprintf(stderr, "wait group is done\n");
}
int main()
{
    signal(SIGINT, sigHandler);
    auto mysqlTask = WFTaskFactory::create_mysql_task("mysql://root:123@127.0.0.1:3306", 0, callback);
    std::string sql = "insert into cloudisk.tbl_user_token (user_name,user_token) values ('test7','abc');"
                      "select * from cloudisk.tbl_user_token;";
    // std::string sql = "show databases;";
    // std::string sql = "select * from cloudisk.tbl_file;";
    auto req = mysqlTask->get_req();
    req->set_query(sql);
    mysqlTask->start();
    waitGroup.wait();
}
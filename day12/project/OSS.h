#include <alibabacloud/oss/OssClient.h>
struct OSSInfo{
    std::string Bucket = "bucket-liaozs-test1";
    std::string Endpoint = "oss-cn-hangzhou.aliyuncs.com";
    std::string AccessKeyId = "LTAI5tRSzFpdVZBFc9dZtk31";
    std::string AccessKeySecret = "uAleQLPrGlHjTY5MwWhLrgDyPVYbZ6"; 
};
enum {
    FS,
    OSS
};
struct Config{
    int storeType = OSS;
    int isAsyncTransferEnable = true;
};



#include "rabbitmq.h"
#include <SimpleAmqpClient/SimpleAmqpClient.h>
int main(){
    // 指定mq的一些信息
    RabbitMqInfo MqInfo;
    // 创建一条和mq的连接
    AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create();
    // 从mq中提取消息
    channel->BasicConsume(MqInfo.TransQueueName,MqInfo.TransQueueName);

    AmqpClient::Envelope::ptr_t envelope;
    bool isNotTimeout = channel->BasicConsumeMessage(envelope,5000);
    if(isNotTimeout == false){
        fprintf(stderr,"timeout\n");
        return -1;
    }

    fprintf(stderr,"message = %s\n", envelope->Message()->Body().c_str());
    return 0;
}

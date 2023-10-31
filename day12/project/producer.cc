#include "rabbitmq.h"
#include <SimpleAmqpClient/SimpleAmqpClient.h>
int main(){
    // 指定mq的一些信息
    RabbitMqInfo MqInfo;
    // 创建一条和mq的连接
    AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create();
    // pause();
    // 创建消息
    AmqpClient::BasicMessage::ptr_t message = AmqpClient::BasicMessage::Create("Hello");
    // 发布消息
    channel->BasicPublish(MqInfo.TransExchangeName,MqInfo.TransRoutingKey,message);
}

all:server producer consumer
server:main.o
	g++ $^ -o $@ -lSimpleAmqpClient -lwfrest -lworkflow -lssl -lcrypt -lcrypto -lalibabacloud-oss-cpp-sdk -lcurl -lpthread
main.o:main.cc
	g++ -c $^ -o $@  -std=c++11 -fno-rtti -g
producer:producer.o
	g++ $^ -o $@ -lSimpleAmqpClient
producer.o:producer.cc
	g++ -c $^ -o $@
consumer:consumer.o
	g++ $^ -o $@ -lSimpleAmqpClient
consumer.o:consumer.cc
	g++ -c $^ -o $@
clean:
	$(RM) server main.o

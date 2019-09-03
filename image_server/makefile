image_server:image_server.cc
	g++ $^ -o $@ -std=c++11 -lpthread $ -L /usr/lib64/mysql/ -lmysqlclient -ljsoncpp -lcrypto -g
test_api:test_api.cc
	g++ $^ -o $@ -L /usr/lib64/mysql/ -lmysqlclient -ljsoncpp -g

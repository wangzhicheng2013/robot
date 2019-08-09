#ifndef SRC_MESSAGE_COMMUNICATE_ENTITY_HPP_
#define SRC_MESSAGE_COMMUNICATE_ENTITY_HPP_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <algorithm>
using namespace std;
class message_communicate_entity {
public:
	message_communicate_entity() = default;
	virtual ~message_communicate_entity() = default;
public:
	virtual bool init() = 0;
	virtual int send(const char *buf, int len) = 0;
	virtual int recv(char *recv_buf, int buf_len) = 0;
};

#endif /* SRC_MESSAGE_COMMUNICATE_ENTITY_HPP_ */

#ifndef SRC_HTTP_CONFIG_HPP_
#define SRC_HTTP_CONFIG_HPP_
#include "http_key.hpp"
class http_config {
public:
	http_config() {
		url_port = 0;
		response_code = 200;
		rest_type_ = UNKNOWN_REST_TYPE;
		url_type_ = UNKNOWN_ZEG_ROBOT_TYPE;
	}
	virtual ~http_config() = default;
public:
	rest_type rest_type_;
	url_type url_type_;
	int url_port;
	long response_code;
	string url;
	string url_ip;
	string url_path;
	string request_body;
	string response_body;
};

#endif /* SRC_HTTP_CONFIG_HPP_ */

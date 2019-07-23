#ifndef SRC_HTTP_UTILITY_HPP_
#define SRC_HTTP_UTILITY_HPP_
#include <map>
#include "event.h"
#include "evhttp.h"
#include "event2/listener.h"
#include "response_handler.hpp"
class http_utility {
public:
	static inline http_utility &get() {
		static http_utility obj;
		return obj;
	}
	inline void set_http_utility(struct evhttp_request *req) {
		http_req_ = req;
		if (nullptr == http_req_) {
			return;
		}
		get_rest_type();
		get_url();
		get_url_type();
		get_request_body();
	}
	inline void registe_handler(const http_key &key, response_handler *handler) {
		mapper_.insert(make_pair(key, handler));
	}
	void get_rest_type() {
		switch (evhttp_request_get_command(http_req_)) {
		case EVHTTP_REQ_GET:
			config_.rest_type_ =  GET;
			break;
		case EVHTTP_REQ_POST:
			config_.rest_type_ =  POST;
			break;
		case EVHTTP_REQ_PUT:
			config_.rest_type_ =  PUT;
			break;
		case EVHTTP_REQ_DELETE:
			config_.rest_type_ =  DELETE;
			break;
		default:
			config_.rest_type_ = UNKNOWN_REST_TYPE;
		}
	}
	void get_url() {
		const char *p = evhttp_request_uri(http_req_);
		if (nullptr == p) {
			return;
		}
		config_.url = p;
		struct evhttp_uri *decoded = evhttp_uri_parse(p);
		if (nullptr == decoded) {
			return;
		}
		const char *path = evhttp_uri_get_path(decoded);
		if (nullptr == path || 0 == path[0]) {
			config_.url_path = "/";
		}
		else {
			config_.url_path = path;
		}
		evhttp_uri_free(decoded);
	}
	inline void get_url_type() {
		config_.url_type_ = url_mapper::get().get_type(config_.url);
	}
	inline void get_request_body() {
		const char *body = (const char *)EVBUFFER_DATA(http_req_->input_buffer);
		if (body != nullptr) {
			config_.request_body = body;
		}
	}
	void handle_request() {
		http_key httpKey(config_.rest_type_, config_.url_type_);
		auto it = mapper_.find(httpKey);
		if (end(mapper_) == it) {
			return;
		}
		it->second->handle_request(config_);
	}
	void reply_client() {
		struct evbuffer *buf = evbuffer_new();
		evhttp_add_header(http_req_->output_headers, "Content-Type", "application/json;charset=UTF-8");
		evbuffer_add_printf(buf, "%s", config_.response_body.c_str());
		evhttp_send_reply(http_req_, config_.response_code, "return", buf);
		evbuffer_free(buf);
	}
	void handle_http() {
		handle_request();
		reply_client();
	}
private:
	http_utility() {
		http_req_ = nullptr;
	}
	virtual ~http_utility() {
		http_req_ = nullptr;
	}
private:
	struct evhttp_request *http_req_;
	http_config config_;
	map<http_key, response_handler *, http_key_cmp>mapper_;
};

#endif /* SRC_HTTP_UTILITY_HPP_ */

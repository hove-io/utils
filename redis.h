#pragma once
#include <hiredis/hiredis.h>
#include "utils/exception.h"
#include "functions.h"

class Redis{
protected:
    std::string host;
    std::string password;
    int port;
    int db;
    std::string prefix;
    std::string separator;    

    redisContext* connect;
    struct timeval timeout;
public:
    Redis(const std::string& redis_connection);    
    Redis(const std::string& _host, const std::string& _password,
          int _port = 6379, int _db = 0, const std::string& _prefix = "REDIS",
          const std::string& _separator = "|", int _timeout = 2):
        host(_host), password(_password), port(_port), db(_db),
        prefix(_prefix), separator(_separator){
        timeout = {_timeout, 0};};
    ~Redis();
    void redis_connect();
    void redis_connect_with_timeout();
    std::string get(const std::string& key);
    void set(const std::string& key, const std::string& value);
    void set_prefix(const std::string& _prefix);
};

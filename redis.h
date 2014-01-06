#pragma once
#include <set>
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
    void key_value_command(const std::string& command, const std::string& key, const std::string& value);
    void key_field_value_command(const std::string& command, const std::string& key, const std::string field,
                                        const std::string& value);
    template<typename T>
    T simple_key_command(const std::string& command, const std::string& key);
    void void_key_command(const std::string& command, const std::string& key);
    void exec_command(const std::string& command);
    void manage_reply(redisReply* redis_reply, const std::string &command);

private:
    bool pipeline_started = false;
    size_t num_commands_pipeline = 0;

public:
    Redis(const std::string& redis_connection, const std::string& prefix="prefix",
            const std::string& separator="|");    
    Redis(const std::string& _host, const std::string& _password,
          int _port = 6379, int _db = 0, const std::string& _prefix = "REDIS",
          const std::string& _separator = "|", int _timeout = 2):
        host(_host), password(_password), port(_port), db(_db),
        prefix(_prefix), separator(_separator){
        timeout = {_timeout, 0};};
    ~Redis();
    //CONNECTIONS FUNCTIONS
    //On failure: throws navitia::exception
    void redis_connect();
    //On failure: throws navitia::exception
    void redis_connect_with_timeout();

    //HELPERS FUNCTIONS
    void set_prefix(const std::string& _prefix);
    std::string get_separator() { return separator;}
    std::string get_prefix() { return prefix;}
    
    //KEYS FUNCTIONS
    std::string get(const std::string& key);
    //On failure: throws navitia::exception
    void set(const std::string& key, const std::string& value);
    //On failure: throws navitia::exception
    void del(const std::string& key);
    std::vector<std::string> keys(const std::string& match);

    //SET FUNCTIONS
    //On failure: throws navitia::exception
    void srem(const std::string& key, const std::string& value);
    size_t scard(const std::string& key);
    //On failure: throws navitia::exception
    void sadd(const std::string& key, const std::string& value);


    //HASH FUNCTIONS
    //On failure: throws navitia::exception
    void hset(const std::string& key, const std::string& field, const std::string& value);
    size_t hlen(const std::string& key);
    //On failure: throws navitia::exception
    void hdel(const std::string& key, const std::string& field);

    void start_pipeline() { pipeline_started = true; num_commands_pipeline = 0; }
    void end_pipeline();
};

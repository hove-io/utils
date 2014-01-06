#include "redis.h"
#include <algorithm>

Redis::Redis(const std::string& redis_connection, const std::string &prefix,
        const std::string& separator) : prefix(prefix), separator(separator){
    std::vector<std::string> params = split_string(redis_connection, " ");
    std::map<std::string, std::string> key_value;

    for(std::string param : params){
        auto split = split_string(param, "=");
        if (split.size() > 0){
            key_value[split[0]] = split[1];
        }
    }

    this->host = value_by_key(key_value, "host");
    this->password = value_by_key(key_value, "password");
    this->port = str_to_int(value_by_key(key_value, "port"));
    this->db = str_to_int(value_by_key(key_value, "db"));
    this->timeout = {str_to_int(value_by_key(key_value, "timeout")),0};
}


void Redis::redis_connect(){
    this->connect = redisConnect(this->host.c_str(),this->port);
    if (this->connect == nullptr){
        throw navitia::exception("redis_connect : Connecting redis failed");
    }
    if(this->connect != nullptr && this->connect->err){
        throw navitia::exception("redis_connect : Connecting redis failed, error : "
                + std::string(this->connect->errstr));
    }
}


void Redis::redis_connect_with_timeout(){
    this->connect = redisConnectWithTimeout(this->host.c_str(),this->port, this->timeout);
    if (this->connect == nullptr){
        throw navitia::exception("redis_connect_with_timeout : Connecting redis failed");
    }
    if(this->connect != nullptr && this->connect->err){
        throw navitia::exception("redis_connect : Connecting redis failed, error : "
                + std::string(this->connect->errstr));
    }
    redisSetTimeout( this->connect, this->timeout);
}


Redis::~Redis(){
    if (this->connect != nullptr){
        redisFree(this->connect);
    }
}


template<typename T>
T Redis::simple_key_command(const std::string& command, const std::string& key) {
    T to_return = {};
    redisReply *redis_reply = (redisReply*) redisCommand(
            this->connect,"%s %s%s%s", command.c_str(), this->prefix.c_str(),
            this->separator.c_str(), key.c_str());
    if ((redis_reply != nullptr) && (redis_reply->type != REDIS_REPLY_NIL)){
        to_return = T(redis_reply->str);
        freeReplyObject(redis_reply);
    }
    return to_return;
}


void Redis::manage_reply(redisReply* redis_reply, const std::string& command) {
    if((redis_reply == nullptr) ||
            ((redis_reply != nullptr) && (redis_reply->type == REDIS_REPLY_ERROR))) {
        std::string error = "";
        if (redis_reply != nullptr) {
            error = " error : " + std::string(redis_reply->str, redis_reply->len);
            freeReplyObject(redis_reply);
        }
        throw navitia::exception("Unable to < " + command +" > with prefix : " + this->prefix + error);
    }else{
        if ((redis_reply->type == REDIS_REPLY_STATUS) && (std::string(redis_reply->str, redis_reply->len) != "OK")){
            auto error = " error : " + std::string(redis_reply->str, redis_reply->len);
            freeReplyObject(redis_reply);
            throw navitia::exception("Unable to < " + command +" > with prefix : " + this->prefix + error);
        }
        if(redis_reply != nullptr) {
            freeReplyObject(redis_reply);
        }
    }
}


void Redis::exec_command(const std::string& command) {
    if(!pipeline_started) {
        redisReply *redis_reply = (redisReply*) redisCommand(this->connect, command.c_str());
        this->manage_reply(redis_reply, command);
    } else {
        redisAppendCommand(this->connect, command.c_str());
        ++ this->num_commands_pipeline;
    }
}


void Redis::end_pipeline() {
    redisReply *redis_reply;
    for(size_t i = 0; i < this->num_commands_pipeline; ++i) {
        redisGetReply(this->connect, (void**) &redis_reply);
        this->manage_reply(redis_reply, "");
    }
    this->pipeline_started = false;
    this->num_commands_pipeline = 0;
}


void Redis::void_key_command(const std::string& command, const std::string& key) {
    this->exec_command(command + " " + this->prefix + this->separator + key);
}


void Redis::key_value_command(const std::string& command, const std::string& key, const std::string& value) {
    this->exec_command(command + " " + this->prefix +  this->separator + key + " " + value);
}

void Redis::key_field_value_command(const std::string& command, const std::string& key, const std::string field,
                                    const std::string& value) {
    this->exec_command(command + " " + this->prefix +  this->separator + key + " " + field + " " + value);
}


std::string Redis::get(const std::string& key) {
    return this->simple_key_command<std::string>("get", key);
}


void Redis::set(const std::string& key, const std::string& value){
    this->key_value_command("set", key, value);
}


void Redis::del(const std::string &key) {
    this->void_key_command("del", key);
}


std::vector<std::string> Redis::keys(const std::string& match) {
    std::vector<std::string> to_return;
    redisReply *redis_reply = (redisReply*) redisCommand(
            this->connect,"KEYS %s%s%s", this->prefix.c_str(), this->separator.c_str(), match.c_str());
    if ((redis_reply != nullptr) && (redis_reply->type != REDIS_REPLY_NIL)){
        for(size_t i=0; i < redis_reply->elements; ++i) {
            to_return.push_back(redis_reply->element[i]->str);
        }
        freeReplyObject(redis_reply);
    }
    std::sort(to_return.begin(), to_return.end());
    return to_return;
}

void Redis::sadd(const std::string& key, const std::string& value) {
    this->key_value_command("sadd", key, value);
}

void Redis::srem(const std::string& key, const std::string& value) {
    this->key_value_command("srem", key, value);
}


size_t Redis::scard(const std::string& key) {
    return this->simple_key_command<size_t>("scard", key);
}


size_t Redis::hlen(const std::string& key) {
    return this->simple_key_command<size_t>("hlen", key);
}


void Redis::hdel(const std::string& key, const std::string& field) {
    this->void_key_command("hdel", key + " " + field);
}


void Redis::hset(const std::string& key, const std::string& field, const std::string& value) {
    this->key_field_value_command("hset", key, field, value);
}


void Redis::set_prefix(const std::string& _prefix){
    prefix = _prefix;
}

#include "redis.h"

Redis::Redis(const std::string& redis_connection){
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

    this->separator = "|";
    this->prefix = "REDIS";
}

void Redis::redis_connect(){
    this->connect = redisConnect(this->host.c_str(),this->port);
    if (this->connect == nullptr){
        throw navitia::exception("redis_connect : Connecting redis failed");
    }
    if(this->connect != nullptr && this->connect->err){
        throw navitia::exception("redis_connect : Connecting redis failed, error : "+std::string(this->connect->errstr));
    }
}

void Redis::redis_connect_with_timeout(){
    this->connect = redisConnectWithTimeout(this->host.c_str(),this->port, this->timeout);
    if (this->connect == nullptr){
        throw navitia::exception("redis_connect_with_timeout : Connecting redis failed");
    }
    if(this->connect != nullptr && this->connect->err){
        throw navitia::exception("redis_connect_with_timeout : Connecting redis failed, error : "+std::string(this->connect->errstr));
    }
    redisSetTimeout( this->connect, this->timeout);
}

Redis::~Redis(){
    if (this->connect != nullptr){
        redisFree(this->connect);
    }
}

std::string Redis::get(const std::string& key){
    std::string to_return;
    redisReply *redis_reply;
    redis_reply = (redisReply*) redisCommand(this->connect,"GET %s%s%s", this->prefix.c_str(), this->separator.c_str(), key.c_str());
    if ((redis_reply != nullptr) && (redis_reply->type != REDIS_REPLY_NIL)){
        to_return = std::string(redis_reply->str);
        freeReplyObject(redis_reply);
    }
    return to_return;
}

void Redis::set(const std::string& key, const std::string& value){
    redisReply *redis_reply;
    redis_reply = (redisReply*) redisCommand(this->connect,"SET %s%s%s %s", this->prefix.c_str(), this->separator.c_str(), key.c_str(), value.c_str());
    if((redis_reply == nullptr) || ((redis_reply != nullptr) && (redis_reply->type == REDIS_REPLY_ERROR))) {
        if (redis_reply != nullptr){
            freeReplyObject(redis_reply);
        }
        throw navitia::exception("set : impossible d'ajouter la correspondance <"+key+","+ value+"> avec le prefix : "+this->prefix);
    }else{
        if ((redis_reply->type != REDIS_REPLY_STATUS) || (std::string(redis_reply->str) != "OK")){
            freeReplyObject(redis_reply);
            throw navitia::exception("set : impossible d'ajouter la correspondance <"+key+","+ value+"> avec le prefix : "+this->prefix);
        }        
    }
}

void Redis::set_prefix(const std::string& _prefix){
    prefix = _prefix;
}

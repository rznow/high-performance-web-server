#include "redis/RedisValue.h"
#include <stdexcept>

RedisValue::RedisValue(redisReply* reply)
{
    switch(reply->type)
    {
        case REDIS_REPLY_STRING:
            type_ = Type::String;
            value_ = std::string(reply->str, reply->len);
            break;

        case REDIS_REPLY_STATUS:
            type_ = Type::Status;
            value_ = std::string(reply->str, reply->len);
            break;

        case REDIS_REPLY_INTEGER:
            type_ = Type::Integer;
            value_ = reply->integer;
            break;

        case REDIS_REPLY_ARRAY:
        {
            type_ = Type::Array;

            std::vector<RedisValue> arr;

            arr.reserve(reply->elements);

            for(size_t i=0;i<reply->elements;i++)
            {
                arr.emplace_back(reply->element[i]);
            }

            value_ = std::move(arr);

            break;
        }

        case REDIS_REPLY_NIL:
            type_ = Type::Nil;
            value_ = std::monostate{};
            break;
        
        case REDIS_REPLY_ERROR:
            type_ = Type::Error;
            value_ = std::string(reply->str, reply->len);
            break;

        default:
            throw std::runtime_error("Unsupported redis reply type");
    }
}

bool RedisValue::isNull() const     {return type_ == Type::Nil;}

bool RedisValue::isString() const   {return type_ == Type::String;}

bool RedisValue::isInteger() const  {return type_ == Type::Integer;}

bool RedisValue::isArray() const    {return type_ == Type::Array;}

std::string RedisValue::asString() const    {return std::get<std::string>(value_);}

int RedisValue::asInt() const               {return static_cast<int>(std::get<long long>(value_));}

long long RedisValue::asLong() const        {return std::get<long long>(value_);}

const std::vector<RedisValue>& RedisValue::asArray() const  {return std::get<std::vector<RedisValue>>(value_);}

const RedisValue& RedisValue::operator[](size_t i) const    {return asArray()[i];}
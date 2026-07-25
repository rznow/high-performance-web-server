#pragma once
#include <hiredis/hiredis.h>
#include <vector>
#include <variant>
#include <string>

class RedisValue
{
public:

    enum class Type
    {
        Nil,
        String,
        Integer,
        Array,
        Error,
        Status
    };

    RedisValue(redisReply* reply);

    ~RedisValue() = default;

    bool isNull() const;

    bool isString() const;

    bool isInteger() const;

    bool isArray() const;

    std::string asString() const;

    int asInt() const;

    long long asLong() const;

    const std::vector<RedisValue>& asArray() const;

    const RedisValue& operator[](size_t i) const;

private:

    Type type_;

    std::variant<
        std::monostate,
        std::string,
        long long,
        std::vector<RedisValue>
    > value_;
};
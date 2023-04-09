#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

template <class T>
class Singleton {
public:
    static constexpr T& instance()    {
        return instance_;
    }
    Singleton() = delete;
    Singleton(const Singleton<T>&) = delete;
    const Singleton<T> & operator=(const Singleton<T>&) = delete;
private:
    inline static T instance_;
};


class MongoConnector //: public Singleton<MongoConnector>
{
public:
    MongoConnector() = default;

    void setConfig(uint16_t port);
private:
    mongocxx::instance instance_;
    mongocxx::client client_;
};
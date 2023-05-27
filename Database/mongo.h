#pragma once

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/options/find.hpp>

#include <spdlog/spdlog.h>

#include "crud_interface.h"

#include "../Serialization/bson.h"

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

namespace database {
class MongoConnector : public CRUD<MongoConnector> {
 public:
  MongoConnector(std::string_view address, uint16_t port);

 public:
  template <class T>
  void createImpl(const T& t) {
    auto result =
        collection_.insert_one(serialization::convert_to_bson_document(t));
    if (!result) {
      spdlog::error("Database insertion failed");
    }
  }

  template <class... Args>
  void createImpl(Args&&... args) {
    std::vector<bsoncxx::document::value> documents;
    documents.reserve(sizeof...(args));

    (documents.push_back(
         serialization::convert_to_bson_document(std::forward<Args>(args))),
     ...);

    auto result = collection_.insert_many(documents);

    if (!result) {
      spdlog::error("Database insertion failed");
    }
  }

 private:
  void set_config(std::string_view address, uint16_t port);

 private:
  mongocxx::instance instance_;
  mongocxx::client client_;
  mongocxx::collection collection_;
};
}  // namespace database

#pragma once

#include "crud_interface.h"

#include "../Serialization/bson.h"

#include "workflow/WFMySQLConnection.h"

namespace db {
class MYSQL : public CRUD {
 public:
  MYSQL() = default;

  void create(int a);

  void read();

  // public:
//  template <class T>
//  void createImpl(const T& t) {
//    auto result =
//        collection_.insert_one(serialization::convert_to_bson_document(t));
//    if (!result) {
//      spdlog::error("Database insertion failed");
//    }
//  }
//
//  template <class... Args>
//  void createImpl(Args&&... args) {
//    std::vector<bsoncxx::document::value> documents;
//    documents.reserve(sizeof...(args));
//
//    (documents.push_back(
//         serialization::convert_to_bson_document(std::forward<Args>(args))),
//     ...);
//
//    auto result = collection_.insert_many(documents);
//
//    if (!result) {
//      spdlog::error("Database insertion failed");
//    }
//  }

  int init(const std::string& url);

 private:
  WFMySQLConnection conn{1};
  inline static const std::string TABLE = "table";
};
}  // namespace database

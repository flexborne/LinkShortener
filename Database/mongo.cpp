#include "mongo.h"

#include <fmt/core.h>

#include "../Serialization/bson.h"

#include <spdlog/spdlog.h>

database::MongoConnector::MongoConnector(std::string_view address,
                                         uint16_t port) {
  set_config(address, port);
}

void database::MongoConnector::set_config(std::string_view address, uint16_t port) {
  client_ = mongocxx::client{
      mongocxx::uri{fmt::format("mongodb://{}:{}", address, port)}};
  // Insert a document into a collection
  collection_ = client_["mydatabase"]["mycollection"];
//  auto result = collection_.insert_one(document{} << "name"
//                                                 << "Nastik"
//                                                 << "age"
//                                                 << "35" << finalize);
//  // Build the query
//  auto filter = bsoncxx::builder::basic::make_document(
//      bsoncxx::builder::basic::kvp("name", "Alice"));
//
//  // Read the inserted document
//  auto cursor = collection_.find({});
//  for (auto&& doc : cursor) {
//    std::cout << bsoncxx::to_json(doc) << std::endl;
//  }
}

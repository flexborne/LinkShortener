#include "Mongo.h"

#include <fmt/core.h>

void MongoConnector::setConfig(uint16_t port) {
  client_ = mongocxx::client{
      mongocxx::uri{fmt::format("mongodb://localhost:{}", port)}};
  // Insert a document into a collection
  auto collection = client_["mydatabase"]["mycollection"];
  auto result = collection.insert_one(document{} << "name"
                                                 << "Nastik"
                                                 << "age"
                                                 << "35" << finalize);
  // Build the query
  auto filter = bsoncxx::builder::basic::make_document(
      bsoncxx::builder::basic::kvp("name", "Alice"));

  // Read the inserted document
  auto cursor = collection.find({});
  for (auto&& doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << std::endl;
  }
}
#include "bson.h"
#include "db_record.h"

#include <bsoncxx/builder/basic/document.hpp>

namespace basic = bsoncxx::builder::basic;
//
//template <>
//bsoncxx::document::value serialization::convert_to_bson_document(
//    const DbRecord& record) {
//  return basic::make_document(basic::kvp("short", record.key_),
//                              basic::kvp("long", record.url_));
//}
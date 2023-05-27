#pragma once

#include <bsoncxx/builder/stream/document.hpp>

namespace serialization {
template <class T>
bsoncxx::document::value convert_to_bson_document(const T& t);
}

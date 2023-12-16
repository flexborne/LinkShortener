#pragma once

#include "result_callback.h"
#include "urls_table_handler.h"

#include "crud_interface.h"
#include "workflow/Workflow.h"

#include <string>

namespace dao {

class ShortenedUrlDAO : public CRUD {
 public:
  using Result = Result<std::string>;
  using ResultCallback = ResultCallback<std::string>;

 public:
  explicit ShortenedUrlDAO(SeriesWork* series) : series{series} {
    assert(series != nullptr);
  }

 public:
  void create_impl(std::string original_url, ResultCallback callback) const;

  void read_impl(std::string shortened, ResultCallback callback) const;

 private:
  SeriesWork* series;
};

}  // namespace dao
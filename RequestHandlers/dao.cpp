#include "dao.h"
#include "generate_random_str.h"
#include "result_callback.h"
#include "urls_table_handler.h"

#include "workflow/MySQLResult.h"

#include <chrono>

void dao::ShortenedUrlDAO::create_impl(std::string original_url,
                                       ResultCallback callback) const {
  if (original_url.empty()) {
    callback(
        std::unexpected(Error{"Url cannot be empty", ErrorCode::InvalidInput}));
    return;
  }

  db::UrlsTableHandler handler{series};
  handler.create(
      ShortenedUrl{.shortened = utils::generate_random_string(),
                   .original = original_url,
                   .creation_time = std::chrono::system_clock::now()},
      std::move(callback));
}

void dao::ShortenedUrlDAO::read_impl(std::string shortened,
                                     ResultCallback callback) const {
  if (shortened.empty()) {
    callback(std::unexpected(Error{"Url cannot be empty", ErrorCode::InvalidInput}));
    return;
  }

  db::UrlsTableHandler handler{series};
  handler.read(
      std::move(shortened), UrlsTableColumn::Original,
      [res_callback = std::move(callback)](::Result<protocol::MySQLCell> res) {
        if (!res.has_value()) {
          res_callback(std::unexpected(std::move(res.error())));
          return;
        }
        res_callback(res->as_string());
      });
}

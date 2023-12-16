#pragma once

#include <expected>
#include <functional>

/// @todo rework error mechanism, it should contain more errors and specific data
enum class ErrorCode : uint8_t {
  None,
  InvalidInput,
  MySQLError,
  ResourceNotFound,
  DuplicateEntry,
  InternalError,
};

/// @brief can be empty -> no error. just to avoid optional
struct Error {
  std::string message;
  ErrorCode code = ErrorCode::None;

  /// @return [+] if no error, [-] otheriwse
  operator bool() const { return !message.empty() && code != ErrorCode::None; }
};

template <class Data>
using Result = std::expected<Data, Error>;

using ErrorCallback = std::function<void(Error)>;

template <class Data>
using ResultCallback = std::function<void(Result<Data>)>;
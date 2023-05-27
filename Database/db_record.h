#pragma once

#include <string>
#include <boost/date_time.hpp>


struct DbRecord {
  std::string key_;
  std::string url_;
  boost::gregorian::date date_;
};
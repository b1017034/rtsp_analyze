//
// Created by alpine on 2021/12/29.
//

#include "include/Util.h"
template<typename... Args>
std::string Util::format(const std::string &fmt, Args... args) {
  size_t len = std::snprintf( nullptr, 0, fmt.c_str(), args ... );
  std::vector<char> buf(len + 1);
  std::snprintf(&buf[0], len + 1, fmt.c_str(), args ... );
  return std::string(&buf[0], &buf[0] + len);
}
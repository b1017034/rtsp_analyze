//
// Created by alpine on 2021/12/29.
//

#ifndef RTSP_SOCKET_RELAY_UTIL_H
#define RTSP_SOCKET_RELAY_UTIL_H
#include <string>
#include <cstdio>
#include <vector>

class Util {
public:
  template <typename ... Args>
  static std::string format(const std::string& fmt, Args ... args );
};


#endif //RTSP_SOCKET_RELAY_UTIL_H

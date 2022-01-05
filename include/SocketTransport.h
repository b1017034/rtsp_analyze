//
// Created by alpine on 2022/01/05.
//

#ifndef RTSP_SOCKET_RELAY_SOCKETTRANSPORT_H
#define RTSP_SOCKET_RELAY_SOCKETTRANSPORT_H

#include "string"

struct RtspSocket {
  int src;
  int dst;
};

class SocketTransport {
public:
  ~SocketTransport();
  void start(const std::string& host, int srcPort, int dstPort);
  static int createSrcSocket(const std::string& srcHost, int srcPort);
  int createDstSocket(int dstPort) const;
  static void rtspRelay(void *rtspSocketVoid);
  static void *readToWrite(int file[]);

private:
  int dstSocket;
  int srcSocket;
};


#endif //RTSP_SOCKET_RELAY_SOCKETTRANSPORT_H

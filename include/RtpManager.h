//
// Created by alpine on 2021/12/29.
//

#ifndef RTSP_SOCKET_RELAY_RTPMANAGER_H
#define RTSP_SOCKET_RELAY_RTPMANAGER_H

#include <cstdint>
#include <vector>
#include "RtcpManager.h"
#include <iostream>
#include <fstream>

struct RtpPacket {
  uint8_t  version;
  uint8_t  padding;
  uint8_t  extension;
  uint8_t  csiCount;
  uint8_t  marker;
  uint8_t  payloadType;
  uint16_t sequenceNumber;
  uint32_t timestamp;
  uint32_t ssi;
  std::vector<unsigned char > payload;
};

class RtpManager {
public:
  RtpManager();

  std::vector<RtpPacket> packets{};
  std::ofstream delayFile;
  void decode(std::vector<unsigned char> buf);
  void decode(std::vector<unsigned char> buf, RtcpManager rtcpManager);

};


#endif //RTSP_SOCKET_RELAY_RTPMANAGER_H

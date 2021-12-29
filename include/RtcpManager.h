//
// Created by alpine on 2021/12/29.
//

#ifndef RTSP_SOCKET_RELAY_RTCPMANAGER_H
#define RTSP_SOCKET_RELAY_RTCPMANAGER_H

#include <cstdint>
#include <vector>
#include <string>

struct SdesItem {
  uint8_t type;
  uint8_t length; // length byte
  std::string text;
};

struct RtcpSourceDescription {
  uint8_t  version;
  uint8_t  padding;
  uint8_t  itemCount;
  uint8_t  packetType; //202
  uint16_t length; //length * 32bit

  std::vector<SdesItem> sdesItems;
};

struct RtcpSenderReport {
  uint8_t  version;
  uint8_t  padding;
  uint8_t  itemCount;
  uint8_t  packetType; //200
  uint16_t length; //length * 32bit

  uint32_t senderSSRC;
  uint32_t timestampMSW;
  uint32_t timestampLSW;
  uint32_t rtpTimestamp;
  uint32_t packetCount;
  uint32_t octetCount;

  RtcpSourceDescription sourceDescription;
};

struct RtcpReceiverReport {
  uint8_t version;
  uint8_t padding;
  uint8_t itemCount;
  uint8_t packetType; //201
  uint16_t length;

  uint32_t senderSSRC;

  RtcpSourceDescription sourceDescription;
};



class RtcpManager {
public:
  RtcpManager();
  std::vector<RtcpSenderReport> senderReports;
  std::vector<RtcpReceiverReport> receiverReports;

  void decode(const std::vector<unsigned char>& buf);
  timeval MSWandLSWtoNTP(uint32_t msw, uint32_t lsw);
  timeval getLastNTP();
  void printNTP(timeval t);
};


#endif //RTSP_SOCKET_RELAY_RTCPMANAGER_H

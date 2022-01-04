//
// Created by alpine on 2021/12/29.
//

#include "include/RtpManager.h"
#include <iostream>
#include <utility>
#include <sys/time.h>
#include "Util.cpp"


RtpManager::RtpManager() {
  this->delayFile.open("delay.txt");
}

void RtpManager::decode(std::vector<unsigned char> buf) {
  auto rtp = RtpPacket{};
  rtp.version = buf[0] >> 6;
  rtp.padding = (buf[0] & 0x20) >> 5;
  rtp.extension = (buf[0] & 0x10) >> 4;
  rtp.csiCount = (buf[0] & 0x0f);

  rtp.marker = buf[1] >> 7;
  rtp.payloadType = buf[1] & 0x7f;

  rtp.sequenceNumber - buf[2] * 256 + buf[3];

  rtp.timestamp = (
    buf[4] << 24 |
    buf[5] << 16 |
    buf[6] << 8  |
    buf[7]);

  rtp.ssi = (
    buf[8 ] << 24 |
    buf[9 ] << 16 |
    buf[10] << 8  |
    buf[11]);

  rtp.payload = { buf.begin() + 11, buf.end() };

  this->packets.push_back(rtp);

//  std::cout << ">> RTP Header" << std::endl;
//  std::cout << Util::format("       Version     : %i", rtp.version) << std::endl;
//  std::cout << Util::format("       Padding     : %i", rtp.padding) << std::endl;
//  std::cout << Util::format("       Extension   : %i", rtp.extension) << std::endl;
//  std::cout << Util::format("       CSRC Count  : %i", rtp.csiCount) << std::endl;
//  std::cout << Util::format("       Marker      : %i", rtp.marker) << std::endl;
//  std::cout << Util::format("       Payload Type: %i", rtp.payloadType) << std::endl;
//  std::cout << Util::format("       Sequence    : %i", rtp.sequenceNumber) << std::endl;
//  std::cout << Util::format("       Timestamp   : %lu", rtp.timestamp) << std::endl;
//  std::cout << Util::format("       Sync Source : %lu", rtp.ssi) << std::endl;
}

void RtpManager::decode(std::vector<unsigned char> buf, RtcpManager rtcpManager) {
  decode(std::move(buf));

  if (!rtcpManager.senderReports.empty()) {
    timeval senderTime = rtcpManager.getLastNTP();
    timeval now{};
    gettimeofday(&now, NULL);

    /* delay */
    auto rtp = this->packets.at(this->packets.size() - 1);
    auto rtcp = rtcpManager.senderReports.at(rtcpManager.senderReports.size() - 1);
    int tsDiff = rtp.timestamp - rtcp.rtpTimestamp;
    if (tsDiff < 0) tsDiff = -tsDiff;

    auto lastNTP = senderTime.tv_sec + ((double)senderTime.tv_usec / 1e+6);
    auto nowNTP = now.tv_sec + ((double)now.tv_usec / 1e+6);
    auto delay = nowNTP - (lastNTP + tsDiff / 90000.0);
    this->delayFile << "delay: " << delay << " timestamp: " << rtp.timestamp << std::endl;
  }
}

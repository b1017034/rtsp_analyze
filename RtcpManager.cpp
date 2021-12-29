//
// Created by alpine on 2021/12/29.
//

#include "include/RtcpManager.h"
#include <iostream>
#include "Util.cpp"

RtcpManager::RtcpManager() {

}

void RtcpManager::decode(const std::vector<unsigned char>& buf) {
  uint8_t version    = buf[0] >> 6;
  uint8_t padding    = (buf[0] & 0x20) >> 5;
  uint8_t itemCount  = (buf[0] & 0x1f);
  uint8_t packetType = buf[1];

  switch (packetType) {
    case 200: {
      //SenderReport
      auto senderReport = RtcpSenderReport{};
      senderReport.version      = version;
      senderReport.padding      = padding;
      senderReport.itemCount    = itemCount;
      senderReport.packetType   = packetType;
      senderReport.length       = buf[2] >> 8 | buf[3];
      senderReport.senderSSRC   = (uint32_t)(buf[7] | buf[6] << 8 | buf[5] << 16 | buf[4] << 24);
      senderReport.timestampMSW = (uint32_t)(buf[11] | buf[10] << 8 | buf[9] << 16 | buf[8] << 24);
      senderReport.timestampLSW = (uint32_t)(buf[15] | buf[14] << 8 | buf[13] << 16 | buf[12] << 24);
      senderReport.rtpTimestamp = (uint32_t)(buf[19] | buf[18] << 8 | buf[17] << 16 | buf[16] << 24);
      senderReport.packetCount  = (uint32_t)(buf[23] | buf[22] << 8 | buf[21] << 16 | buf[20] << 24);
      senderReport.octetCount   = (uint32_t)(buf[27] | buf[26] << 8 | buf[25] << 16 | buf[24] << 24);

//      std::cout << ">>RTCP SenderReport" << std::endl;
//      std::cout << Util::format("       Version    : %i", senderReport.version) << std::endl;
//      std::cout << Util::format("       Padding    : %i", senderReport.padding) << std::endl;
//      std::cout << Util::format("       ItemCount  : %i", senderReport.itemCount) << std::endl;
//      std::cout << Util::format("       Packet TP  : %i", senderReport.packetType) << std::endl;
//      std::cout << Util::format("       Length     : %i", senderReport.length) << std::endl;
//      std::cout << Util::format("       SSRC       : %i", senderReport.senderSSRC) << std::endl;
//      std::cout << Util::format("       TS MSW     : %i", senderReport.timestampMSW) << std::endl;
//      std::cout << Util::format("       TS LSW     : %i", senderReport.timestampLSW) << std::endl;
//      std::cout << Util::format("       TS RTP     : %i", senderReport.rtpTimestamp) << std::endl;
//      std::cout << Util::format("       SD PK CT   : %i", senderReport.packetCount) << std::endl;
//      std::cout << Util::format("       SD OC CT   : %i", senderReport.octetCount) << std::endl;
      this->senderReports.push_back(senderReport);
      auto time = this->MSWandLSWtoNTP(senderReport.timestampMSW, senderReport.timestampLSW);
      this->printNTP(time);
      break;
    }
    case 201: {
      auto receiverReport = RtcpSenderReport{};
    }


      break;
    default:
      break;
  }
}

timeval RtcpManager::MSWandLSWtoNTP(uint32_t msw, uint32_t lsw) {
  const auto NTP_EPOCH_OFFSET = static_cast<uint32_t>(2208988800ul);
  struct timeval t{};
  t.tv_sec = msw - NTP_EPOCH_OFFSET;
  t.tv_usec = (uint32_t)((((double)lsw) * 1000000.0) / ((uint32_t)(~0)));
  return t;
}

timeval RtcpManager::getLastNTP() {
  auto senderReport = this->senderReports.at(this->senderReports.size() - 1);
  return this->MSWandLSWtoNTP(senderReport.timestampMSW, senderReport.timestampLSW);
}

void RtcpManager::printNTP(timeval t) {
  /* 経過秒変換 */
  auto now = localtime(&t.tv_sec); // 経過秒を日本時刻に変換
  auto year = now->tm_year+1900; //今年に変換
  auto month = now->tm_mon+1; //今月に変換
  auto day = now->tm_mday; //今日を取得
  auto hour = now->tm_hour;
  auto min = now->tm_min;
  auto sec = now->tm_sec;
  auto usec = t.tv_usec; //マイクロ秒

  std::cout << Util::format("%d/%02d/%02d %02d:%02d:%02d.%d\n",year,month,day,hour,min,sec,usec) << std::endl;
}



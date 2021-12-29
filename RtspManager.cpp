//
// Created by alpine on 2021/12/29.
//

#include "include/RtspManager.h"
#include <cstdio>
#include <iostream>

RtspManager::RtspManager() {

}

std::string RtspManager::cmdOption(const std::string& host, int port, int seq) {
  return Util::format(this->commands.OPTIONS, host.c_str(), port, seq);
}

std::string RtspManager::cmdDescribe(const std::string& url, int seq) {
  return Util::format(this->commands.DESCRIBE, url.c_str(), seq);
}

std::string RtspManager::cmdSetup(const std::string &url, int seq) {
  return Util::format(this->commands.SETUP, url.c_str(), seq);
}

std::string RtspManager::getSession(const std::string &setupRes) {
  std::string session = "Session: ";
  std::string split = ";";
  auto posLeft = setupRes.find(session);
  auto posRight = setupRes.find(split, posLeft + session.length());
  return setupRes.substr(posLeft + session.length(), posRight - (posLeft + session.length()));
}

std::string RtspManager::cmdPlay(const std::string &url, int seq, const std::string &session) {
  return Util::format(this->commands.PLAY, url.c_str(), seq, session.c_str());
}

void RtspManager::decode(std::vector<unsigned char> buf) {
  auto rtspPacket = RtspPacket{};
  /* RTSP Interleaved header */
//  std::cout << "tempBufVec.size(): " << tempBufVec.size() << std::endl;
//  std::cout << "index: " << index << std::endl;
  this->tempBufVec.erase(tempBufVec.begin(), tempBufVec.begin() + index);
  std::copy(buf.begin(), buf.end(), std::back_inserter(this->tempBufVec));
  this->index = 0;
  while(index < this->tempBufVec.size()) {
    if (this->tempBufVec[index] == 0x24) {
      auto interleavedFrame = RtspInterleavedFrame{};
      interleavedFrame.magic =  uint8_t(this->tempBufVec[index]);
      interleavedFrame.channel = uint8_t(this->tempBufVec[index + 1]);
      interleavedFrame.length = uint16_t(this->tempBufVec[index + 2] << 8 | this->tempBufVec[index + 3]);
      index += 4;
//      std::cout << ">> Interleaved Frame" << std::endl;
//      std::cout << Util::format("       length     : %i", interleavedFrame.length) << std::endl;
//      std::cout << Util::format("       channel    : %i", interleavedFrame.channel) << std::endl;

      if (index + interleavedFrame.length >= this->tempBufVec.size()) {
//        std::cout << "buffer is full" << std::endl;
        index += -4;
        break;
      } else if (interleavedFrame.channel == 0x00) {
        //RTP
        rtspPacket.interLeavedFrame = interleavedFrame;
        this->rtpManager.decode({ this->tempBufVec.begin() + index, this->tempBufVec.begin() + (index + rtspPacket.interLeavedFrame.length) }, this->rtcpManager);
        index += int(interleavedFrame.length);

      } else if (interleavedFrame.channel == 0x01) {
        //RTCP
        rtspPacket.interLeavedFrame = interleavedFrame;
        this->rtcpManager.decode({ this->tempBufVec.begin() + index, this->tempBufVec.begin() + (index + rtspPacket.interLeavedFrame.length) });
        index += int(interleavedFrame.length);
      }
    } else {
      index++;
    }

  }
}





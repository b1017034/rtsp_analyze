//
// Created by alpine on 2021/12/29.
//

#ifndef RTSP_SOCKET_RELAY_RTSPMANAGER_H
#define RTSP_SOCKET_RELAY_RTSPMANAGER_H
#include "../Util.cpp"
#include "RtpManager.h"
#include "RtcpManager.h"

struct RtspCommand {
  #define PROTOCOL_PREFIX   "RTSP://"
  #define RTSP_PORT         554
  #define RTSP_CLIENT_PORT  9500
  #define RTSP_RESPONSE     "RTSP/1.0 "
  std::string OPTIONS  = "OPTIONS rtsp://%s:%lu RTSP/1.0\r\nCSeq: %i\r\n\r\n";
  std::string DESCRIBE = "DESCRIBE rtsp://%s RTSP/1.0\r\nCSeq: %i\r\nAccept: application/sdp\r\n\r\n";
  //#define CMD_SETUP         "SETUP %s  RTSP/1.0\r\nCSeq: %i\r\nTransport: RTP/AVP;unicast;client_port=54984-54985\r\n\r\n"
  //#define CMD_SETUP         "SETUP %s  RTSP/1.0\r\nCSeq: %i\r\nTransport: RTP/AVP;unicast;client_port=%i-%i\r\n\r\n"
  std::string SETUP    = "SETUP rtsp://%s RTSP/1.0\r\nCSeq: %i\r\nTransport: RTP/AVP/TCP;interleaved=0-1;\r\n\r\n";
  std::string PLAY     = "PLAY rtsp://%s RTSP/1.0\r\nCSeq: %i\r\nUser-Agent: LibVLC/3.0.14 (LIVE555 Streaming Media v2016.11.28)\r\nSession: %s\r\nRange: npt=0.000-\r\n\r\n";
};

struct RtspInterleavedFrame {
  uint8_t  magic;
  uint8_t  channel;
  uint32_t length;
};

struct RtspPacket {
  RtspInterleavedFrame interLeavedFrame;
};

class RtspManager {
public:
  RtspManager();

  std::string cmdOption(const std::string& host, int port, int seq);
  std::string cmdDescribe(const std::string& url, int seq);
  std::string cmdSetup(const std::string& url, int seq);
  std::string getSession(const std::string& setupRes);
  std::string cmdPlay(const std::string& url, int seq, const std::string& session);

  void decode(std::vector<unsigned char> buf);
private:
  RtspCommand commands;
  RtpManager rtpManager;
  RtcpManager rtcpManager;

  int index = 0;
  std::vector<unsigned char>tempBufVec{};
  std::vector<RtspPacket> rtspPacketVec;
  std::vector<RtpPacket> rtpPacketVec;
};


#endif //RTSP_SOCKET_RELAY_RTSPMANAGER_H

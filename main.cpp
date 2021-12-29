#include <iostream>
#include "include/RtspManager.h"
#include "util/Network.h"

#include <vector>
#define BUFFER_SIZE ( 1024 * 16 )
std::string sendWithResponse(int sock, std::string sendMsg);
std::vector<unsigned char> receive(int sock);

int main() {
  std::string host = "localhost";
  int port = 8554;
  std::string media = "sample.mkv";
  int sock = Network::tcpConnect(host, 8554);
  RtspManager rtsp = RtspManager();
  std::string option = rtsp.cmdOption(host, 8554, 0);
  std::string optionRes = sendWithResponse(sock, option);

  std::string describe = rtsp.cmdDescribe(host, port, media, 1);
  std::string describeRes = sendWithResponse(sock, describe);

  std::string setup = rtsp.cmdSetup(host, port, media, 2);
  std::string setupRes = sendWithResponse(sock, setup);
  std::string session = rtsp.getSession(setupRes);

  std::string play = rtsp.cmdPlay(host, port, media, 3, session);
  std::string playRes = sendWithResponse(sock, play);
  std::cout << playRes << std::endl;
  while (true) {
    auto vec = receive(sock);
    if (vec.empty()) break;
    rtsp.decode({vec.begin(), vec.end()});
  }
  return 0;
}

std::string sendWithResponse(int sock, std::string sendMsg) {
  int n = send(sock, sendMsg.c_str(), sendMsg.size(), 0);
  if (n <= 0) {
    std::cout << "send failed" << std::endl;
    return "";
  }

  char buf[BUFFER_SIZE];
  memset(buf, 0, sizeof(buf));

  n = recv(sock, buf, sizeof(buf), 0);
  return buf;
}

std::vector<unsigned char> receive(int sock) {
  char buf[BUFFER_SIZE];

  memset(buf, 0, sizeof(buf));

  int n = recv(sock, buf, sizeof(buf), 0);
  if ( n == 0) {
    std::cout << "size: " << n << std::endl;
  } else {
    std::vector<unsigned char> bufVec(buf, buf + n);
    return bufVec;
  }

  return {};
}

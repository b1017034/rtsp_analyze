//
// Created by alpine on 2022/01/05.
//

#include "include/SocketTransport.h"
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

void SocketTransport::start(const std::string& host, int srcPort, int dstPort) {
  this->dstSocket = createDstSocket(dstPort);
  int connect;
  pthread_t rtspThread;

  while (true) {
    struct sockaddr_in peerSin{};
    socklen_t len = sizeof(peerSin);

    connect = accept(this->dstSocket, (struct sockaddr *)&peerSin, &len);
    if (connect == -1) {
      std::cout << "[ERR]: Failed Connect Dst Socket" << std::endl;
      exit(EXIT_FAILURE);
    }
    this->srcSocket = createSrcSocket(host, srcPort);

    RtspSocket rtspSocket{};
    rtspSocket.src = this->srcSocket;
    rtspSocket.dst = this->dstSocket;
    if (pthread_create(&rtspThread, nullptr, (void *(*)(void *)) rtspRelay, (void *) &rtspSocket) != 0) {
      std::cout << "[ERR]: Failed Create Thread" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

}

int SocketTransport::createSrcSocket(const std::string& srcHost, int srcPort) {
  int sock;
  struct sockaddr_in addr{};

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET; //ipv4
  addr.sin_port = htons(srcPort);
  addr.sin_addr.s_addr = inet_addr(srcHost.c_str());

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    std::cout << "[ERR]: Failed Create Src Socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    std::cout << "[ERR]: Failed Connect Src Socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  return sock;
}

int SocketTransport::createDstSocket(int dstPort) const {
  int sock;
  struct sockaddr_in addr{};
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET; //ipv4
  addr.sin_port = htons(dstPort);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (this->srcSocket < 0) {
    std::cout << "[ERR]: Failed Create Dst Socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    std::cout << "[ERR]: Failed Bind Dst Port" << std::endl;
    exit(EXIT_FAILURE);
  }

  listen(sock, SOMAXCONN);
  std::cout << "[INFO]: Success Create Dst Socket" << std::endl;
  return sock;
}

void SocketTransport::rtspRelay(void *rtspSocketVoid) {
  auto *rtspSocket = static_cast<RtspSocket *>(rtspSocketVoid);
  std::cout << "hello" << std::endl;
}

SocketTransport::~SocketTransport() {
  close(this->dstSocket);
  close(this->srcSocket);
}

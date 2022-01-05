//
// Created by alpine on 2022/01/05.
//

#include "include/SocketTransport.h"
#include "include/RtspManager.h"
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define BUFFER_SIZE ( 1024 * 16  )
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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

    int rtspSocket[2];
    rtspSocket[0] = this->srcSocket;
    rtspSocket[1] = this->dstSocket;
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

void* SocketTransport::rtspRelay(int sock[]) {
  int srcSock = sock[0];
  int dstSock = sock[1];
  int filesIN[3] = {srcSock, dstSock, 0};
  int filesOUT[3] = {dstSock, srcSock, 1};
  pthread_t pthreadIN, pthreadOUT;

  int status;
  status = pthread_create(&pthreadIN ,nullptr,(void *(*)(void *))readToWrite, filesIN);
  if(status!=0){
    fprintf(stderr,"pthread_create : %s",strerror(status));
    exit(EXIT_FAILURE);
  }
  status = pthread_create(&pthreadOUT ,nullptr,(void *(*)(void *))readToWrite, filesOUT);
  if(status!=0){
    fprintf(stderr,"pthread_create : %s",strerror(status));
    exit(EXIT_FAILURE);
  }

  pthread_join(pthreadOUT, nullptr);
  pthread_cancel(pthreadIN);

  close(dstSock);
  close(srcSock);

  return sock;
}

void* SocketTransport::readToWrite(int *file) {
  char buffer[BUFFER_SIZE];

  ssize_t read_size;
  int in_file = file[0];
  int out_file = file[1];
  bool readRTSP = file[2];
  RtspManager rtspManager;

  while(true){
    read_size = read(in_file, buffer, sizeof(buffer));
    if (read_size == 0){
      break; //EOF
    }
    //close
    if(read_size < 0){
      break;
    }

    pthread_mutex_lock( &mutex );
    if(readRTSP) {
//      std::vector<unsigned char> vectorVBuffer(buffer, buffer + read_size);
//      rtsp.decode(vectorVBuffer, read_size);
    }
    pthread_mutex_unlock( &mutex );

    write(out_file, buffer, (unsigned int) read_size);
  }
  return (file);
}

SocketTransport::~SocketTransport() {
  close(this->dstSocket);
  close(this->srcSocket);
}

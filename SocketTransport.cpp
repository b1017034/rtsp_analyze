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

//void *socketRelay(int file[]){
//  int connectedSocket = file[0];
//  int clientSock = file[1];
//
//  int files_in[3] = {connectedSocket, clientSock, 0};
//  pthread_t   thread_id1, thread_id2;
//  int         status;
//
//  status = pthread_create(&thread_id1,nullptr,(void *(*)(void *))readToWrite, files_in);
//  if(status!=0){
//    fprintf(stderr,"pthread_create : %s",strerror(status));
//    exit(EXIT_FAILURE);
//  }
//
//  int files_out[3] = {clientSock, connectedSocket, 2};
//  status = pthread_create(&thread_id2,nullptr,(void *(*)(void *))readToWrite, files_out);
//  if(status!=0){
//    fprintf(stderr,"pthread_create : %s",strerror(status));
//    exit(EXIT_FAILURE);
//  }
//
//  pthread_join(thread_id2,nullptr);
//  pthread_cancel(thread_id1);
//
//  close(connectedSocket);
//  close(clientSock);
//
//  return file;
//}

SocketTransport::~SocketTransport() {
  close(this->dstSocket);
  close(this->srcSocket);
}

//void *SocketTransport::readToWrite(int *file) {
//  char buffer[BUFFER_SIZE];
//
//  ssize_t read_size;
//  int in_file = file[0];
//  int out_file = file[1];
//  bool readRTSP = file[2];
//  RTSP rtsp(true);
//
//  while(true){
//    read_size = read(in_file, buffer, sizeof(buffer));
//    if (read_size == 0){
//      break; //EOF
//    }
//    //close
//    if(read_size < 0){
//      break;
//    }
//
//    pthread_mutex_lock( &mutex );
//    if(readRTSP) {
////      std::vector<unsigned char> vectorVBuffer(buffer, buffer + read_size);
////      rtsp.decode(vectorVBuffer, read_size);
//    }
//    pthread_mutex_unlock( &mutex );
//
//    write(out_file, buffer, (unsigned int) read_size);
//  }
//  return (file);
//}

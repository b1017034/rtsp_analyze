//
// Created by alpine on 2022/01/05.
//

#include "./include/SocketTransport.h"

#include "string"
#include "iostream"
#include "csignal"

void signalHandler( int signal ){
  fprintf(stderr,"EXIT SIGNAL:%d\n",signal);
  exit(EXIT_FAILURE);
}

int main() {
  SocketTransport socketTransport{};
  std::string host = "127.0.0.1";
  int srcPort = 554;
  int dstPort = 8080;

  std::cout << "transport: localhost:" << dstPort << " <-> " << host << ":" << srcPort << std::endl;

  signal( SIGINT, signalHandler );
  signal( SIGKILL, signalHandler );
  signal( SIGTERM, signalHandler );

  socketTransport.start(host, srcPort, dstPort);

  return EXIT_SUCCESS;
}


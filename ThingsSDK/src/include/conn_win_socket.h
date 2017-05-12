#ifndef CONN_LINUX_SOCKET_H
#define CONN_LINUX_SOCKET_H

#include "connection.h"
//#include "stdint.h"

class Things;
class Window_socket: public Connection {
 private:
  char     _host[50];
  uint16_t _port;
 public:
  int sockfd;

  Window_socket();
  ~Window_socket();
  virtual int set_connect_info(const char* connection_str);
  virtual int do_connect_to();
  virtual int do_disconnect();
  virtual int  connected(); //0 = not connected  1 = connected ; -1 = error
  virtual ssize_t do_send_data(const char* buf, size_t len);
  virtual ssize_t do_recv_data(char* buf, size_t len);
};

#endif


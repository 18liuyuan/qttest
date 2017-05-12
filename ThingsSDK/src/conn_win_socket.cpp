#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <winsock.h>
#include "src/include/conn_win_socket.h"
#include "src/include/things.h"


#pragma comment(lib,"ws2_32.lib")


Window_socket::Window_socket() {
	WSADATA sd;
	WORD ver=0x0101;
	if(WSAStartup(ver,&sd))
	{
		printf("SOCKET³õÊ¼»¯Ê§°Ü£¡");
		return;
	}
  sockfd = -1;
}
Window_socket::~Window_socket() {
  if (sockfd != -1) {
    
  };
}

int Window_socket::set_connect_info(const char* connection_str) {
    int result;
    char buf[50];
    if (strlen(connection_str) > sizeof(buf)) { return -1; };
    while (1) {
        strcpy(buf, connection_str);
        char *p, *p1, *p2, *p3, *p4;
        p = get_token_first((char*)&buf, ':'); p1 = p;
        p = get_token_next(p, ';');      p2 = p;
        p = get_token_next(p, ':');      p3 = p;
        p = get_token_rest(p);           p4 = p;
        if (strcmp(p1, "host") != 0) { result = -1; break;};
        if (strcmp(p3, "port") != 0) { result = -1; break;};
        _port = atoi(p4);
        strcpy(_host, p2);
        result = 0;
        break;
    };
    if (result < 0) {
        _port = 0;
    };
    return result;
};

int Window_socket::do_connect_to() {
  if (_port == 0) return -1;
  int error, yes = 1;
  u_long flags;
  struct sockaddr_in serv_addr;
  struct hostent* server;
  printf("connect to us.thingscloud.cn:8008\n");
    while (1) {
        error = 0;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);				 
        if(sockfd  < 0) { 
			error=WSAGetLastError();
			//error = -1; 
			break; 
		};
        if(sockfd == 0) { error = -2; break; };
	/*	if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) {
            error = -3; break;
        }*/
		flags = 1;
        int x = ioctlsocket(sockfd,FIONBIO,&flags);//fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);      
		if (x  < 0) { error = -4; break; };
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));
      //  bzero((char *)&serv_addr, sizeof(serv_addr));
		memset((char *)&serv_addr, 0, sizeof(serv_addr));
        server = gethostbyname(_host);
        if(server == NULL) { error = -5; break; };
        serv_addr.sin_family = AF_INET;
        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        serv_addr.sin_port = htons(_port);
        int connector = connect(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
        if((connector < 0) && (errno != WSAEWOULDBLOCK)) {
            printf("connect error\n");
            error = -5;
            break;
        } else {
            error = 0;
            fprintf(stdout, "Made a connection to %s\n", inet_ntoa(serv_addr.sin_addr));
            break;
        };
    };
  return 0;
}
int Window_socket::do_disconnect(){
  if (sockfd != -1) {
	closesocket(sockfd);
  }
  return 0;
}
int Window_socket::connected(){
    int rc;
    struct timeval tv;
    fd_set fdset;
    
    if (sockfd != -1) {
        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        rc = select(sockfd + 1, &fdset, NULL, NULL, &tv);
    // if ((error == 0) && (errno != EINPROGRESS)) {
    //   owner->post_event(E_socket_connected, NULL, 0);
    // } else if (error < 0) {
    //   owner->post_event(E_socket_error, NULL, 0);
        if (rc >= 0) {
            return 1;
        } else {
            if (errno != WSAEWOULDBLOCK) {
                return 0;
            } else {
                return -1;
            }
        };
    };
    return -1;
}
ssize_t Window_socket::do_send_data(const char* buf, size_t len){
  //t->print_out_with_frame(data);
    int rc;
    rc = send(sockfd, buf, len, 0);
    return (int)rc;
  
  return 0;
}
ssize_t Window_socket::do_recv_data(char* buf, size_t len) {
    int rc;
    struct timeval tv;
    fd_set fdset;
    if (sockfd != -1) {
        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        rc = select(sockfd + 1, &fdset, NULL, NULL, &tv);
        if ((rc > 0) && FD_ISSET(sockfd, &fdset)) {
            rc = recv(sockfd, buf, len, 0);
            len = (int)rc;
            if (rc < 0) {
                if (errno != WSAEWOULDBLOCK) {
                    // owner->post_event(E_socket_error, NULL, 0);
                    len = -1;
                    sockfd = -1;
                }
            } else if (rc == 0) {
                // owner->post_event(E_socket_close, NULL, 0);
                sockfd = -1;
            } else {
                buf[len] = 0;
                //printf("socket data available:len=%d data = %s\n", len, buffer);
                // owner->push_data_to_stream(buffer, len);
            };
        } else {
            len = 0;
        };
    } else {
        len = 0;
    };
    return len;
}

// client.cpp 2021/09/29
// for test

// #include "spdlog/spdlog.h"
#include <arpa/inet.h>
#include <cerrno>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 1024

void str_cli(FILE *fp, int sockfd);

static ssize_t my_read(int fd, char *ptr) {
  static int read_cnt = 0;
  static char *read_ptr;
  static char read_buf[MAXLINE];

  if (read_cnt <= 0) {
    if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)
      perror("read error");
    else if (read_cnt == 0)
      return (0);
    read_ptr = read_buf;
  }

  read_cnt--;
  *ptr = *read_ptr++ & 255;
  return (1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen) {
  int n, rc;
  char c, *ptr;

  ptr = (char *)vptr;
  for (n = 1; n < maxlen; n++) {
    if ((rc = my_read(fd, &c)) == 1) {
      *ptr++ = c;
      if (c == '\n')
        break;
    } else if (rc == 0) {
      *ptr = 0;
      return (n - 1); /* EOF, n - 1 bytes were read */
    } else
      return (-1); /* error */
  }

  *ptr = 0;
  return (n);
}
/* end readline */

int main(int argc, const char *argv[]) {
  int sockfd;
  struct sockaddr_in servaddr;

  if (argc != 3) {
    perror("usage: tcpcli <IPaddress> <Port>");
    return 1;
  }
  uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return 1;
  }
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    return 2;
  }

  str_cli(stdin, sockfd); /* do it all */

  exit(0);
}

void str_cli(FILE *fp, int sockfd) {
  char sendline[MAXLINE], recvline[MAXLINE];

  while (true) {
    // printf("recv->: ");
    printf("send=> ");
    fgets(sendline, MAXLINE, fp);
    write(sockfd, sendline, strlen(sendline));

    if (readline(sockfd, recvline, MAXLINE) == 0)
      perror("str_cli: server terminated prematurely");

    printf("recv: %s", recvline);
  }
}

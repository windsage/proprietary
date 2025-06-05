/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <algorithm>
#include <arpa/inet.h>
#include <errno.h>
#include <log/log.h>
#include <stdlib.h>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>

//       #include <sys/time.h>
//       #include <sys/types.h>

#include <unistd.h>
#include "xpan_ac_int.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "vendor.qti.bluetooth.xpan_ac.XpanSocketHandler"

/* Xpan Socket Manager Implementation.*/

namespace xpan {
namespace ac {

int XpanSocketHandler::tcp_lfd;
int XpanSocketHandler::tcp_lport;
int XpanSocketHandler::udp_data_fd;
int XpanSocketHandler::udp_data_port_rx;
int XpanSocketHandler::udp_tsf_fd;
int XpanSocketHandler::udp_tsf_port_rx;
int XpanSocketHandler::hdlr_pipe_fd[2];
int XpanSocketHandler::nfds;
std::vector<int> XpanSocketHandler::mReadFds;
std::vector<bdaddr_t> XpanSocketHandler::mListeners;
fd_set XpanSocketHandler::rfds;

XpanSocketHandler::XpanSocketHandler(bdaddr_t addr, XpanEarbudRole role) {
  tcp_fd = -1;
  mBdAddr = addr;
  mRole = role;
  qhci = XpanQhciAcIf::GetIf();
}

XpanSocketHandler::~XpanSocketHandler() {
  ALOGD("%s", __func__);
  if (tcp_fd > 0) {
    close(tcp_fd);
  }
  tcp_fd = -1;
}

int XpanSocketHandler::GetTcpFd() {
  return tcp_fd;
}

int XpanSocketHandler::GetUdpDataFd() {
  return udp_data_fd;
}

int XpanSocketHandler::GetUdpTsfFd() {
  return udp_tsf_fd;
}

ipaddr_t sockaddr_to_ip(int fd) {
  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return {};
  }

  ipaddr_t ip = xac->GetIpFromFd(fd);
  if (!ip.isEmpty()) {
    return ip;
  }

  struct sockaddr_in addr;
  socklen_t addr_size = sizeof(struct sockaddr_in);
  int res = getpeername(fd, (struct sockaddr *)&addr, &addr_size);
  if (res != XPAN_AC_SUCCESS) {
    ALOGE("%s: Couldnt locate IP of remote. Ignore data", __func__);
    return {};
  }

  ip.type = IPv4;
  ip.ipv4[0] = (uint8_t)(addr.sin_addr.s_addr);
  ip.ipv4[1] = (uint8_t)(addr.sin_addr.s_addr >> 8);
  ip.ipv4[2] = (uint8_t)(addr.sin_addr.s_addr >> 16);
  ip.ipv4[3] = (uint8_t)(addr.sin_addr.s_addr >> 24);
  ALOGD("%s: Sockaddr_in IP (%s)", __func__, ip.toString().c_str());

  return ip;
}

int XpanSocketHandler::GetLastFd() {
  int maxFd = 0;
  for (int i = 0; i < mReadFds.size(); i++) {
    if (maxFd < mReadFds[i]) {
      maxFd = mReadFds[i];
    }
  }

  ALOGD("%s: max fd = %d", __func__, maxFd);//debug
  return maxFd;
}

int XpanSocketHandler::GetTcpListeningPort() {
  return tcp_lport;
}

int XpanSocketHandler::GetUdpPort() {
  return udp_data_port_rx;
}

int XpanSocketHandler::GetUdpTsfPort() {
  return udp_tsf_port_rx;
}

void XpanSocketHandler::HandleUpdateReadFds(char *msg, int len) {
  if (!msg || (len < 5)) {
    ALOGE("%s: Invalid message. Ignore.", __func__);
    return;
  }

  /* Other thread writes message to the pipe to either add/remove
   * fd from read set used for select call.
   * Format of the message :-
   *      "Action-fd"
   *       Where, Action: add/remove
   *              fd: fd integer value
   */
  char add[] = "add", remove[] = "remove";
  char fd_act[2][20];
  char* tmp = NULL;
  char* token = strtok_r(msg, "-", &tmp);
  int idx = 0; // Index to token list
  while (token != NULL && idx < 2){
    size_t token_len = strlen(token) + 1; // extra byte needed by strlcpy for null terminator
    strlcpy(fd_act[idx], token, token_len); // Copy to token list
    idx++;
    token = strtok_r(NULL, "-", &tmp);
  }

  int fd = atoi(fd_act[1]);
  if (fd == 0) {
    ALOGE("%s: Invalid fd. Skip the value", __func__);
    return;
  }

  if (!strcmp(fd_act[0], add)) {
    ALOGD("%s: Add new fd(%d) to the set", __func__, fd);
    // Add new fd to the fd set
    UpdateFdSet(true, fd);
  } else if (!strcmp(fd_act[0], remove)) {
    ALOGD("%s: Remove fd(%d) from the set", __func__, fd);
    UpdateFdSet(false, fd);
  }
}

bool XpanSocketHandler::SendData(uint8_t llid, std::vector<uint8_t> &data) {
  if (tcp_fd == -1) {
    ALOGE("%s: TCP Connection is not created yet", __func__);
    return false;
  }

  void *buf = (void *)data.data();
  size_t len = (size_t)data.size();

  ssize_t ret = send(tcp_fd, buf, len, 0);
  if (ret <= 0) {
    ALOGE("%s: Failed to write data error: %s", __func__, strerror(errno));
    return false;
  }

  if (llid == LE_L2CAP_CONT || llid == LE_L2CAP_START) {
    // callback to QHCI (Number of packets completed)
    qhci->NumberOfPacketsCompleted(mBdAddr, 1);
  }

  return true;
}

/* Tx Handler APIs */
bool XpanSocketHandler::InitiateTxRoutine() {
  return true;
}

/* Intermediate API to transfer context to new thread */
bool XpanSocketHandler::InitTcpConnection(XpanSocketHandler *sock, ipaddr_t remote_ip,
                       uint32_t tcp_port) {
  mTcpConnThread = std::thread(XpanSocketHandler::ConnectTcp, sock, remote_ip, tcp_port);
  mTcpConnThread.detach();
  ALOGD("%s: TCP Connect Initiated in other thread", __func__);
  return true;
}

/* Generates tokens based on delimiter and string */
std::vector<std::string> GetTokens(const char* p, char *msg) {
  std::vector<std::string> tokens;

  char* tmp = NULL;
  char* token = strtok_r(msg, p, &tmp);

  while (token != NULL){
    std::string tok(token);
    tokens.push_back(token);
    token = strtok_r(NULL, p, &tmp);
  }

  return tokens;
}

std::string TruncateLeadingZeros(std::string str) {
  char valid_ip[16] = {};
  int j = 0;
  for (int i = 0; i < str.length(); i++) {
    if (i > 0 && str[i] == '0' && str[i-1] == '.')
      continue;
    valid_ip[j] = str[i];
    j++;
  }
  valid_ip[j] = '\0';
  std::string ret(valid_ip);
  return ret;
}

bool XpanSocketHandler::ConnectTcp(XpanSocketHandler *sock, ipaddr_t remote_ip,
                                   uint32_t tcp_port) {
  ALOGD("%s: IP = %s, port = %ld", __func__, remote_ip.toString().c_str(), tcp_port);

  XpanSocketHandler *sock_hdlr = static_cast<XpanSocketHandler *>(sock);
  if (!sock_hdlr) {
    ALOGE("%s: Invalid socket Handler instance", __func__);
    return false;
  }

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  XpanTcpConnectionFailedEvt *evt;
  XpanTcpConnectedEvt *msg;
  struct sockaddr_in servaddr;
  socklen_t addrlen;

  /* create socket for client */
  sock_hdlr->tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_hdlr->tcp_fd == -1) {
    ALOGE("%s: socket creation failed with server(%s:%d) failed: %s", __func__,
          remote_ip.toString().c_str(), tcp_port, strerror(errno));
    evt = (XpanTcpConnectionFailedEvt *)malloc(sizeof(XpanTcpConnectionFailedEvt));
    evt->event = XPAN_TCP_CONNECT_FAILED;
    evt->addr = sock_hdlr->mBdAddr;
    evt->ip = remote_ip;
    evt->role = sock_hdlr->mRole;
    xac->PostMessage((xac_handler_msg_t *)evt, MSG_PRIORITY_DEFAULT);
    return false;
  }

  bzero(&servaddr, sizeof(servaddr));
  addrlen = sizeof(struct sockaddr_in);
  servaddr.sin_family = AF_INET ;
  servaddr.sin_port = htons(tcp_port);
  std::string ip_upd = TruncateLeadingZeros(remote_ip.toString());
  inet_pton(AF_INET, ip_upd.c_str(), &servaddr.sin_addr.s_addr);

  uint8_t retry_count = 0, retval, MAX_RETRY_COUNT = 2;

  do {
    retval = connect(sock_hdlr->tcp_fd,(struct sockaddr *)&servaddr, addrlen);
    int connect_err = errno;

    /* Note - TCP Connection to be retried again if connection with remote fails
              with No route to Host reason */
    if (retval != 0 && connect_err == EHOSTUNREACH) {
      retry_count++;
      if (retry_count < MAX_RETRY_COUNT) {
        ALOGE("%s: retry TCP connection for %s", __func__, strerror(errno));
      }
    } else {
      break;
    }
  } while (retry_count < MAX_RETRY_COUNT);
  if (retval != 0) {
    ALOGE("%s: connect with server(%s:%d) failed: %s", __func__,
          remote_ip.toString().c_str(), tcp_port, strerror(errno));
    evt = (XpanTcpConnectionFailedEvt *)malloc(sizeof(XpanTcpConnectionFailedEvt));
    evt->event = XPAN_TCP_CONNECT_FAILED;
    evt->addr = sock_hdlr->mBdAddr;
    evt->ip = remote_ip;
    evt->role = sock_hdlr->mRole;
    xac->PostMessage((xac_handler_msg_t *)evt, MSG_PRIORITY_DEFAULT);
    // Close opened socked
    close(sock_hdlr->tcp_fd);
    sock_hdlr->tcp_fd = -1;
    return false;
  }

  /* add fd to I/O multiplexing Rx thread */
  std::string int_msg = "add-" + std::to_string(sock_hdlr->tcp_fd) + "*";
  ALOGD("%s: Add fd to I/O Mux fd set(msg-> %s)", __func__, int_msg.c_str());
  write(hdlr_pipe_fd[1], int_msg.c_str(), int_msg.length());

  /* Send TCP Connected Evt to State Machine */
  msg = (XpanTcpConnectedEvt *)malloc(sizeof(XpanTcpConnectedEvt));
  msg->event = XPAN_TCP_CONNECTED_EVT;
  msg->addr = sock_hdlr->mBdAddr;
  msg->ip = remote_ip;
  msg->role = sock_hdlr->mRole;
  msg->fd = sock_hdlr->tcp_fd;
  msg->isIncoming = false;
  xac->PostMessage((xac_handler_msg_t *)msg, MSG_PRIORITY_DEFAULT);

  return true;
}

/* AP to AP Roaming transport preparation */
/* Intermediate API to transfer context to new thread */
bool XpanSocketHandler::InitiateRoamingPrep(XpanSocketHandler *sock, ipaddr_t remote_ip,
                       uint32_t tcp_port) {
  mTcpConnThread = std::thread(XpanSocketHandler::PrepareRoamingTransport, sock, remote_ip, tcp_port);
  mTcpConnThread.detach();
  ALOGD("%s: TCP Connect Initiated in other thread", __func__);
  return true;
}

bool XpanSocketHandler::PrepareRoamingTransport(XpanSocketHandler *sock, ipaddr_t remote_ip,
                                   uint32_t tcp_port) {
  ALOGD("%s: IP = %s, port = %ld", __func__, remote_ip.toString().c_str(), tcp_port);

  XpanSocketHandler *sock_hdlr = static_cast<XpanSocketHandler *>(sock);
  if (!sock_hdlr) {
    ALOGE("%s: Invalid socket Handler instance", __func__);
    return false;
  }

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return false;
  }

  XpanNewApTransportStatus *evt;
  struct sockaddr_in servaddr;
  socklen_t addrlen;

  /* create socket for client */
  sock_hdlr->tcp_fd_roaming = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_hdlr->tcp_fd_roaming == -1) {
    ALOGE("%s: socket creation failed with server(%s:%d) failed: %s", __func__,
          remote_ip.toString().c_str(), tcp_port, strerror(errno));
    evt = (XpanNewApTransportStatus *)malloc(sizeof(XpanNewApTransportStatus));
    evt->event = XPAN_TCP_NEW_AP_TRANSPORT_STATUS;
    evt->addr = sock_hdlr->mBdAddr;
    evt->ip = remote_ip;
    evt->role = sock_hdlr->mRole;
    evt->status = 0xFF;
    xac->PostMessage((xac_handler_msg_t *)evt, MSG_PRIORITY_DEFAULT);
    return false;
  }

  bzero(&servaddr, sizeof(servaddr));
  addrlen = sizeof(struct sockaddr_in);
  servaddr.sin_family = AF_INET ;
  servaddr.sin_port = htons(tcp_port);
  std::string ip_upd = TruncateLeadingZeros(remote_ip.toString());
  inet_pton(AF_INET, ip_upd.c_str(), &servaddr.sin_addr.s_addr);

  int retval = connect(sock_hdlr->tcp_fd,(struct sockaddr *)&servaddr, addrlen);

  if (retval != 0) {
    ALOGE("%s: connect with server(%s:%d) failed: %s", __func__,
          remote_ip.toString().c_str(), tcp_port, strerror(errno));
    evt = (XpanNewApTransportStatus *)malloc(sizeof(XpanNewApTransportStatus));
    evt->event = XPAN_TCP_NEW_AP_TRANSPORT_STATUS;
    evt->addr = sock_hdlr->mBdAddr;
    evt->ip = remote_ip;
    evt->role = sock_hdlr->mRole;
    evt->status = 0xFF;
    xac->PostMessage((xac_handler_msg_t *)evt, MSG_PRIORITY_DEFAULT);
    return false;
  }

  /* add fd to I/O multiplexing Rx thread */
  std::string int_msg = "add-" + std::to_string(sock_hdlr->tcp_fd_roaming);
  ALOGD("%s: Add fd to I/O Mux fd set(msg-> %s)", __func__, int_msg.c_str());
  write(hdlr_pipe_fd[1], int_msg.c_str(), int_msg.length() + 1);

  /* Send TCP Connected Evt to State Machine */
  evt = (XpanNewApTransportStatus *)malloc(sizeof(XpanNewApTransportStatus));
  evt->event = XPAN_TCP_NEW_AP_TRANSPORT_STATUS;
  evt->addr = sock_hdlr->mBdAddr;
  evt->ip = remote_ip;
  evt->role = sock_hdlr->mRole;
  evt->status = (retval == 0 ? XPAN_AC_SUCCESS: 0xFF);
  xac->PostMessage((xac_handler_msg_t *)evt, MSG_PRIORITY_DEFAULT);

  return true;
}

void XpanSocketHandler::HandleRoamingCompletion() {
  // remove tcp fd from FD Set
  UpdateFdSet(false, tcp_fd);

  // Close previous socket connection
  int ret = shutdown(tcp_fd, SHUT_RDWR);  // Is this needed
  if (!ret) {
    ALOGD("%s: Shutdown successful", __func__);
  }
  close(tcp_fd);

  tcp_fd = tcp_fd_roaming;
  tcp_fd_roaming = -1;
}

bool XpanSocketHandler::CreateTcpSocketForIncomingConnection(ipaddr_t ip, bdaddr_t addr) {
  ALOGD("%s: For bd addr %s", __func__, addr.toString().c_str());

  if (IsListeningOnTcp()) {
    ALOGD("%s: Already Listening for inc connection", __func__);
    AddListenerFor(addr);
    return true;
  }

  struct sockaddr_in servaddr;
  int retval = -1;

  tcp_lfd = socket(AF_INET, SOCK_STREAM| SOCK_NONBLOCK, 0);
  if (tcp_lfd < 0) {
    ALOGE("%s: TCP Socket creation failed: %s", __func__, strerror(errno));
    return false;
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET ;
  std::string ip_upd = TruncateLeadingZeros(ip.toString());
  inet_pton(AF_INET, ip_upd.c_str(), &servaddr.sin_addr.s_addr);
  servaddr.sin_port = 0; // gets port dynamically

  if ((retval = bind(tcp_lfd, (sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
    ALOGE("%s: Failed to bind udp port for incoming data: %s", __func__, strerror(errno));
    return false;
  }

  struct sockaddr_in sock_addr;
  socklen_t  addr_len = sizeof(sock_addr);
  if (getsockname(tcp_lfd, (struct sockaddr *)&sock_addr, &addr_len) == 0) {
    tcp_lport = ntohs(sock_addr.sin_port);
  }

  ALOGD("%s: TCP Socket is ready to receive TCP data on port %d",
        __func__, tcp_lport);

  retval = listen(tcp_lfd, 5); // 5 max incoming connection in queue allowed
  if (retval == -1) {
    ALOGE("%s: failed to start listening for tcp ip:%s port:%d error:%s",
          __func__, ip.toString().c_str(), tcp_lport, strerror(errno));
    return false;
  }

  AddListenerFor(addr);

  /* add fd to I/O multiplexing Rx thread */
  std::string int_msg = "add-" + std::to_string(tcp_lfd);
  ALOGD("%s: Add fd to I/O Mux fd set(msg-> %s)", __func__, int_msg.c_str());
  write(hdlr_pipe_fd[1], int_msg.c_str(), int_msg.length() + 1);

  return true;
}

bool XpanSocketHandler::IsListeningOnTcp() {
  if (tcp_lfd <= 0) {
    return false;
  }
  return true;
}

bool XpanSocketHandler::AcceptIncomingTcpConnection() {
  struct sockaddr_in clientaddr;
  socklen_t addrlen = sizeof(struct sockaddr_in);

  int fd = accept4(tcp_lfd, (sockaddr *)&clientaddr, &addrlen, SOCK_NONBLOCK);
  if (fd == -1) {
    ALOGE("%s: Accept Connection Failed (%d - %s)", __func__, fd, strerror(errno));
    return false;
  }

  // Extract IP address (for IPv4)
  char ip_addr[INET_ADDRSTRLEN];
  if (clientaddr.sin_family != AF_INET) {
    ALOGE("%s: not a IP address. close socket.", __func__);
    close(fd);
    return false;
  }
  inet_ntop(AF_INET, &(clientaddr.sin_addr), ip_addr, INET_ADDRSTRLEN);
  ALOGD("%s: Connection accepted for %s: ",__func__, ip_addr);

  // Add new fd to the fd set
  UpdateFdSet(true, fd);

  return true;
}

void XpanSocketHandler::CloseConnectionSocket() {
  if (tcp_fd > 0) {
    int ret = shutdown(tcp_fd, SHUT_RDWR);  // Is this needed
    if (!ret) {
      ALOGD("%s: Shutdown successful", __func__);
    }

    ret = close(tcp_fd);
    if (!ret) {
      ALOGD("%s: socket close successful", __func__);
    }

    // remove the socket from I/O mux
    ALOGD("%s: Remove tcp_fd (%d from FD_SET)", __func__, tcp_fd);
    std::string int_msg = "remove-" + std::to_string(tcp_fd);
    write(hdlr_pipe_fd[1], int_msg.c_str(), int_msg.length() + 1);

  } else {
    ALOGE("%s: Socket already closed", __func__);
  }

  tcp_fd = -1;
}

void XpanSocketHandler::CloseListeningSocket(bdaddr_t addr) {
  if (!ContainsListener(addr)) {
    ALOGE("%s: No incoming connection expected for %s", __func__, ConvertRawBdaddress(addr));
    return;
  }

  RemoveListenerFor(addr);
  if (mListeners.size() > 0) {
    ALOGD("%s: Other Incoming connections pending", __func__);
    return;
  }

  if (tcp_lfd > 0) {
    int ret = close(tcp_lfd);
    if (!ret) {
      ALOGD("%s: socket close successful"
            ". listening socket (fd %d)", __func__, tcp_lfd);
      std::string int_msg = "remove-" + std::to_string(tcp_lfd);
      write(hdlr_pipe_fd[1], int_msg.c_str(), int_msg.length() + 1);
    }
  } else {
    ALOGE("%s: Socket already closed", __func__);
  }

  tcp_lfd = -1;
}

bool XpanSocketHandler::CreateUdpSocketForIncomingData(ipaddr_t ip) {
  ALOGD("%s", __func__);

  struct sockaddr_in servaddr;
  int retval = -1;

  udp_data_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_data_fd < 0) {
    ALOGE("%s: Udp Socket creation failed (%s)", __func__, strerror(errno));
    return false;
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET ;
  inet_pton(AF_INET, ip.toString().c_str(), &servaddr.sin_addr.s_addr);
  servaddr.sin_port = 0; // gets port dynamically

  if ((retval = bind(udp_data_fd, (sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
    ALOGE("%s: Failed to bind udp port for incoming data: %s", __func__, strerror(errno));
    return false;
  }


  struct sockaddr_in sock_addr;
  socklen_t  addr_len = sizeof(sock_addr);
  if (getsockname(udp_data_fd, (struct sockaddr *)&sock_addr, &addr_len) == 0) {
      udp_data_port_rx = ntohs(sock_addr.sin_port);
  }

  ALOGD("%s: UDP Socket is ready to receive audio data on port %d",
        __func__, udp_data_port_rx);

  return true;
}

bool XpanSocketHandler::CreateUpdSocketForTsf(ipaddr_t ip) {
  ALOGD("%s", __func__);
  struct sockaddr_in servaddr;
  int retval = -1;

  udp_tsf_fd = socket(AF_INET,SOCK_DGRAM, 0);
  if (udp_tsf_fd < 0) {
    ALOGE("%s: Udp Socket creation failed (%s)", __func__, strerror(errno));
    return false;
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET ;
  inet_pton(AF_INET, ip.toString().c_str(), &servaddr.sin_addr.s_addr);
  servaddr.sin_port = 0; // gets port dynamically

  if((retval = bind(udp_tsf_fd, (sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
    ALOGE("%s: Failed to bind udp port (%s)", __func__, strerror(errno));
    return false;
  }

  struct sockaddr_in sock_addr;
  socklen_t  addr_len = sizeof(sock_addr);
  if (getsockname(udp_tsf_fd, (struct sockaddr *)&sock_addr, &addr_len) == 0) {
      udp_tsf_port_rx = ntohs(sock_addr.sin_port);
  }

  ALOGD("%s: UDP Socket is ready to receive audio tsf sync data on port %d",
        __func__, udp_tsf_port_rx);

  return true;
}

void XpanSocketHandler::CloseTsfRxUdpPort() {
  if (udp_tsf_fd > 0) {
    close(udp_tsf_fd);
    udp_tsf_fd = -1;
  }
}

void XpanSocketHandler::CloseDataRxUdpPort() {
  if (udp_data_port_rx > 0) {
    close(udp_data_port_rx);
    udp_data_port_rx = -1;
  }
}

void XpanSocketHandler::CloseInternalPipeFds() {
  if (hdlr_pipe_fd[1] > 0) {
    ALOGD("%s: Closing Write pipe end %d", __func__, hdlr_pipe_fd[1]);
    close(hdlr_pipe_fd[1]);
    hdlr_pipe_fd[1] = -1;
  }

  if (hdlr_pipe_fd[0] > 0) {
    ALOGD("%s: Closing Read pipe end %d", __func__, hdlr_pipe_fd[0]);
    close(hdlr_pipe_fd[0]);
    hdlr_pipe_fd[0] = -1;
  }
}

void XpanSocketHandler::HandleRxDateParsingFailure(int fd, uint8_t ret) {
  ALOGD("%s: fd = %d return value = %d", __func__, fd, ret);

  std::vector<uint8_t> cmd_data;
  const uint16_t length = 3;
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_MSG));
  addUint16ToData(cmd_data, length);
  cmd_data.push_back((uint8_t)(0xFF & XPAN_LMP_NOT_ACCEPTED));
  addUint16ToData(cmd_data, (0xFFFF & ret));

  if (ret == XPAN_CONNECTION_ALREADY_EXISTS ||
      ret == XPAN_LMP_UNKNOWN_PDU ||
      ret == XPAN_LMP_INVALID_PARAMS) {
    void *buf = (void *)cmd_data.data();
    size_t len = (size_t)cmd_data.size();

    ssize_t ret_size = send(fd, buf, len, 0);
    if (ret_size <= 0) {
      ALOGE("%s: Failed to write data error: %s", __func__, strerror(errno));
    }
  }

  if (ret == XPAN_CONNECTION_ALREADY_EXISTS ||
      ret == XPAN_CONNECTION_FAILED_TO_ESTABLISH ||
      ret == XPAN_CONN_DOESNT_EXIST) {
    // remove fd frm fd set
    ALOGE("%s: remove fd %d from FD_SET(Invalid Lmp Connect Attempt)", __func__);
    UpdateFdSet(false, fd);  // remove fd from fd set
    close(fd); // close the socket
  }
}

void XpanSocketHandler::HandleRemoteDisconnection(int fd) {
  // Remote the FD from FD Set
  UpdateFdSet(false, fd);

  XpanApplicationController *xac = XpanApplicationController::Get();
  if (!xac) {
    ALOGE("%s: Xpan Application Controller is not initialized.", __func__);
    return;
  }

  ipaddr_t ip = xac->GetIpFromFd(fd);
  if (ip.isEmpty()) {
    ALOGE("%s: No IP associated with fd (%d)", __func__, fd);
    return;
  }

  XpanDevice *device = xac->GetDeviceByIpAddr(ip);
  if (!device) {
    ALOGE("%s: No associated device instance with ip (%s)",
          __func__, ip.toString().c_str());
    return;
  }

  bdaddr_t addr = device->GetAddr();
  XpanEarbudRole role = device->GetRoleByIpAddr(ip);

  // Send Remote Disconnected event to state machine
  XpanTcpDisconnectionEvt *msg =
    (XpanTcpDisconnectionEvt *)malloc(sizeof(XpanTcpDisconnectionEvt));
  msg->event = XPAN_TCP_DISCONNECTED_EVT;
  msg->addr = addr;
  msg->ip = ip;
  msg->role = role;
  msg->status = LOCAL_USER_TERMINATED_CONNECION;

  xac->PostMessage((xac_handler_msg_t *)msg);
}

void XpanSocketHandler::ResetFdSet() {
  FD_ZERO(&rfds);

  for (int i = 0; i < mReadFds.size(); i++) {
    if (mReadFds[i] >= FD_SETSIZE || mReadFds[i] < 0 ||
        (fcntl(mReadFds[i], F_GETFD) < 0)) {
      ALOGE("%s: Invalid fd = %d", __func__, mReadFds[i]);
      UpdateFdSet(false, mReadFds[i]);
      continue;
    }
    FD_SET(mReadFds[i], &rfds);
  }
}

/* Rx Handler API's */
bool XpanSocketHandler::InitiateRxRoutine() {
  ALOGD("%s", __func__);
  int retval = -1;

  int status = pipe(hdlr_pipe_fd);
  if (status != XPAN_AC_SUCCESS) {
    ALOGE("%s: Pipe creation failed", __func__);
    return false;
  }

  ALOGD("%s: read fd = %d, write fd = %d", __func__,
        hdlr_pipe_fd[0], hdlr_pipe_fd[1]);

  mReadFds.clear();
  FD_ZERO(&rfds);
  FD_SET(hdlr_pipe_fd[0], &rfds);
  nfds++;
  mReadFds.push_back(hdlr_pipe_fd[0]);

  do {
    ResetFdSet();
    int max_fd = GetLastFd();
    retval = select((max_fd + 1), &rfds, NULL, NULL, NULL);

    ALOGD("%s: select() -> retval = %d", __func__, retval);

    switch (retval) {
      case EBADF:
           ALOGE("%s: select Error: An invalid fd given in the set", __func__);
           break;

      case EINTR:
           ALOGE("%s: select Error: A signal was caught", __func__);
           break;

      case EINVAL:
           ALOGE("%s: select Error: nfds is negative or exceeds the limit"
                 "or Invalid timeout mentioned ", __func__);
           break;

      case ENOMEM:
           ALOGE("%s: Unable to allocate memory for internal tables", __func__);
           break;

      default: {
        for (int i = 0; i < nfds; i++) {
          if (mReadFds[i] >= FD_SETSIZE || mReadFds[i] < 0) {
            ALOGE("%s: Invalid fd = %d", __func__, mReadFds[i]);
            continue;
          }
          if (FD_ISSET(mReadFds[i],&rfds)) {
            /* Identify this fd */
            ALOGD("%s: Event on fd = %d", __func__, mReadFds[i]);

            /* fd for internal events like add/remove fd from select */
            if (mReadFds[i] == hdlr_pipe_fd[0]) {
              ALOGD("%s: Internal event to manage fds", __func__);
              // TODO: parse message to add/remove fd
              char msg[FD_MSG_LEN];
              int len = read(hdlr_pipe_fd[0], msg, FD_MSG_LEN);
              std::vector<std::string> fdact_list = GetTokens("*", msg);
              for (int l = 0; l < fdact_list.size(); l++) {
                HandleUpdateReadFds(fdact_list[l].data(), fdact_list[l].length());
              }

            /* Incoming TCP connection */
            } else if (mReadFds[i] == tcp_lfd) {
              ALOGD("%s: Incoming connection event", __func__);
              AcceptIncomingTcpConnection();

            /* TCP data received */
            } else {
              uint8_t buffer[XPAN_CTR_DATA_LEN];
              ssize_t data_len = recv(mReadFds[i], &buffer, sizeof(buffer), 0);
              ALOGD("%s: TCP data receieved. len = %d ", __func__, data_len);
              if (data_len <= 0) {
                ALOGE("%s: Incorrect data length (%d). Disconnection event",
                      __func__, data_len);
                HandleRemoteDisconnection(mReadFds[i]);
                // TODO: Possible disconnection
              } else {
                ipaddr_t ip = sockaddr_to_ip(mReadFds[i]);
                if (ip.isEmpty()) {
                  ALOGE("%s: not a valid ip address. Ignore data", __func__);
                  continue;
                }
                bool ret = XpanLmpManager::parseXpanEvent(buffer, data_len, ip, mReadFds[i]);
                if (ret != XPAN_AC_SUCCESS) {
                  HandleRxDateParsingFailure(mReadFds[i], ret);
                }
              }
            }
          }
        }
      }
    }
  } while (1);

  return true;
}

void XpanSocketHandler::SetMasterFdForConnection(int fd) {

}

/* Sets the tcp_fd after incoming connection is established */
void XpanSocketHandler::SetSocketFd(int fd) {
  tcp_fd = fd;
}

void XpanSocketHandler::UpdateFdSet(bool add, int fd) {
  ALOGD("%s: add = %d, fd = %d", __func__, add, fd);
  if (add) {
    if (fd >= FD_SETSIZE || fd < 0 || (fcntl(fd, F_GETFD) < 0)) {
      ALOGE("%s: Invalid fd = %d", __func__, fd);
      return;
    }
    nfds++;
    //add fd to the list
    mReadFds.push_back(fd);
  } else {
    if (std::find(mReadFds.begin(), mReadFds.end(), fd) != mReadFds.end()) {
      nfds--;
      //remove fd from list
      mReadFds.erase(std::remove(mReadFds.begin(), mReadFds.end(), fd), mReadFds.end());
    } else {
      ALOGD("%s: fd not present in the set", __func__);
    }
  }
}

bool XpanSocketHandler::ContainsListener(bdaddr_t addr) {
  std::vector<bdaddr_t>::iterator it = mListeners.begin();
  while (it != mListeners.end()) {
    if (*it == addr) {
      return true;
    } else {
      it++;
    }
  }
  return false;
}

bool XpanSocketHandler::AddListenerFor(bdaddr_t addr) {
  mListeners.push_back(addr);
  return true;
}

bool XpanSocketHandler::RemoveListenerFor(bdaddr_t addr) {
  std::vector<bdaddr_t>::iterator it = mListeners.begin();
  while (it != mListeners.end()) {
    if (*it == addr) {
      /* Remove entry */
      it = mListeners.erase(it);
      ALOGD("%s: removed %s", __func__, ConvertRawBdaddress(addr));
      return true;
    } else {
      it++;
    }
  }
  return false;
}

bool ipStringToIpAddr(const char *str, ipaddr_t *addr) {
  if (!str || !addr) {
    ALOGE("%s: Invalid string to convert to ip address", __func__);
    return false;
  }

  //TODO: validate that the string is IP address
  if (sscanf(str, "%hu.%hu.%hu.%hu",
      &addr->ipv4[0], &addr->ipv4[1], &addr->ipv4[2], &addr->ipv4[3]) != 4) {
    ALOGE("%s: Error while parsing string to IP address", __func__);
    return false;
  }

  return true;
}

}
}

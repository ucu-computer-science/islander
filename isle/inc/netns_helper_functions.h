//
// Created by yaroslav_morozevych on 27.11.21.
//

#ifndef NAMESPACES_NETNS_HELPER_FUNCTIONS_H
#define NAMESPACES_NETNS_HELPER_FUNCTIONS_H


#include <arpa/inet.h>
#include <net/if.h>


#define MAX_PAYLOAD 1024
#define BUFFER_SIZE 1024


/* Create a socket. */
int create_socket(int domain, int type, int protocol) {
    int sock_fd = socket(domain, type, protocol);
    if (sock_fd < 0) kill_process("Can't open socket: %m\n");
    return sock_fd;
}


/* Read response from the socket. */
static ssize_t read_response(int fd, struct msghdr *msg, char **response) {
    struct iovec *iov = msg->msg_iov;
    iov->iov_base = *response;
    iov->iov_len = MAX_PAYLOAD;

    ssize_t resp_len = recvmsg(fd, msg, 0);

    if (resp_len == 0) kill_process("EOF on netlink\n");
    if (resp_len < 0) kill_process("netlink receive error: %m\n");

    return resp_len;
}


/* Check if we can read a response from the socket. */
static void check_response(int sock_fd) {
    struct iovec iov;
    struct msghdr msg = {
            .msg_name = NULL,
            .msg_namelen = 0,
            .msg_iov = &iov,
            .msg_iovlen = 1
    };
    char *resp = malloc(MAX_PAYLOAD);

    ssize_t resp_len = read_response(sock_fd, &msg, &resp);

    struct nlmsghdr *hdr = (struct nlmsghdr *) resp;
    int nlmsglen = (int)hdr->nlmsg_len;
    unsigned int datalen = nlmsglen - sizeof(*hdr);

    // Did we read all data?
    if (datalen < 0 || nlmsglen > resp_len) {
        if (msg.msg_flags & MSG_TRUNC) kill_process("received truncated message\n");
        kill_process("malformed message: nlmsg_len=%d\n", nlmsglen);
    }

    // Was there an error?
    if (hdr->nlmsg_type == NLMSG_ERROR) {
        struct nlmsgerr *err = (struct nlmsgerr *) NLMSG_DATA(hdr);
        if (datalen < sizeof(struct nlmsgerr)) fprintf(stderr, "ERROR truncated!\n");
        if(err->error) {
            errno = -err->error;
            kill_process("RTNETLINK: %m\n");
        }
    }

    free(resp);
}


/* Send a Netlink message via socket. */
static void send_nlmsg(int sock_fd, struct nlmsghdr *n) {
    struct iovec iov = {
            .iov_base = n,
            .iov_len = n->nlmsg_len
    };

    struct msghdr msg = {
            .msg_name = NULL,
            .msg_namelen = 0,
            .msg_iov = &iov,
            .msg_iovlen = 1
    };
    n->nlmsg_seq++;

    ssize_t status = sendmsg(sock_fd, &msg, 0);
    if (status < 0) kill_process("cannot talk to rtnetlink: %m\n");

    check_response(sock_fd);
}


/* Get the network namespace fd given the PID. */
int get_netns_fd(int pid) {
    char path[BUFFER_SIZE];
    sprintf(path, "/proc/%d/ns/net", pid);

    int fd = open(path, O_RDONLY);
    if (fd < 0) kill_process("Can't read netns file %s: %m\n", path);

    return fd;
}


/* Up the interface in its network namespace. */
void interface_up(char *ifname, char *ip, char *netmask) {
    int sock_fd = create_socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, ifname, strlen(ifname));

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(struct sockaddr_in));
    saddr.sin_family = AF_INET;
    saddr.sin_port = 0;

    char *p = (char *) &saddr;

    saddr.sin_addr.s_addr = inet_addr(ip);
    memcpy(((char *)&(ifr.ifr_addr)), p, sizeof(struct sockaddr));
    if (ioctl(sock_fd, SIOCSIFADDR, &ifr))
        kill_process("cannot set ip addr %s, %s: %m\n", ifname, ip);

    saddr.sin_addr.s_addr = inet_addr(netmask);
    memcpy(((char *)&(ifr.ifr_addr)), p, sizeof(struct sockaddr));
    if (ioctl(sock_fd, SIOCSIFNETMASK, &ifr))
        kill_process("cannot set netmask for addr %s, %s: %m\n", ifname, netmask);

    ifr.ifr_flags |= IFF_UP | IFF_BROADCAST | IFF_RUNNING | IFF_MULTICAST;
    if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr))
        kill_process("cannot set flags for addr %s, %s: %m\n", ifname, ip);

    close(sock_fd);
}

#endif //NAMESPACES_NETNS_HELPER_FUNCTIONS_H

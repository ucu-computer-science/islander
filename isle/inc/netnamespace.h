#ifndef NAMESPACES_NETNAMESPACE_H
#define NAMESPACES_NETNAMESPACE_H


#include <stdio.h>
#include <linux/rtnetlink.h>
#include <linux/veth.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>

#include "netns_helper_functions.h"

#define INT_EXISTS  1
#define VETH        "veth0"
#define VPEER       "veth1"
#define IFACE       "wlp0s20f3"
#define VETH_ADDR   "10.1.1.1"
#define VPEER_ADDR  "10.1.1.2"
#define NETMASK     "255.255.255.0"


// Netlink Message Structure.
struct nl_req {
    struct nlmsghdr n;     // Netlink Message Header
    struct ifinfomsg i;    // Useful Payload (Starts with NIS module information)
    char buf[MAX_PAYLOAD]; // The rest of payload
};


#define NLMSG_TAIL(nmsg) ((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))


/* Return True if such network interface exists. Return 0 otherwise.
 *    iface: name of the network interface. */
int interface_exists(const char* iface) {
    // Create a linked list of structures describing the network interfaces of the local system.
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    // Walk through linked list, maintaining head pointer so we can free list later.
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_PACKET)
            continue;

        if (strcmp(ifa->ifa_name, iface) == 0)
            return INT_EXISTS;
        // printf("Interface: %s\n", ifa->ifa_name);
    }

    freeifaddrs(ifaddr);
    return 0;
}


/* Add attribute to the Netlink message. For example, if ifname is veth0,
 * then the following will be added to the message:
 * {
 *   rta_type: IFLA_IFNAME
 *   rta_len: 5 (len(veth0) + 1)
 *   data: veth0\0
 * }
 * Parameters:
 *    header: Netlink Message Header.
 *    maxlen: Message header max size.
 *    type: Option of how to interpret the contents of the useful payload.
 *    data: The rest useful payload.
 *    datalen: The size of the rest useful payload. */
static void addattr_l(struct nlmsghdr *header, int maxlen, __u16 type, const void *data, __u16 datalen) {
    __u16 attr_len = RTA_LENGTH(datalen); // __u16 is unsigned 16 bits
    __u32 newlen = NLMSG_ALIGN(header->nlmsg_len) + RTA_ALIGN(attr_len);
    if (newlen > maxlen) kill_process("Can't add attribute. size (%d) exceeded maxlen (%d).\n", newlen, maxlen);

    // Useful payload in Netlink is encoded as the list of arguments that can be contained.
    // Encode useful payload in rtattr structure.
    struct rtattr *rta;
    rta = NLMSG_TAIL(header);
    rta->rta_type = type;    // how to interpret the contents of the useful payload
    rta->rta_len = attr_len; // the length of the useful payload

    // Add data as the rest useful payload.
    if (datalen) memcpy(RTA_DATA(rta), data, datalen);
    header->nlmsg_len = newlen;
}


/* Add nested attribute to Netlink message.
 *    header: Netlink Message Header.
 *    maxlen: Message header max size.
 *    type: Option of how to interpret the contents of the useful payload. */
static struct rtattr *addattr_nest(struct nlmsghdr *header, int maxlen, __u16 type) {
    struct rtattr *nest = NLMSG_TAIL(header);
    addattr_l(header, maxlen, type, NULL, 0);
    return nest;
}


/* Denote the end (length) of the nested attribute.
 *    header: Netlink Message Header.
 *    nest: Netlink Attribute (useful payload) to be nested into header. */
static void addattr_nest_end(struct nlmsghdr *header, struct rtattr *nest) {
    nest->rta_len = (void *)NLMSG_TAIL(header) - (void *)nest;
}


/* Create a tunnel between veth0 in the parent namespace and veth1 in the child one.
 * This function is analogous to shell command:
 * ip link add veth0 type veth peer name veth1
 *     sock_fd: File descriptor of the NETLINK socket.
 *     ifname: Name of veth interface in the parent ns.
 *     peername: Name of veth interface in the child ns. */
void create_veth(int sock_fd, char *ifname, char *peername) {
    __u16 flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL | NLM_F_ACK;
    struct nl_req req = {
            .n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg)),
            .n.nlmsg_flags = flags,
            .n.nlmsg_type = RTM_NEWLINK, // Create, remove, or get information about a specific network interface.
            .i.ifi_family = PF_NETLINK,
    };

    // Get Netlink Message Header.
    struct nlmsghdr *n = &req.n;
    int maxlen = sizeof(req);  // Netlink message size.

    // Creating r0 attribute with the info about veth.
    addattr_l(n, maxlen, IFLA_IFNAME, ifname, strlen(ifname) + 1);

    // Add the nested attribute r1 to r0, that contains iface info.
    struct rtattr *linfo = addattr_nest(n, maxlen, IFLA_LINKINFO);

    // Denote device type as veth.
    addattr_l(&req.n, sizeof(req), IFLA_INFO_KIND, "veth", 5);

    // Adding one more nested attribute r2.
    struct rtattr *linfodata = addattr_nest(n, maxlen, IFLA_INFO_DATA);

    // Next nested attribute r3 contains a peer name, veth1 in our case.
    struct rtattr *peerinfo = addattr_nest(n, maxlen, VETH_INFO_PEER);

    n->nlmsg_len += sizeof(struct ifinfomsg);

    // Creating r3 attribute with the info about veth.
    addattr_l(n, maxlen, IFLA_IFNAME, peername, strlen(peername) + 1);

    addattr_nest_end(n, peerinfo);   // Denote the end of the nested attribute r3.
    addattr_nest_end(n, linfodata);  // Denote the end of the nested attribute r2.
    addattr_nest_end(n, linfo);      // Denote the end of the nested attribute r1.

    send_nlmsg(sock_fd, n);  // Send Netlink message header.
}


/* Move veth1 to the new (child) namespace so that veth0 is in parent ns while veth1 is in the child one.
 * This function is analogous to shell command: ip link set veth1 netns <ns-name>
 *     sock_fd: File descriptor of the NETLINK socket.
 *     ifname: Name of veth interface in the parent ns.
 *     netns: */
void move_to_pid_netns(int sock_fd, char *ifname, int netns) {
    struct nl_req req = {
            .n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg)),
            .n.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
            .n.nlmsg_type = RTM_NEWLINK,
            .i.ifi_family = PF_NETLINK,
    };

    addattr_l(&req.n, sizeof(req), IFLA_NET_NS_FD, &netns, 4);
    addattr_l(&req.n, sizeof(req), IFLA_IFNAME, ifname, strlen(ifname) + 1);
    send_nlmsg(sock_fd, &req.n);
}


/** This function can be modified. In order to allow multiple containers to run at the same time,
 *  it is better to create a switch and a bridge between each veth interface from child namespace
 *  and the switch. Then assign a veth interface from the parent namespace as a default gateway to
 *  the switch. It should solve all the problems regarding multiple network namespaces.
 *
 *  Guide: https://www.youtube.com/watch?v=j_UUnlVC2Ss **/


/* Create a tunnel between veth interfaces on both the parent and child side.
 * Move veth1 to the child net namespace. Up those interfaces.
 *     child_pid: PID of the child process. */
static void set_netns(int child_pid) {
    char command[100];

    // Create NETLINK socket.
    int sock_fd = create_socket(PF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE);

    // Get the namespace instances.
    int parent_netns = get_netns_fd(getpid());
    int child_netns = get_netns_fd(child_pid);

    // Check if interface exists. If thus - delete it.
    if (interface_exists(VETH) == INT_EXISTS) {
        sprintf(command, "ip li delete %s 2>/dev/null", VETH);
        system(command);
    }
    // Create veth link.
    sprintf(command, "ip link add %s type veth peer name %s", VETH, VPEER);
    system(command);

    // Move veth1 to the child netns.
    move_to_pid_netns(sock_fd, VPEER, child_netns);

    // Setup IP address of ${VETH}.
    sprintf(command, "ip addr add %s/24 dev %s", VETH_ADDR, VETH);
    system(command);

    // Up the ${VETH} interface.
    sprintf(command, "ip link set %s up", VETH);
    system(command);

    // Enter the child namespace to up ${VPEER}.
    if (setns(child_netns, CLONE_NEWNET))
        kill_process("Failed to setns for command at pid %d: %m\n", child_pid);

    // Assign the IP address to the ${VPEER}.
    sprintf(command, "ip addr add %s/24 dev %s", VPEER_ADDR, VPEER);
    system(command);

    // Up veth and lo interfaces in the child namespace.
    sprintf(command, "ip link set %s up", VPEER);
    system(command);
    system("ip link set lo up");

    // Assign the Default Gateway for the VPEER.
    sprintf(command, "ip route add default via %s", VETH_ADDR);
    system(command);

    // Return back to the parent namespace.
    if (setns(parent_netns, CLONE_NEWNET))
        kill_process("Failed to restore previous net namespace: %m\n");

    // Ensure that the packet forwarding is allowed.
    system("echo 1 > /proc/sys/net/ipv4/ip_forward");

    // Flush forward and NAT rules.
    system("iptables -P FORWARD DROP");
    system("iptables -F FORWARD");
    system("iptables -t nat -F");

    // Enable masquerading of 10.1.1.0.
    sprintf(command, "iptables -t nat -A POSTROUTING -s %s/24 -o %s -j MASQUERADE", VETH_ADDR, IFACE);
    system(command);
    sprintf(command, "iptables -A FORWARD -o %s -i %s -j ACCEPT", IFACE, VETH);
    system(command);
    sprintf(command, "iptables -A FORWARD -i %s -o %s -j ACCEPT", IFACE, VETH);
    system(command);
}


/** C version of the NET namespace that can only transfer packets between parent and child namespaces. **/
//static void set_netns(int child_pid) {
//    // Create NETLINK socket.
//    int sock_fd = create_socket(PF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE);
//
//    // Create a tunnel between veth1 and veth0.
//    create_veth(sock_fd, VETH, VPEER);
//
//    // Up the veth0 inferface in the parent process.
//    interface_up(VETH, VETH_ADDR, NETMASK);
//
//    int parent_netns = get_netns_fd(getpid());
//    int child_netns = get_netns_fd(child_pid);
//
//    // Move veth1 to the child netns.
//    move_to_pid_netns(sock_fd, VPEER, child_netns);
//
//    // Enter the child namespace to up veth1.
//    if (setns(child_netns, CLONE_NEWNET))
//        kill_process("Failed to setns for command at pid %d: %m\n", child_pid);
//
//    // Up the veth1 inferface in the child process.
//    interface_up(VPEER, VPEER_ADDR, NETMASK);
//
//    // Return back to the parent namespace.
//    if (setns(parent_netns, CLONE_NEWNET))
//        kill_process("Failed to restore previous net namespace: %m\n");
//
//    close(sock_fd);
//}

#endif //NAMESPACES_NETNAMESPACE_H

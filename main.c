#include <stdio.h>
#include <string.h> /* strcpy */
#include <stdlib.h> /* EXIT_FAILURE */
#include <errno.h>  /* perror */
#include <pcap/pcap.h>

#include <netinet/in.h>
#include <arpa/inet.h>

char errbuf[PCAP_ERRBUF_SIZE];

typedef struct pcap_addr pcap_addr;

void pcap_error_exit(const char * const s);
void print_address(struct sockaddr *addr);
int init_pcap(pcap_if_t *alldevsp);
void print_devices(pcap_if_t *alldevsp);
void app_runner(pcap_if_t *alldevsp);

int
main(int argc, char **argv)
{
    pcap_if_t *alldevsp;

    if (!init_pcap(alldevsp)) pcap_error_exit(errbuf);

    app_runner(alldevsp);

    pcap_freealldevs(alldevsp);

    return 0;
}

void
app_runner(pcap_if_t *alldevsp)
{ 
    print_devices(alldevsp);
}

void 
print_devices(pcap_if_t *alldevsp)
{
    pcap_if_t *devicep;
    pcap_addr *address;

    for (devicep = alldevsp; devicep->next; devicep = devicep ->next) { 
        printf("\nDEVICE:\n");
        printf("\tNAME: %s\n", devicep->name);
        printf("\tDESC: %s\n", devicep->description);
        printf("\tFLAGS: 0x%x\n", devicep->flags);
        printf("\tADDR:\n");

        for(address = devicep->addresses; address != NULL; address = address->next) {
            if (address->addr) {
                printf("\t\tAddress: ");
                print_address(address->addr);
            }
            if (address->netmask) {
                printf("\t\tNetmask: ");
                print_address(address->netmask);
            }
            if (address->broadaddr) {
                printf("\t\tBroadcast Address: ");
                print_address(address->broadaddr);
            }
            if (address->dstaddr) {
                printf("\t\tP2P Destination: ");
                print_address(address->dstaddr);
            }
        }
    }
}

void 
print_address(struct sockaddr *addr) 
{
    char ipstr[INET6_ADDRSTRLEN];
    int ai_family, unk_flag;
    void *sin_addr;
    char *ipver;

    unk_flag = 0;
    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in *) addr;
        sin_addr = &sin->sin_addr;
        ai_family = AF_INET;
    } else if (addr->sa_family == AF_INET6) {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) addr;
        sin_addr = &sin6->sin6_addr;
        ai_family = AF_INET6;
    } else {
        unk_flag = 1;
        strcpy(ipstr, "Unknown AF");
    }

    if (!unk_flag) inet_ntop(ai_family, sin_addr, ipstr, sizeof ipstr);
    printf("%s\n", ipstr);
}

void
pcap_error_exit(const char * const s)
{ 
    fprintf(stderr, "%s\n", s);
    exit(EXIT_FAILURE);
}

int 
init_pcap(pcap_if_t *alldevsp)
{
    return pcap_init(PCAP_CHAR_ENC_LOCAL, errbuf) == 0 && pcap_findalldevs(&alldevsp, errbuf) == 0;
}


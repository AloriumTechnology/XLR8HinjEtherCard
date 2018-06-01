// Simple UDP listening server
//
// Author: Brian Lee
//
// Copyright: GPL V2
// See http://www.gnu.org/licenses/gpl.html
//
// 2018-01-18 <info@aloriumtech.com> Modified by Bryan Craker of Alorium Technology to support the builtin Hinj Ethernet interface

#include "XLR8HinjEtherCard.h"
#include "net.h"

#define gPB XLR8Ether.buffer

#define UDPSERVER_MAXLISTENERS 8    //the maximum number of port listeners.

typedef struct {
    UdpServerCallback callback;
    uint16_t port;
    bool listening;
} UdpServerListener;

UdpServerListener listeners[UDPSERVER_MAXLISTENERS];
byte numListeners = 0;

void XLR8HinjEtherCard::udpServerListenOnPort(UdpServerCallback callback, uint16_t port) {
    if(numListeners < UDPSERVER_MAXLISTENERS)
    {
        listeners[numListeners] = (UdpServerListener) {
            callback, port, true
        };
        numListeners++;
    }
}

void XLR8HinjEtherCard::udpServerPauseListenOnPort(uint16_t port) {
    for(int i = 0; i < numListeners; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (listeners[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) listeners[i].port)) {
            listeners[i].listening = false;
        }
    }
}

void XLR8HinjEtherCard::udpServerResumeListenOnPort(uint16_t port) {
    for(int i = 0; i < numListeners; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (listeners[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) listeners[i].port)) {
            listeners[i].listening = true;
        }
    }
}

bool XLR8HinjEtherCard::udpServerListening() {
    return numListeners > 0;
}

bool XLR8HinjEtherCard::udpServerHasProcessedPacket(uint16_t plen) {
    bool packetProcessed = false;
    for(int i = 0; i < numListeners; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (listeners[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) listeners[i].port) && listeners[i].listening)
        {
            uint16_t datalen = (uint16_t) (gPB[UDP_LEN_H_P] << 8)  + gPB[UDP_LEN_L_P] - UDP_HEADER_LEN;
            listeners[i].callback(
                listeners[i].port,
                gPB + IP_SRC_P,
                (gPB[UDP_SRC_PORT_H_P] << 8) | gPB[UDP_SRC_PORT_L_P],
                (const char *) (gPB + UDP_DATA_P),
                datalen);
            packetProcessed = true;
        }
    }
    return packetProcessed;
}

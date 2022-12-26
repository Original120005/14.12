#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "windows.h"
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

void CheckReceiveMessage(char* receiveBuf, int& seconds) {
    char hamburger[] = "hamburger";
    char cola[] = "cola";
    char potato[] = "potato";
    int size = strlen(receiveBuf);

    int first = 0, i = 0, h = 0, c = 0, p = 0, count_hamburger = 0, count_cola = 0, count_potato = 0;
    while (i < size) {
        while (receiveBuf[i] != ' ') {
            if (receiveBuf[i] == hamburger[h]) {
                count_hamburger++;
                h++;
            }
            if (receiveBuf[i] == cola[c]) {
                count_cola++;
                c++;
            }
            if (receiveBuf[i] == potato[p]) {
                count_potato++;
                p++;
            }
            i++;
        }
        if (count_hamburger == strlen(hamburger)) {
            seconds += 5000;
            count_hamburger = 0;
        }
        if (count_cola == strlen(cola)) {
            seconds += 3000;
            count_cola = 0;
        }
        if (count_potato == strlen(potato)) {
            seconds += 1000;
            count_potato = 0;
        }
        i++;
    }
}

int main()
{
    WSADATA wsadata;

    int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res != NO_ERROR)
    {
        cout << "WSAStartup failked with error " << res << endl;
        return 1;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        cout << "socket failed with error " << WSAGetLastError() << endl;
        return 2;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(23000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    if (bind(udpSocket, (SOCKADDR*)&addr, sizeof(addr)) != NO_ERROR)
    {
        cout << "bind failed with error " << WSAGetLastError() << endl;
        return 3;
    }

    const size_t receiveBufSize = 1024;
    char receiveBuf[receiveBufSize];

    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    cout << "Receiving data...." << endl;
    int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "recvfrom failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    receiveBuf[bytesReceived] = '\0';
    cout << "Client: " << receiveBuf << endl;
    int seconds = 0;
    CheckReceiveMessage(receiveBuf, seconds);


    const size_t sendBufSize = 1024;
    char sendBuf[sendBufSize];
    string str = to_string(seconds);
    str += " milliseconds";
    strcpy(sendBuf, str.c_str());

    int sendResult = sendto(udpSocket, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&senderAddr, senderAddrSize);
    if (sendResult == SOCKET_ERROR)
    {
        cout << "sendto failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    Sleep(seconds);
    char sendBuf2[] = "Your order is ready!";
    int sendResult2 = sendto(udpSocket, sendBuf2, strlen(sendBuf2), 0, (SOCKADDR*)&senderAddr, senderAddrSize);
    if (sendResult == SOCKET_ERROR) {
        cout << "sendto failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    closesocket(udpSocket);
    WSACleanup();
}
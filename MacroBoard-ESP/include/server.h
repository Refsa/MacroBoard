#ifndef SERVER_H
#define SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>

#define BUFFER_LEN 16384

void setup_server(uint16_t port, AcConnectHandler on_connection);

#endif
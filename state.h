/*
 * state.h — Objetos e variáveis globais de tempo de execução.
 * Incluído apenas pelo gizmo.ino (projeto = 1 unidade de compilação),
 * então definir os globais aqui é seguro e evita 'extern' espalhado.
 */
#ifndef GIZMO_STATE_H
#define GIZMO_STATE_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <EEPROM.h>
#include "config.h"

// ---------- Serviços ----------
ESP8266WebServer server(80);
WiFiUDP           ntpUDP;
// timeClient mantido em UTC (offset 0); a conversão p/ local usa settings.tzOffset.
NTPClient         timeClient(ntpUDP, "pool.ntp.org", 0, NTP_INTERVAL_MS);

// ---------- Configuração persistida ----------
Settings settings;

// ---------- Estado do relé / motor ----------
bool          motorRunning = false;
unsigned long motorStartMs = 0;
unsigned long motorRunMs   = PORTION_MS_DEFAULT;

// ---------- Relógio manual (fallback sem NTP) ----------
int           manualHour    = 0;
int           manualMinute  = 0;
unsigned long manualBaseMs  = 0;
bool          useManualTime = false;
int           lastCheckedMinute = -1;

// ---------- Wi-Fi ----------
unsigned long lastWifiTry = 0;

// ---------- Histórico (RAM) ----------
FeedEvent feedHistory[FEED_HISTORY];
uint8_t   feedHistoryCount = 0;

#endif // GIZMO_STATE_H

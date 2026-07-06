/*
 * config.h — Parâmetros fixos, mapa de pinos e tipos de dados do GIZMO.
 * Nada aqui guarda estado em tempo de execução (isso fica em state.h).
 */
#ifndef GIZMO_CONFIG_H
#define GIZMO_CONFIG_H

#include <Arduino.h>

// ---------- Identidade / Access Point padrão ----------
#define AP_SSID_DEFAULT     "GIZMO-Feeder"
#define AP_PASS_DEFAULT     "gizmo1234"
#define MDNS_HOST           "gizmo"          // vira http://gizmo.local

// ---------- Hardware do relé ----------
// Agora usando um MÓDULO relé de 1 canal pronto (SRD-05VDC-SL-C + driver na
// plaquinha). A maioria desses módulos é ACIONADA EM NÍVEL BAIXO (active-LOW):
// IN em LOW liga a bobina, IN em HIGH desliga.
// Se o SEU módulo for active-HIGH (relé liga com IN em HIGH), basta trocar para
//   #define RELAY_ON HIGH / #define RELAY_OFF LOW   (ver teste no README/comentário).
#define RELAY_PIN           D5               // GPIO14 (pino "limpo", sem função de boot)
#define RELAY_ON            LOW
#define RELAY_OFF           HIGH

// ---------- Tempos ----------
#define PORTION_MS_DEFAULT  1300             // duração de 1 porção (ms)
#define PORTION_MS_MIN      500
#define PORTION_MS_MAX      30000
#define TEST_DURATION_MS    1300             // botão "Testar relé"
#define WIFI_RETRY_MS       15000            // intervalo p/ retentar STA
#define NTP_INTERVAL_MS     60000UL
#define TIME_SNAPSHOT_MS    900000UL         // salva hora na EEPROM a cada 15 min

// ---------- Limites ----------
#define MAX_SCHEDULES       8                // horários por perfil
#define MAX_PROFILES        4                // pets cadastrados
#define FEED_HISTORY        8                // últimas refeições (RAM)

// ---------- EEPROM ----------
#define EEPROM_SIZE         512
#define SETTINGS_MAGIC      0x4749           // "GI"
#define SETTINGS_VERSION    3                // mudou o layout -> EEPROM antiga é descartada

// ---------- Tipos ----------
struct Schedule {
  uint8_t hour;
  uint8_t minute;
  bool    enabled;
};

// Cada pet tem nome, tamanho de porção e seus próprios horários.
struct PetProfile {
  char     name[24];
  uint16_t portionMs;
  uint8_t  numSchedules;
  Schedule schedules[MAX_SCHEDULES];
};

// Bloco único persistido na EEPROM via EEPROM.put/get.
struct Settings {
  uint16_t   magic;
  uint8_t    version;
  char       deviceName[33];
  int8_t     tzOffset;              // offset UTC em horas (Brasília = -3)
  char       wifiSsid[33];          // rede de casa (vazio = só AP)
  char       wifiPass[65];
  uint8_t    numProfiles;
  uint8_t    activeProfile;
  PetProfile profiles[MAX_PROFILES];
  // Instantâneo da hora p/ recuperar após queda de energia (sem RTC).
  uint8_t    snapHour;
  uint8_t    snapMinute;
  uint8_t    snapValid;
};

// Origem de um acionamento, para o histórico.
enum FeedSource { FEED_MANUAL, FEED_SCHEDULE, FEED_TEST };

struct FeedEvent {
  char       when[6];               // "HH:MM"
  FeedSource source;
};

#endif // GIZMO_CONFIG_H

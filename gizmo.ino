/*
 * ============================================================
 *  GIZMO - Alimentador Automatico de Pet
 *  ESP8266 Wemos D1 Mini + rele SRD-05VDC-SL-C
 * ============================================================
 *
 *  ARQUITETURA (arquivos da pasta, todos compilam junto com este .ino):
 *    config.h      -> pinos, tempos, limites e tipos (Settings/PetProfile)
 *    state.h       -> objetos e variaveis globais de runtime
 *    storage.h     -> ler/gravar Settings na EEPROM (struct versionada)
 *    clocktime.h   -> hora local (NTP ou relogio manual)
 *    feeder.h      -> rele/motor, historico, horarios e snapshot de hora
 *    network.h     -> Wi-Fi AP+STA, mDNS e NTP (reconecta sozinho no boot)
 *    webui.h       -> rotas HTTP / API JSON
 *    index_html.h  -> pagina web (PROGMEM)
 *
 *  PERSISTENCIA: tudo que o usuario marca (perfis, horarios, porcao, rede,
 *  fuso) eh gravado na EEPROM a cada alteracao -> sobrevive a queda de energia.
 *  Ao religar, reconecta na ultima rede e o NTP corrige a hora; sem internet,
 *  usa o ultimo snapshot de hora gravado.
 *
 *  Bibliotecas: ESP8266WiFi, ESP8266WebServer, ESP8266mDNS, WiFiUdp,
 *               EEPROM (core do ESP8266) + NTPClient (Library Manager).
 *
 * ------------------------------------------------------------
 *  LIGACAO DO RELE -- usando MODULO rele de 1 canal (driver na plaquinha):
 *    Controle:  VCC -> 5V do Wemos ; GND -> GND ; IN -> D5 (GPIO14)
 *    Potencia:  COM -> +fonte do motor ; NO -> motor(+) ; motor(-) -> -fonte
 *    O motor usa fonte propria; o rele so fecha o contato. Jumper JD-VCC fica.
 *    Polaridade: a maioria desses modulos eh active-LOW (ver RELAY_ON em config.h).
 *  (Sem modulo, com rele "pelado": NPN BC547/2N2222 + 1k na base + diodo 1N4007.)
 * ------------------------------------------------------------
 */

#include "state.h"
#include "storage.h"
#include "feeder.h"
#include "network.h"
#include "webui.h"

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n[GIZMO] Iniciando...");

  // Garante o nível de "desligado" ANTES de virar saída, senão o pino flutua
  // por instantes no boot e um módulo active-LOW dá um pulso indesejado no relé.
  digitalWrite(RELAY_PIN, RELAY_OFF);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);   // rele desligado no boot

  EEPROM.begin(EEPROM_SIZE);
  settingsLoad();
  restoreTimeSnapshot();                // hora aproximada ate o NTP chegar
  Serial.printf("[GIZMO] Disp: %s | Pets: %d | TZ: UTC%d\n",
                settings.deviceName, settings.numProfiles, settings.tzOffset);

  networkBegin();
  registerRoutes();
  server.begin();

  Serial.printf("[GIZMO] Pronto. AP: http://%s  |  http://%s.local\n",
                apIp().c_str(), MDNS_HOST);
}

void loop() {
  server.handleClient();
  networkLoop();
  handleMotorTimer();
  checkSchedules();

  // Snapshot periodico da hora -> recuperacao apos queda de energia.
  static unsigned long lastSnap = 0;
  if (millis() - lastSnap > TIME_SNAPSHOT_MS) {
    lastSnap = millis();
    saveTimeSnapshot();
  }
}

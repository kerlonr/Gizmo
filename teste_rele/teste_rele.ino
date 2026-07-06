/*
 * teste_rele.ino  —  Sketch MÍNIMO para validar o módulo relé do GIZMO.
 *
 * Objetivo: provar (a) que o GPIO comanda o módulo e (b) descobrir a
 * polaridade real (active-LOW x active-HIGH), SEM Wi-Fi, sem web, sem EEPROM.
 *
 * LIGAÇÃO (módulo relé de 1 canal):
 *    VCC -> 5V do Wemos      (NÃO use 3V3)
 *    GND -> GND do Wemos     (terra comum é OBRIGATÓRIO)
 *    IN  -> D5 (GPIO14)      (pino "limpo", sem função de boot)
 *
 * COMO LER O TESTE:
 *    O Serial (115200) imprime o nível que está sendo escrito no pino.
 *    Observe o relé (clique + LED de status) a cada troca:
 *      - Se o relé ACIONA quando escrevo LOW  -> módulo é ACTIVE-LOW
 *      - Se o relé ACIONA quando escrevo HIGH -> módulo é ACTIVE-HIGH
 *    (O LED verde de ALIMENTAÇÃO fica sempre aceso — ignore-o, olhe o clique.)
 */

#define RELAY_PIN  D4     // troque aqui se quiser testar outro pino

void setRelay(int level, const char* rotulo) {
  digitalWrite(RELAY_PIN, level);
  Serial.printf("Escrevendo %-4s no pino  ->  relé deveria estar: %s\n",
                level == HIGH ? "HIGH" : "LOW", rotulo);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n\n=== TESTE RELE GIZMO ===");
  Serial.println("Alternando o pino a cada 2s. Olhe/escute o rele a cada troca.\n");

  // Começa em HIGH antes de virar saída (evita pulso no boot em módulo active-LOW)
  digitalWrite(RELAY_PIN, HIGH);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  // Se o módulo for active-LOW, "Estado A" é que aciona o relé.
  setRelay(LOW,  "Estado A (active-LOW liga AQUI)");
  delay(2000);

  setRelay(HIGH, "Estado B (active-HIGH liga AQUI)");
  delay(2000);
}

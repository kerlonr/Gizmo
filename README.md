<div align="center">

# 🐾 Gizmo

**Alimentador automático para pets, open source, com horários programados e controle pelo Wi-Fi de casa.**

![ESP8266](https://img.shields.io/badge/ESP8266-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-00878F?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)

</div>

---

## Sobre

Com a rotina corrida, o horário da ração do pet varia. Os alimentadores prontos são caros e quase
sempre dependem do aplicativo e da nuvem do fabricante.

O **Gizmo** é um alimentador aberto: você define os horários numa página web servida pelo próprio
aparelho e ele libera a ração sozinho. **Tudo funciona na rede local, sem nuvem e sem aplicativo.**

> Projeto Interdisciplinar III — Engenharia de Computação, SETREM.

## Visão Geral

O projeto permite:

- programar até **8 horários** de alimentação por pet
- cadastrar até **4 pets** (perfis), cada um com seus horários e tamanho de porção
- liberar uma porção na hora pelo botão **Alimentar agora**
- testar o relé direto pela interface
- ver o histórico das últimas refeições
- configurar o Wi-Fi pela própria página (modo Access Point na primeira vez)
- acessar o painel por `http://gizmo.local` (mDNS)
- continuar funcionando sem internet: a hora é salva na EEPROM e restaurada após quedas de energia

## Hardware

| Peça | Função |
|---|---|
| ESP8266 (NodeMCU / Wemos) | Cérebro do aparelho, Wi-Fi e servidor web |
| Módulo relé | Liga o motor do dosador (pino `D5` / GPIO14) |
| Dosador de ração | Libera a porção enquanto o relé está ligado |

## Como Funciona

```text
┌──────────────┐   Wi-Fi    ┌───────────────────────────────┐   relé   ┌──────────┐
│  Navegador   │ ─────────► │ ESP8266                       │ ───────► │ Dosador  │
│ gizmo.local  │ ◄───────── │  servidor web + API JSON      │          └──────────┘
└──────────────┘            │  NTP (hora certa) + EEPROM    │
                            └───────────────────────────────┘
```

1. O ESP8266 conecta no Wi-Fi e pega a hora certa via **NTP**.
2. A cada minuto ele confere os horários do pet ativo.
3. Na hora marcada, aciona o relé pelo tempo de uma porção (padrão **1,3 s**, ajustável de 0,5 s a 30 s).
4. Configurações ficam salvas na **EEPROM**, então nada se perde ao desligar.

## Estrutura

```text
.
|-- gizmo.ino          setup() e loop() principais
|-- config.h           pinos, limites e valores padrão
|-- network.h          Wi-Fi (cliente + Access Point) e reconexão
|-- clocktime.h        relógio via NTP
|-- storage.h          leitura e gravação das configurações na EEPROM
|-- state.h            estado global do aparelho
|-- feeder.h           relé, porções, agenda e histórico
|-- webui.h            rotas HTTP / API JSON
|-- index_html.h       painel web embarcado (HTML, CSS e JS)
`-- teste_rele/        sketch separado para testar só o relé
```

## Rodando

1. Instale a **Arduino IDE** com o pacote de placas **ESP8266**.
2. Instale a biblioteca **NTPClient**.
3. Abra `gizmo.ino`, selecione a sua placa e envie.
4. Na primeira vez, conecte na rede **`GIZMO-Feeder`** (senha `gizmo1234`) e configure o seu Wi-Fi.
5. Depois acesse **http://gizmo.local** na mesma rede.

> Se o relé ligar ao contrário (ligado quando deveria estar desligado), inverta `RELAY_ON` e
> `RELAY_OFF` em `config.h`. O sketch `teste_rele/` ajuda a descobrir qual é o caso do seu módulo.

## Próximos Passos

- sensor de nível para avisar quando o reservatório estiver vazio
- notificações no celular a cada refeição

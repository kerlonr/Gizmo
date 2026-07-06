/*
 * storage.h — Persistência das configurações na EEPROM (struct versionada).
 * Tudo o que o usuário marca (perfis, horários, porção, rede, fuso) vive aqui
 * e é regravado a cada alteração, então sobrevive a quedas de energia.
 */
#ifndef GIZMO_STORAGE_H
#define GIZMO_STORAGE_H

#include "state.h"

inline PetProfile& activePet() { return settings.profiles[settings.activeProfile]; }

inline void settingsDefaults() {
  memset(&settings, 0, sizeof(settings));
  settings.magic   = SETTINGS_MAGIC;
  settings.version = SETTINGS_VERSION;
  strcpy(settings.deviceName, "GIZMO");
  settings.tzOffset      = -3;
  settings.wifiSsid[0]   = '\0';
  settings.wifiPass[0]   = '\0';
  settings.numProfiles   = 1;
  settings.activeProfile = 0;
  strcpy(settings.profiles[0].name, "Pet");
  settings.profiles[0].portionMs    = PORTION_MS_DEFAULT;
  settings.profiles[0].numSchedules = 0;
  settings.snapValid = 0;
}

inline void settingsSave() {
  settings.magic   = SETTINGS_MAGIC;
  settings.version = SETTINGS_VERSION;
  EEPROM.put(0, settings);
  EEPROM.commit();
}

inline void settingsLoad() {
  EEPROM.get(0, settings);
  if (settings.magic != SETTINGS_MAGIC || settings.version != SETTINGS_VERSION) {
    settingsDefaults();
    settingsSave();
    return;
  }
  // Saneamento contra valores corrompidos.
  if (settings.tzOffset < -12 || settings.tzOffset > 14) settings.tzOffset = -3;
  if (settings.numProfiles < 1 || settings.numProfiles > MAX_PROFILES) settings.numProfiles = 1;
  if (settings.activeProfile >= settings.numProfiles) settings.activeProfile = 0;
  settings.deviceName[32] = '\0';
  settings.wifiSsid[32]   = '\0';
  settings.wifiPass[64]   = '\0';
  for (int i = 0; i < settings.numProfiles; i++) {
    settings.profiles[i].name[sizeof(settings.profiles[i].name) - 1] = '\0';
    if (settings.profiles[i].numSchedules > MAX_SCHEDULES) settings.profiles[i].numSchedules = 0;
    if (settings.profiles[i].portionMs < PORTION_MS_MIN || settings.profiles[i].portionMs > PORTION_MS_MAX)
      settings.profiles[i].portionMs = PORTION_MS_DEFAULT;
  }
}

#endif // GIZMO_STORAGE_H

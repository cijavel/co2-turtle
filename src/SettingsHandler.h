#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <Preferences.h>
#include <FS.h>
#include <LittleFS.h>

class SettingsHandler {
public:

    static SettingsHandler &getInstance() {
        static SettingsHandler instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.
    }
    void restoreDefaultSettings();
    int getSetting(String settingName);
    void setSettingsOnFirstRun();

private:
    Preferences preferences;
    void loadAllPersistedSettings();
    void persistAllSettings();
};

#endif
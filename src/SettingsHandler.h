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
    void loadDefaultSettings();
    void reset();
    void setSeetingsOnFirstRun();

private:
    Preferences preferences;
};

#endif
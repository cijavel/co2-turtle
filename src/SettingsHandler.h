#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <Preferences.h>
#include <FS.h>
#include <LittleFS.h>

class SettingsHandler
{
public:
	static SettingsHandler &getInstance()
	{
		static SettingsHandler instance; // Guaranteed to be destroyed.
		return instance;				 // Instantiated on first use.
	}
	void restoreDefaultConfiguration();
	int getConfigModul(String settingName);
	void setConfigModul(String settingName, int value);
	String getConfigDevice(String settingName);
	void setConfigDevice(String settingName, String value);
	int getConfigLED(String settingName);
	void setConfigLED(String settingName, int value);
	int getConfigSensor(String settingName);
	void setConfigSensor(String settingName, int value);
	void setSettingsOnFirstRun();
	void validateConfigMaps();
	void printConfigMaps();

private:
	Preferences preferences;
	void loadAllPersistedSettings();
	void persistAllSettings();
};

#endif
#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <Preferences.h>
#include <FS.h>
#include <LittleFS.h>

class ConfigHandler
{
public:
	static ConfigHandler &getInstance()
	{
		static ConfigHandler instance; // Guaranteed to be destroyed.
		return instance;				 // Instantiated on first use.
	}
	void restoreDefaultConfiguration();
	int getConfigSwitch(String settingName);
	void setConfigSwitch(String settingName, int value);
	int getConfigInterval(String settingName);
	void setConfigInterval(String settingName, int value);
	String getConfigDevice(String settingName);
	void setConfigDevice(String settingName, String value);
	int getConfigLED(String settingName);
	void setConfigLED(String settingName, int value);
	int getConfigSensor(String settingName);
	void setConfigSensor(String settingName, int value);
	void setSettingsOnFirstRun();
	void validateConfigMaps();
	void persistAllSettings();


private:
	Preferences preferences;
	void loadAllPersistedSettings();
};

#endif
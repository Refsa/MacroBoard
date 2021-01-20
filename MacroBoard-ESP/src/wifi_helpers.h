#ifndef WIFI_HELPERS_H
#define WIFI_HELPERS_H

#include <WiFi.h>

String translateEncryptionType(wifi_auth_mode_t encryptionType)
{

	switch (encryptionType)
	{
	case (WIFI_AUTH_OPEN):
		return "Open";
	case (WIFI_AUTH_WEP):
		return "WEP";
	case (WIFI_AUTH_WPA_PSK):
		return "WPA_PSK";
	case (WIFI_AUTH_WPA2_PSK):
		return "WPA2_PSK";
	case (WIFI_AUTH_WPA_WPA2_PSK):
		return "WPA_WPA2_PSK";
	case (WIFI_AUTH_WPA2_ENTERPRISE):
		return "WPA2_ENTERPRISE";
	}
}

void scanNetworks()
{

	int numberOfNetworks = WiFi.scanNetworks();

	Serial.print("Number of networks found: ");
	Serial.println(numberOfNetworks);

	for (int i = 0; i < numberOfNetworks; i++)
	{

		Serial.print("Network name: ");
		Serial.println(WiFi.SSID(i));

		Serial.print("Signal strength: ");
		Serial.println(WiFi.RSSI(i));

		Serial.print("MAC address: ");
		Serial.println(WiFi.BSSIDstr(i));

		Serial.print("Encryption type: ");
		String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
		Serial.println(encryptionTypeDescription);
		Serial.println("-----------------------");
	}
}

void tryConnect(const char *ssid, const char *password, uint8_t attempts, uint8_t maxAttempts)
{
	WiFi.begin(ssid, password);

	if (WiFi.waitForConnectResult() != WL_CONNECTED)
	{
		Serial.printf("WiFi Failed!\n");

		if (attempts >= maxAttempts)
		{
			return;
		}
		else
		{
			tryConnect(ssid, password, attempts + 1, maxAttempts);
		}
	}
}


void connectToNetwork(const char *ssid, const char *password, uint8_t maxAttempts = 0)
{
	WiFi.mode(WIFI_STA);
	
	tryConnect(ssid, password, 0, maxAttempts);

	Serial.println("Connected to network");
	Serial.println(WiFi.localIP());
}
#endif
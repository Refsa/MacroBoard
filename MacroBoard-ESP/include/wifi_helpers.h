#ifndef WIFI_HELPERS_H
#define WIFI_HELPERS_H

#include <WiFi.h>

#define WIFI_CONNECT_TIMEOUT 1000
#define WIFI_CONNECT_ATTEMPTS 5

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

bool tryConnect(const char *ssid, const char *password)
{
	size_t start_millis = millis();

	wl_status_t status = WiFi.begin(ssid, password);
	for (;;)
	{
		if ((millis() - start_millis) > WIFI_CONNECT_TIMEOUT)
			break;

		if (WiFi.status() == WL_CONNECTED)
		{
			return true;
		}
	}

	return false;
}

bool connectToNetwork(const char *ssid, const char *password)
{
	WiFi.mode(WIFI_STA);

	bool connected = false;
	for (int i = 0; i < WIFI_CONNECT_ATTEMPTS; i++)
	{
		connected = tryConnect(ssid, password);
		if (connected)
			break;
	}

	Serial.println("Connected to network");
	Serial.println(WiFi.localIP());

	return connected;
}

String get_ip_string()
{
	return WiFi.localIP().toString();
}
#endif
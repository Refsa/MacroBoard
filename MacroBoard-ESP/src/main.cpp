#include <Arduino.h>
#include <buttons.h>
#include <server.h>
#include <wifi_helpers.h>
#include <thread.h>
#include <wifi_data.h>
#include <display.h>

const uint16_t port = 8080;
AsyncClient *target_client = NULL;

void server_send(AsyncClient *client, BfButton *btn, uint8_t row, const BfButton::press_pattern_t &pattern)
{
    char *buf = new char[2];
    buf[0] = btn->getID() + row * 4;
    buf[1] = button_state_id(pattern);

    client->write(buf, 2);
}

void button_callback_tr(BfButton *btn, BfButton::press_pattern_t pattern)
{
    Serial.println("Button Pressed");

    if (target_client == NULL)
    {
        return;
    }

    if (target_client->connected())
    {
        if (target_client->canSend())
        {
            server_send(target_client, btn, 0, pattern);
        }
    }
}

void button_callback_mr(BfButton *btn, BfButton::press_pattern_t pattern)
{
    Serial.println("Button Pressed");

    if (target_client == NULL)
    {
        return;
    }

    if (target_client->connected())
    {
        if (target_client->canSend())
        {
            server_send(target_client, btn, 1, pattern);
        }
    }
}

void button_callback_br(BfButton *btn, BfButton::press_pattern_t pattern)
{
    Serial.println("Button Pressed");

    if (target_client == NULL)
    {
        return;
    }

    if (target_client->connected())
    {
        if (target_client->canSend())
        {
            server_send(target_client, btn, 2, pattern);
        }
    }
}

void on_client_data(void *, AsyncClient *, void *data, size_t len)
{
    char *asChar = (char *)data;

    Serial.println(asChar);
}

void on_client_disconnect(void *, AsyncClient *)
{
    target_client = NULL;
}

void on_client_connected(void *data, AsyncClient *client)
{
    Serial.printf("Client Connected: %f\n", client->localIP().toString());

    String display_data = "Client:" + client->remoteIP().toString();
    display_print(display_data, 0, 20);

    target_client = client;

    client->onData(on_client_data);
    client->onDisconnect(on_client_disconnect);
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    Serial.println("Program Setup");

    setup_display();
    display_print("MacroBoard", 0, 0);

    connectToNetwork(ssid, password);
    display_print("IP: " + get_ip_string(), 0, 10);

    setup_server(port, on_client_connected);
    setup_buttons(button_callback_tr, button_callback_mr, button_callback_br);

    Serial.println("Program Start");
}

void loop()
{
    button_manager_tr.loop();
    button_manager_mr.loop();
    button_manager_br.loop();
}
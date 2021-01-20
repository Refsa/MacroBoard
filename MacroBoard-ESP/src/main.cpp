#include <Arduino.h>
#include <buttons.h>
#include <server.h>
#include <wifi_helpers.h>
#include <thread.h>
#include <wifi_data.h>
#include <display.h>
#include <packet.h>
#include <payloads.h>
#include <buffer.h>
#include <logo.h>

// PACKETS
const char NIL_PACKET_ID = '0';
const char BUTTON_PACKET_ID = '1';
const char STRING_PACKET_ID = '2';

// SERVER
static const uint16_t port = 8080;
AsyncClient *target_client = NULL;

// DISPLAY
DisplayBuffer display_buffer;

void server_send(AsyncClient *client, BfButton *btn, uint8_t row, const BfButton::press_pattern_t &pattern)
{
    if (target_client == NULL)
    {
        return;
    }

    set_dbuffer_line(display_buffer, "Sending Data...", 3);
    display_dbuffer(display_buffer);

    ButtonPayload pld = ButtonPayload(btn->getID() + row * 4, button_state_id(pattern));
    BufData pkt = mk_pkt(pld.to_bytes(), BUTTON_PACKET_ID);

    client->write(pkt.data, pkt.len);

    set_dbuffer_line(display_buffer, "", 3);
    display_dbuffer(display_buffer);
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

void on_client_data(void *, AsyncClient *client, void *data, size_t len)
{
    // char *asChar = (char *)data;
    // Serial.println(asChar);

    if (len <= 0)
        return;

    char *pkt = (char *)data;
    char *pld = pkt + 1;
    BufData buf_data = BufData(len - 1, pld);

    char pkt_id = pkt[0];

    set_dbuffer_line(display_buffer, "Packet (ID: " + String(pkt_id) + "): " + String(len - 1, 4), 2);
    display_dbuffer(display_buffer);

    switch (pkt_id)
    {
    case NIL_PACKET_ID:
        return;
    case BUTTON_PACKET_ID:
        {
            ButtonPayload btn_pld = ButtonPayload(buf_data);
            return;
        }
    case STRING_PACKET_ID:
        {
            StringPayload str_pld = StringPayload(buf_data);

            BufData pkt = mk_pkt(str_pld.to_bytes(), STRING_PACKET_ID);
            client->write(pkt.data, pkt.len);
            return;
        }
    }
}

void on_client_disconnect(void *, AsyncClient *)
{
    target_client = NULL;
    clear_dbuffer(display_buffer, 1, 5);
    set_dbuffer_line(display_buffer, "No Client...", 1);
    display_dbuffer(display_buffer);
}

void on_client_connected(void *data, AsyncClient *client)
{
    // Serial.printf("Client Connected: %f\n", client->localIP().toString());

    String display_data = "Client:" + client->remoteIP().toString();
    set_dbuffer_line(display_buffer, display_data, 1);
    display_dbuffer(display_buffer);

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
    display.drawXBitmap(0, 0, Logo_bits, 128, 64, WHITE);
    display.display();
    delay(1000);

    // set_dbuffer_line(display_buffer, "---- MacroBoard ----", 0);

    connectToNetwork(ssid, password);
    set_dbuffer_line(display_buffer, "IP: " + get_ip_string(), 0);
    set_dbuffer_line(display_buffer, "No Client...", 1);

    setup_server(port, on_client_connected);
    setup_buttons(button_callback_tr, button_callback_mr, button_callback_br);

    Serial.println("Program Start");

    display_dbuffer(display_buffer);
}

void loop()
{
    button_manager_tr.loop();
    button_manager_mr.loop();
    button_manager_br.loop();
}
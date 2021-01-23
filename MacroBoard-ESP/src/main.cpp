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
typedef enum PacketID
{
    NIL = 0,
    BUTTON_PACKET_ID = '1',
    STRING_PACKET_ID = '2',
    BITMAP_PACKET_ID = '3',
    ACK_PACKET_ID = 255,
};

// SERVER
static const uint16_t port = 8080;
AsyncClient *target_client = NULL;
uint32_t expect_ack = 0;
std::vector<ISerializer> outbound;

// DISPLAY
DisplayBuffer display_buffer;
Thread display_refresh;

// Buttons
ButtonMatrix button_matrix;

void server_send(AsyncClient *client, ISerializer &data, const PacketID &packet_id)
{
    if (target_client == NULL)
    {
        return;
    }

    BufDataWriter writer = BufDataWriter();
    expect_ack = mk_pkt(writer, packet_id);
    data.to_bytes(writer);

    BufData pkt = writer.GetData();

    client->write(pkt.data, pkt.len);

    set_dbuffer_line(display_buffer, "Awaiting Response...", 3);
}

void button_callback(uint8_t btn, BfButton::press_pattern_t pattern)
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
            ButtonPayload pld = ButtonPayload(btn, button_state_id(pattern));
            server_send(target_client, pld, PacketID::BUTTON_PACKET_ID);
        }
    }
}

void on_client_data(void *, AsyncClient *client, void *data, size_t len)
{
    if (len <= 0)
        return;

    char *pkt = (char *)data;
    BufData buf_data = BufData(len, pkt);
    BufDataReader buf_reader = BufDataReader(buf_data);

    PacketID pkt_id = (PacketID)buf_reader.ReadChar();
    u32_t uid = buf_reader.ReadU32();

    set_dbuffer_line(display_buffer, "Packet (ID: " + String(pkt_id) + "): " + String(len - 1, 4), 2);

    switch (pkt_id)
    {
    case PacketID::NIL:
    {
        return;
    }
    case PacketID::BUTTON_PACKET_ID:
    {
        ButtonPayload btn_pld = ButtonPayload(buf_reader);
        return;
    }
    case PacketID::STRING_PACKET_ID:
    {
        StringPayload str_pld = StringPayload(buf_reader);

        Serial.printf("Received StringPayload: %s\n", str_pld.msg.c_str());
        server_send(client, str_pld, STRING_PACKET_ID);
        return;
    }
    case PacketID::ACK_PACKET_ID:
    {
        uint32_t uid = buf_reader.ReadU32();

        if (uid == expect_ack)
        {
            set_dbuffer_line(display_buffer, "", 3);
            Serial.printf("ACK Received: %d\n", uid);

            expect_ack = 0;
        }

        return;
    }
    }
}

void on_client_disconnect(void *, AsyncClient *client)
{
    target_client = NULL;
    expect_ack = 0;

    clear_dbuffer(display_buffer, 1, 5);
    set_dbuffer_line(display_buffer, "No Client...", 1);

    Serial.println("Client Disconnected");
}

void on_client_connected(void *data, AsyncClient *client)
{
    // Serial.printf("Client Connected: %f\n", client->localIP().toString());

    String display_data = "Client:" + client->remoteIP().toString();
    set_dbuffer_line(display_buffer, display_data, 1);

    target_client = client;

    client->onData(on_client_data);
    client->onDisconnect(on_client_disconnect);

    StringPayload str_pld = StringPayload("AUTH");
    server_send(client, str_pld, STRING_PACKET_ID);
}

void refresh_display()
{
    display_dbuffer(display_buffer);
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
    display_refresh.setInterval(60);
    display_refresh.onRun(refresh_display);
    delay(1000);

    // set_dbuffer_line(display_buffer, "---- MacroBoard ----", 0);

    connectToNetwork(ssid, password);
    set_dbuffer_line(display_buffer, "IP: " + get_ip_string(), 0);
    set_dbuffer_line(display_buffer, "No Client...", 1);

    setup_server(port, on_client_connected);
    Serial.println("Server Ready");

    button_pressed_callback = button_callback;
    button_matrix.Setup();

    Serial.println("Program Start");

    display_dbuffer(display_buffer);
}

void loop()
{
    if (expect_ack == 0)
    {
        button_matrix.LoopButtons();
    }

    if (display_refresh.shouldRun())
    {
        display_refresh.run();
    }
}
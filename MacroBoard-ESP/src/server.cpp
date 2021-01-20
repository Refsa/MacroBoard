#include <server.h>

AsyncServer tcpServer(8080);

void setup_server(uint16_t port, AcConnectHandler on_connection)
{
    tcpServer = AsyncServer(port);

    tcpServer.onClient(on_connection, 0);
    
    tcpServer.begin();
}

void stop_server()
{
    tcpServer.end();
}
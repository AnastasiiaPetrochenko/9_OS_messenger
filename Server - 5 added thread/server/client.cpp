#include "client.h"

Client::Client()
{
    this->connection = NULL;
}

void Client::CloseSocket()
{
    if (connection != NULL)
    {
        shutdown(connection, SD_BOTH);
        closesocket(connection);
        connection = NULL;
    }
}




#ifndef LEVEN_CONNECTOR_H
#define LEVEN_CONNECTOR_H

#include <leven/InetAddress.h>
#include <leven/noncopyable.h>
#include "Callbacks.h"

namespace leven
{

class InetAdress;

class Connector : noncopyable
{
public:
    Connector(const InetAddress& peer);
    ~Connector();

    void start();

    static int createSocket();

private:
    const InetAddress peer_;
    const int sockfd_;
    bool connected_;
    bool started_;
    NewConnectionCallBack newConnectionCallBack_;
    ErrorCallback errorCallback_;

};


} // namespace leven


#endif //LEVEN_CONNECTOR_H

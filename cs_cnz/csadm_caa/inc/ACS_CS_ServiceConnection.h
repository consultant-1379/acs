//  Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_ServiceConnection_h
#define ACS_CS_ServiceConnection_h 1

#include "ACS_CS_Thread.h"
#include "ACS_CS_Parser.h"
#include "ACS_CS_Event.h"

class ACS_CS_Parser;
class ACS_CS_TCPClient;
class ACS_CS_ServiceHandler;


class ACS_CS_ServiceConnection: public ACS_CS_Thread
{
    public:
        ACS_CS_ServiceConnection(ACS_CS_TCPClient *client, ACS_CS_ServiceHandler *handler,
        		ACS_CS_Protocol::CS_Protocol_Type protocolType);

        virtual ~ACS_CS_ServiceConnection();

        virtual int exec();

        void disconnect();

        bool hasFinished() const;

        virtual void finish();

    private:
        ACS_CS_ServiceConnection(const ACS_CS_ServiceConnection &right);

        ACS_CS_ServiceConnection & operator=(const ACS_CS_ServiceConnection &right);

    private:
        ACS_CS_ServiceHandler *serviceHandler;

        ACS_CS_EventHandle shutdownEvent;

        ACS_CS_EventHandle exitThreadEvent;

        ACS_CS_EventHandle removeClosedConnectionsEvent;

        ACS_CS_TCPClient *tcpClient;

        ACS_CS_Parser *csParser;

        bool finished;

        std::string remoteAddress;

        ACS_CS_Protocol::CS_Protocol_Type protocolType;
};


#endif

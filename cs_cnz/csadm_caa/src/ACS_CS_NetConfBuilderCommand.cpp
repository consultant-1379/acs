/* 
 * File:   ACS_CS_NetConfBuilderCommand.cpp
 * Author: renato
 * 
 * Created on 9 ottobre 2012, 17.15
 */

#include "ACS_CS_NetConfBuilderCommand.h"
#include "ACS_CS_NetConfMacParser.h"
#include "ACS_CS_NetConfCommand.h"

#include "ACS_CS_NetConfRemoteSender.h"


ACS_CS_NetConfBuilderCommand::~ACS_CS_NetConfBuilderCommand() {
}

ACS_CS_NetConfCommandInt * ACS_CS_NetConfBuilderCommand::make(netconfBuilder::builds choice)
{
    
    ACS_CS_NetConfCommandInt *coMac;
    ACS_CS_NetConfParserInt  *parser;
    ACS_CS_NetConfSenderInt  *fakeSend;
    
    coMac=new ACS_CS_NetConfCommand();
    
    switch (choice)
    {
        case netconfBuilder::BLADE:
                
                parser= new ACS_CS_NetConfMacParser();
                fakeSend=new ACS_CS_NetConfRemoteSender();
                coMac->setParser(parser);
                coMac->setSender(fakeSend);

                break;
                
        case netconfBuilder::HARDWARE:
        	break;
//
//                parser= new ACS_CS_NetConfHwParser();
//                fakeSend=new ACS_CS_NetConfRemoteSender();
//                coMac->setParser(parser);
//                coMac->setSender(fakeSend);
//
//                break;
	case netconfBuilder::UDP:
		fakeSend=new ACS_CS_NetConfRemoteSender();
		 coMac->setSender(fakeSend);
		 break;   

    }           
    return coMac;
                
                
}


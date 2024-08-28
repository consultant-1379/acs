

#ifndef ACS_CS_NETCONF_MAC_PARSER_H
#define	ACS_CS_NETCONF_MAC_PARSER_H

#include <iostream>
#include <sstream>
#include "ACS_CS_NetConfParserInt.h"


#include <xercesc/sax/DocumentHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

using namespace xercesc;

using namespace std;

class ACS_CS_NetConfMacParser  : public ACS_CS_NetConfParserInt{

public:
    
    ACS_CS_NetConfMacParser();
    
    virtual ~ACS_CS_NetConfMacParser();
    
    virtual void warning (const SAXParseException &exception);

    //	Metod to handle errors that occured during parsing
    virtual void error (const SAXParseException &exception);


    //	Metod to handle errors that occured during parsing
    virtual void fatalError (const SAXParseException &exception);

    //	Method to handle the start events that are generated for each element when
    //	an XML file is parsed
    
    virtual void characters (const XMLCh* const  chars, const unsigned int length);
    
    //	Method to handle the start events that are generated for each element when
    //	an XML file is parsed
    
    
    virtual void startElement(const XMLCh* const name,AttributeList& attributes);
    
    virtual void getResponse(netConfResponse &res);

private:
    
    void resetFlags();
    
    
    netConfResponse nresponse;
    
    bool fl_shelf;
    bool fl_shelfId;
    bool fl_Slot;
    bool fl_slotId;
    bool fl_Blade;
    bool fl_bladeId;
    bool fl_productName;
    bool fl_productNumber;
    bool fl_productRevisionState;
    bool fl_firstMacAddress;
    bool fl_physicalAddress;
    bool fl_vendorName;
    bool fl_serialNumber;
    bool fl_manufacturingDate;

    bool fl_virtualEquipmentId;
    int tenantCounter;

};

#endif


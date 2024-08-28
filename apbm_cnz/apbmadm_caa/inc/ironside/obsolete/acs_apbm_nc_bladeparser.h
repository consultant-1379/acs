

#ifndef ACS_CS_NETCONF_MAC_PARSER_H
#define	ACS_CS_NETCONF_MAC_PARSER_H

#include <iostream>
#include <sstream>
#include "acs_apbm_nc_parser.h"


#include <xercesc/sax/DocumentHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

using namespace xercesc;

using namespace std;



class acs_apbm_nc_bladeparser  : public acs_apbm_nc_parser{

public:
    
    acs_apbm_nc_bladeparser();
    
    virtual ~acs_apbm_nc_bladeparser();
    
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
    
    virtual void getResponse(nresponse &res);
    
    virtual void endElement(const XMLCh* const name);
    

private:
    
    
    void resetFlags();
    
    bool fl_shelf;
    bool fl_shelfId;
    bool fl_Slot;
    bool fl_slotId;
    bool fl_Blade;
    bool fl_bladeId;

    bool fl_productName;
    bool fl_productNumber;
    bool fl_productRevisionState;
    bool fl_serialNumber;
    bool fl_manufacturingDate;
    bool fl_vendorName;

    bool fl_firstMacAddress;
    

    bool m_name ;
    bool m_rState ;


    bool m_operationalLed;
	bool m_ledStatus;
	bool m_faultLed;
	bool m_statusLed;
	bool m_maintenanceLed;
	bool m_biosRunMode;
	bool m_powerState;
	bool m_fatalEventLog;
        bool m_ledType;
	bool m_ipmidata;
};

#endif


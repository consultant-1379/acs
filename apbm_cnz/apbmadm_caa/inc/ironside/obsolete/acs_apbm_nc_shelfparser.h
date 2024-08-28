#ifndef ACS_CS_NETCONFSHELFPARSER_H
#define	ACS_CS_NETCONFSHELFPARSER_H

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

class acs_apbm_nc_shelfparser  : public acs_apbm_nc_parser{

public:
    
    acs_apbm_nc_shelfparser();
    
    virtual ~acs_apbm_nc_shelfparser();
    
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
    
    
    bool fl_shelfId;
    bool fl_userLabel;
    bool fl_rack;
    bool fl_shelfType;
    bool fl_position;
    bool fl_physicalAddress;
    
};

#endif




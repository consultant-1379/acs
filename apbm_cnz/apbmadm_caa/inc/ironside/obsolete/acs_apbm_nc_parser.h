/* 
 * File:   acs_apbm_netconfMacParser.h
 * Author: renato
 *
 * Created on 9 ottobre 2012, 15.38
 */

#ifndef ACS_CS_NETCONF_PARSER_INT_H
#define	ACS_CS_NETCONF_PARSER_INT_H

#include <xercesc/sax/DocumentHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <map>
#include <string>
#include "acs_apbm_nc_util.h"



using namespace xercesc;



class acs_apbm_nc_parser : virtual  HandlerBase {
public:

    
    acs_apbm_nc_parser()
    {
        
    }

    virtual ~acs_apbm_nc_parser()
    {
        
    }

    //	Metod to handle errors that occured during parsing
    virtual void warning (const SAXParseException &exception)=0;

    //	Metod to handle errors that occured during parsing
    virtual void error (const SAXParseException &exception)=0;


    //	Metod to handle errors that occured during parsing
    virtual void fatalError (const SAXParseException &exception)=0;

    //	Method to handle the start events that are generated for each element when
    //	an XML file is parsed
    
    virtual void characters (const XMLCh* const  chars, const unsigned int length)=0;
    
    //	Method to handle the start events that are generated for each element when
    //	an XML file is parsed
    
    inline void clearResponse() {_response.clear();};
    

    virtual void startElement(const XMLCh* const name,AttributeList& attributes)=0;
          
    virtual void getResponse(nresponse &res)=0;
    
    virtual void endElement(const XMLCh* const name)=0;
    
protected:
    nresponse _response;

};

#endif	/* ACS_CS_NETCONF_PARSER_INT_H */


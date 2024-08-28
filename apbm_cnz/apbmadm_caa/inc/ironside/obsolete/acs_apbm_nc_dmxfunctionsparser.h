/*
 * acs_apbm_nc_dmxfunctionsparser.h
 *
 *  Created on: Dec 11, 2012
 *      Author: qrenbor
 */

#ifndef acs_apbm_netconfDMXFUNCTIONSPARSER_H_
#define acs_apbm_netconfDMXFUNCTIONSPARSER_H_
#include "acs_apbm_nc_parser.h"

class acs_apbm_nc_dmxfunctionsparser : public  acs_apbm_nc_parser {
public:
	acs_apbm_nc_dmxfunctionsparser();
	virtual ~acs_apbm_nc_dmxfunctionsparser();


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

	    bool m_address;
	    bool m_UpgradeJobs;
	    bool m_notStarted;
	    bool m_ongoing;
	    bool m_succeded;
	    bool m_failed;
	    bool m_ipv4Connection_Ipaddr;
	    bool m_ipv4Connection_port;
	    bool m_ipv4Connection_protocol;
	    bool m_controlNetworkId;
	    //static int m_controlNetworkId_counter;

	    std::string state;
	    std::string shelfSlot;

	    bool m_tenantId;
	    int tenantCounter;

	    bool m_controlNetwork;
	    bool m_switchSide;
	    std::string ctrlNetId;
	    std::string switchSide;
};

#endif /* acs_apbm_netconfDMXFUNCTIONSPARSER_H_ */

/*
 * acs_apbm_nc_dmxfunctionsparser.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: qrenbor
 */

#include "acs_apbm_nc_dmxfunctionsparser.h"
#include <sstream>

//int acs_apbm_nc_dmxfunctionsparser::m_controlNetworkId_counter =0;

acs_apbm_nc_dmxfunctionsparser::acs_apbm_nc_dmxfunctionsparser():
			tenantCounter(0)
{
	resetFlags();
}

acs_apbm_nc_dmxfunctionsparser::~acs_apbm_nc_dmxfunctionsparser() {

}


void acs_apbm_nc_dmxfunctionsparser::warning (const SAXParseException &)
{

}

//	Metod to handle errors that occured during parsing
void acs_apbm_nc_dmxfunctionsparser::error (const SAXParseException &)
{

}


//	Metod to handle errors that occured during parsing
void acs_apbm_nc_dmxfunctionsparser::fatalError (const SAXParseException &)
{

}

//	Method to handle the start events that are generated for each element when
//	an XML file is parsed

void acs_apbm_nc_dmxfunctionsparser::characters (const XMLCh* const  chars, const unsigned int length)
{
    char *str = new char [length+1];
    XMLString::transcode(chars, str, length);

    str[length] = 0;
    
    nresponse::iterator it;

    if(m_address)
     {
    	 _response.find("address")->second=std::string("")+str;
     }

    if(m_UpgradeJobs && m_notStarted)
    {
        state="notstarted";
        shelfSlot=str;
        m_notStarted=false;
    }

  	if(m_UpgradeJobs && m_ongoing)
  	{
        state="ongoing";
        shelfSlot=str;

        m_ongoing=false;
  	}

  	if(m_UpgradeJobs && m_succeded)
  	{
        state="succeded";
        shelfSlot=str;

        m_succeded=false;

  	}

  	if(m_UpgradeJobs && m_failed)
  	{
        state="failed";
        shelfSlot=str;

        m_failed=false;

  	}

    
    if(m_ipv4Connection_Ipaddr)
    {
       it = _response.find("ipAddr");
       if (it != _response.end())
           it->second=std::string("")+str;
    }
    
    if(m_ipv4Connection_port)
    {
       it = _response.find("port");
       if (it != _response.end())
           it->second=std::string("")+str;
    }
    if(m_ipv4Connection_protocol)
    {
       it = _response.find("protocol");
       if (it != _response.end())
                it->second=std::string("")+str;
    }
    if(m_controlNetworkId)
    { 
//        if(acs_apbm_nc_dmxfunctionsparser::m_controlNetworkId_counter ==1){
//        it = _response.find("controlNetworkIdLeft");
//        if (it != _response.end())
//                it->second=std::string("")+str;
//        }else if (acs_apbm_nc_dmxfunctionsparser::m_controlNetworkId_counter ==0){
//        it = _response.find("controlNetworkIdRight");
//        if (it != _response.end())
//                it->second=std::string("")+str;
//        }

    	ctrlNetId = std::string("") + str;
    }

    if (m_switchSide)
    {
    	switchSide = std::string("") + str;
    }

    if(m_tenantId)
    {
    	std::stringstream ss;
    	ss << tenantCounter;
    	std::string mapKey = "tenantId_" + ss.str();
    	_response.find(mapKey)->second=std::string("")+str;

    	_response.find("tenantCounter")->second=std::string("")+ss.str();
    }
       

    resetFlags();

    XMLString::release(&str);

}

//	Method to handle the start events that are generated for each element when
//	an XML file is parsed


void acs_apbm_nc_dmxfunctionsparser::startElement(const XMLCh* const name,AttributeList&)
{
    char* elem = XMLString::transcode(name);
    std::string element(elem);

    resetFlags();

    if(element.compare("address")==0)
    {
    	printf("start element address\n");

        std::string pN("address");
        std::string emp("");

        _response.insert(std::make_pair(pN,emp));

        m_address=true;
    }

    if(element.compare("UpgradeJobs")==0)
    {
    	printf("start element UpgradeJobs\n");

        std::string pN("state");
        std::string emp("");
        std::string pN1("shelfSlot");

        _response.insert(std::make_pair(pN,emp));
        _response.insert(std::make_pair(pN1,emp));

        m_UpgradeJobs=true;
    }

    if(element.compare("notStarted")==0)
    {
    	printf("start element notStarted\n");
        m_notStarted=true;
    }

    if(element.compare("ongoing")==0)
    {
    	printf("start element ongoing\n");
        m_ongoing=true;
    }

    if(element.compare("succeded")==0)
    {
    	printf("start element succeded\n");
        m_succeded=true;
    }

    if(element.compare("failed")==0)
    {
    	printf("start element failed\n");
        m_failed=true;
    }
    
    if(element.compare("Ipaddr")==0)
    {
       printf("IpV4Connection Ipaddr\n");
       
       std::string  pN("Ipaddr");
       std::string emp("");
       _response.insert(std::make_pair(pN,emp));
       
       m_ipv4Connection_Ipaddr=true;
    }

    if(element.compare("port")==0)
    {
      printf("IpV4Connection port\n");
      std::string pN("port");
      std::string emp("");
      _response.insert(std::make_pair(pN,emp));
       
      m_ipv4Connection_port=true;
    } 

    if(element.compare("protocol")==0)
    {
      printf("IpV4Connection protocol\n");
      std::string pN("protocol");
      std::string emp("");
      _response.insert(std::make_pair(pN,emp));
      
      m_ipv4Connection_protocol=true;
    }  
    if(element.compare("controlNetworkId")==0)
    {
//       if (acs_apbm_nc_dmxfunctionsparser::m_controlNetworkId_counter==0){
//          std::string pN("controlNetworkIdLeft");
//          std::string emp("");
//          _response.insert(std::make_pair(pN,emp));
//          m_controlNetworkId=true;
//          acs_apbm_nc_dmxfunctionsparser::m_controlNetworkId_counter=acs_apbm_nc_dmxfunctionsparser::m_controlNetworkId_counter+ 1;
//       }else if (acs_apbm_nc_dmxfunctionsparser::m_controlNetworkId_counter ==1){
//          std::string pN("controlNetworkIdRight");
//          std::string emp("");
//          _response.insert(std::make_pair(pN,emp));
//          m_controlNetworkId=true;
//          m_controlNetworkId_counter = 0;
//          acs_apbm_nc_dmxfunctionsparser::m_controlNetworkId_counter=0;
//
//      }

    	m_controlNetworkId = true;

    }

    if(element.compare("ControlNetwork")==0)
    {
    	ctrlNetId.clear();
    	switchSide.clear();
    }

    if(element.compare("switchSide")==0)
    {
    	m_switchSide = true;
    }

    if(element.compare("tenantId")==0)
    {

    	if (tenantCounter == 0)
    	{
    		std::string pN("tenantCounter");
    		std::string emp("");

    		_response.insert(std::make_pair(pN,emp));
    	}

    	std::stringstream ss;
    	ss << ++tenantCounter;

    	std::string pN = "tenantId_" + ss.str();
    	std::string emp("");

    	_response.insert(std::make_pair(pN,emp));


    	m_tenantId=true;

    }
    XMLString::release(&elem);

}

void acs_apbm_nc_dmxfunctionsparser::getResponse(nresponse &res)
{
    res=_response;
}


void acs_apbm_nc_dmxfunctionsparser::endElement(const XMLCh* const name)
{

   char* elem = XMLString::transcode(name);

   std::string element(elem);

   if (element.compare("UpgradeJobs")==0)
   {
	   _response.find("state")->second=state;
	   _response.find("shelfSlot")->second=shelfSlot;

	   m_UpgradeJobs=false;

   }

   if (element.compare("ControlNetwork")==0)
   {
	   std::string key = "ControlNetwork_" + switchSide;
	   nresponse::iterator it;

	   if ((it = _response.find(key)) != _response.end())
	   {
		   //A control network for this side already exists!!! It should not happen
		   it->second = ctrlNetId;
	   }
	   else
	   {
		   _response.insert(std::make_pair(key, ctrlNetId));
	   }
   }

   XMLString::release(&elem);

}

void acs_apbm_nc_dmxfunctionsparser::resetFlags()
{
	m_address=false;
	m_ipv4Connection_Ipaddr=false;
	m_ipv4Connection_port=false;
	m_ipv4Connection_protocol=false;
	m_controlNetworkId=false;
	m_tenantId=false;
	m_switchSide=false;
}

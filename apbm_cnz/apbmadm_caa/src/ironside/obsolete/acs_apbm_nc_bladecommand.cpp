
#include "acs_apbm_nc_bladecommand.h"
#include "acs_apbm_nc_shelfcommand.h"

#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"


#include <stdio.h>
#include <stdlib.h>

typedef std::map <std::string,std::string> nresponse; 


acs_apbm_nc_bladecommand::acs_apbm_nc_bladecommand(char target_a[16], char target_b[16]):
	_parser(0),
	_sender(0),
	_blade_filter(0),
	commandBuilder(target_a, target_b)
{

}

acs_apbm_nc_bladecommand::~acs_apbm_nc_bladecommand() {
    
	if (_parser)
		delete(_parser);

	if (_sender)
		delete(_sender);
        
	if (_blade_filter)
		delete(_blade_filter);
        
}


int acs_apbm_nc_bladecommand::execute()
{
   
   int status=1;
   
   std::string res("");
   int result=0;
   
   if (!_sender || !_parser)
	   return status;


   if(xtype==xmltype::XML_DATA)
   {
        result=_sender->sendNetconfMessage(xml_filter);
   }
   
   if(xtype==xmltype::XML_DATA_ACTION)
   {
       printf("ACTION SENDING with %s \n",xml_filter.c_str());
       result=_sender->sendNetconfAction(xml_filter);
   }

   if(xtype==xmltype::XML_DATA_SET)
   {
       printf("ACTION SENDING with %s \n",xml_filter.c_str());
       result=_sender->sendNetconfDataSet(xml_filter);
   }

   
   if(result == 0)
   {
     res=_sender->getData();
     try
     {
         XMLPlatformUtils::Initialize();
     }
     catch (const XMLException& e)
     {

     }

     SAXParser* parser = new SAXParser;

     parser->setValidationScheme(SAXParser::Val_Auto);
     parser->setDoNamespaces(false);
     parser->setDoSchema(false);
     parser->setValidationSchemaFullChecking(false);

     DocumentHandler* docHandler =  (HandlerBase*)_parser;

     ErrorHandler* errHandler = (ErrorHandler*) docHandler;

     parser->setDocumentHandler(docHandler);
     parser->setErrorHandler(errHandler);

     MemBufInputSource myxml_buf((const XMLByte*)res.c_str(), res.size(), "example",false);

     printf("RESPONSE %s ",res.c_str());

     try
     {
    	 _parser->clearResponse();
    	 parser->parse(myxml_buf);

    	 status = 0;

     }
     catch (const XMLException& toCatch) 
     {
    	 message_error="Parse Error";
    	 message_error_type=0;

    	 char* message = XMLString::transcode(toCatch.getMessage());
    	 cout << "Exception message is: \n"
    			 << message << "\n";
    	 XMLString::release(&message);
     }
     catch (const SAXParseException& toCatch) 
     {

    	 message_error="Parse Error";
    	 message_error_type=0;

    	 char* message = XMLString::transcode(toCatch.getMessage());
    	 cout << "Exception message is: \n"
    			 << message << "\n";
    	 XMLString::release(&message);
     }
     catch (...) 
     {
    	 message_error="Parse Error";
    	 message_error_type=0;

    	 cout << "Unexpected Exception \n" ;
     }

     delete parser;

   }
   else
   {
	   message_error=_sender->getErrorMessage();
	   message_error_type=_sender->getErrorType();
   }

   return status;

}


void acs_apbm_nc_bladecommand::setParser(acs_apbm_nc_parser* new_par)
{
    if (_parser)
    {
    	delete _parser;
    	_parser = 0;
    }
	_parser = new_par;
}


void acs_apbm_nc_bladecommand::getResult(nresponse &ret)
{

	if (_parser)
		_parser->getResponse(ret);
}


void acs_apbm_nc_bladecommand::setSender(acs_apbm_nc_sender* send)
{
	if (_sender)
	{
		delete _sender;
		_sender = 0;
	}
    _sender = send;
}

 void acs_apbm_nc_bladecommand::setXmlFilter(std::string filter)
 {
    xml_filter=filter;
 }
 
  void acs_apbm_nc_bladecommand::setFilter(acs_apbm_nc_filter * filter)
 {
	  if (_blade_filter)
	  {
		  delete _blade_filter;
		  _blade_filter = 0;
	  }

	  _blade_filter = (acs_apbm_nc_bladefilter*)filter;
 }
 
const char *acs_apbm_nc_bladecommand::getErrorMessage()
{
    return message_error;
}

int acs_apbm_nc_bladecommand::getErrorType()
{
    return message_error_type;
}


int acs_apbm_nc_bladecommand::getErrorSeverity()
{
    return message_error_severity;
}

const char* acs_apbm_nc_bladecommand::getErrorMessageInfo()
{
    return message_error_info;
}

 int acs_apbm_nc_bladecommand::setBladeQuery(std::string tenantId, std::string physicalAddress,std::string shelf_id, std::string slot_id)
 {
     int res=0;
     
     xtype = xmltype::XML_DATA;
     
     if (shelf_id.empty())
     {
        shelf_id = getShelfidFromPhysicalAddressLoop(physicalAddress);
     }  
     
     if (_blade_filter)
    	 res = _blade_filter->setBladeQuery(tenantId, physicalAddress,shelf_id,slot_id,xml_filter);
     else
    	 res = 1;
     
     return res;
 }
 


 int acs_apbm_nc_bladecommand::setFirmwareDataActQuery(std::string tenantId, std::string physicalAddress, std::string shelf_id, std::string slot_id)
 {
     xtype  = xmltype::XML_DATA;
     
     if (shelf_id.empty())
     {
    	 shelf_id = getShelfidFromPhysicalAddressLoop(physicalAddress);
     }

     int res=0;
     
     if(!shelf_id.empty() && _blade_filter)
     {
        res = _blade_filter->makeXmlgetFirmwareDataAct(tenantId,physicalAddress,shelf_id,slot_id,xml_filter);
     }
     else
    	 res = 1;
     
     return res;
     
 }

int acs_apbm_nc_bladecommand::get_blade_led_type(std::string tenantId, std::string physicalAddress,std::string shelf_id,std::string slot_id)
 {
     xtype=xmltype::XML_DATA;

     int res;

     if (_blade_filter)
         res = _blade_filter->get_blade_led_type(tenantId, physicalAddress,shelf_id,slot_id,xml_filter);
     else
         res = 1;

     return res;

 }


 int acs_apbm_nc_bladecommand::setXmlGetLedInfo(std::string tenantId, std::string physicalAddress,std::string shelf_id,std::string slot_id)
 {
     xtype=xmltype::XML_DATA;

     int res;

     if (_blade_filter)
    	 res = _blade_filter->setXmlGetLedInfo(tenantId, physicalAddress,shelf_id,slot_id,xml_filter);
     else
    	 res = 1;

     return res;

 }

 
 std::string acs_apbm_nc_bladecommand::getShelfidFromPhysicalAddress(std::string physicalAddress)
 {
	 acs_apbm_nc_shelfcommand *comShelf=(acs_apbm_nc_shelfcommand*)commandBuilder.make(netconfBuilder::SHELF);

	 comShelf->createShelfQuery("","","","","",physicalAddress);

	 nresponse respon;

	 std::string shelf_id("");

	 int res=comShelf->execute();

	 if(res==0)
	 {
		 comShelf->getResult(respon);

		 if(respon.find("shelfId")!= respon.end())
		 {
			 shelf_id=respon.find("shelfId")->second;
		 }
	 }


	 delete(comShelf);

	 return shelf_id;
     
 }

 std::string acs_apbm_nc_bladecommand::getShelfidFromPhysicalAddressLoop(std::string physicalAddress)
  {
	 acs_apbm_nc_shelfcommand *comShelf=(acs_apbm_nc_shelfcommand*)commandBuilder.make(netconfBuilder::SHELF);

	 std::string shelf_id("");

	 int res =0;
	 nresponse response;
	 std::string shelfId_str;
	 char intshelfid[4] = {0};
	 bool found = false;

	 for (int shelfId_it = 0; shelfId_it <= 15 && !found; shelfId_it++)
	 {
		 sprintf(intshelfid,"%d",shelfId_it);
		 shelfId_str = std::string(intshelfid);
		 comShelf->createShelfQuery(shelfId_str,"","","","","");
		 res = comShelf->execute();

		 if(res == 0)
		 {
			 comShelf->getResult(response);

			 if(response.find("physicalAddress")!= response.end())
			 {
				 string phyAdd = response.find("physicalAddress")->second;

				 if(phyAdd.compare(physicalAddress.c_str())==0)
				 {
					 shelf_id = shelfId_str;
					 found = true;
				 }
			 }
		 }
	 }

	 delete(comShelf);

	 return shelf_id;

  }

std::string acs_apbm_nc_bladecommand::getXml()
{
    return xml_filter;
}
        

int acs_apbm_nc_bladecommand::setXmlBiosRunMode(std::string tenantId, std::string physicalAddress,std::string shelf_id,std::string slot_id)
 {
     xtype=xmltype::XML_DATA;

     int res;

     if (_blade_filter)
    	 res = _blade_filter->setXmlBiosRunMode(tenantId, physicalAddress,shelf_id,slot_id,xml_filter);
     else
    	 res = 1;

     return res;

 }

int acs_apbm_nc_bladecommand::setXmlPowerState(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string pwr_status)
{

    xtype=xmltype::XML_DATA_SET;

    int res;

    if(shelf_id.empty())
    {
    	shelf_id = getShelfidFromPhysicalAddressLoop(physicAdd);
    }

    if(!shelf_id.empty() && _blade_filter)
    {
    	res = _blade_filter->setXmlPowerState(tenantId, "",shelf_id,slot_id,pwr_status,xml_filter);
    	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "%s", xml_filter.c_str());
    }
    else
    {
    	res=1;
    }
    return res;

}


int acs_apbm_nc_bladecommand::getXmlPowerState(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id)
{

    xtype=xmltype::XML_DATA;

    int res;

    if (_blade_filter)
    	res=_blade_filter->getXmlPowerState(tenantId, physicAdd,shelf_id,slot_id,xml_filter);
    else
    	res = 1;

    return res;

}


int acs_apbm_nc_bladecommand::setBladeREDLed(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_status)
{
    xtype=xmltype::XML_DATA_SET;

    int res;

    if(shelf_id.empty())
    {
        shelf_id = getShelfidFromPhysicalAddressLoop(physicAdd);
    }

    if(!shelf_id.empty() && _blade_filter)
    {
        res = _blade_filter->setBladeREDLed(tenantId, "",shelf_id,slot_id,led_status,xml_filter);
    }
    else
    {
        res=1;
    }
    return res;
}

int acs_apbm_nc_bladecommand::setBladeBlueLed(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_status)
{
    xtype=xmltype::XML_DATA_SET;

    int res;

    if(shelf_id.empty())
    {
        shelf_id = getShelfidFromPhysicalAddressLoop(physicAdd);
    }

    if(!shelf_id.empty() && _blade_filter)
    {
        res = _blade_filter->setBladeBlueLed(tenantId, "",shelf_id,slot_id,led_status,xml_filter);
    }
    else
    {
        res=1;
    }
    return res;
}

int acs_apbm_nc_bladecommand::setBladeMIALed(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_status)
{
    xtype=xmltype::XML_DATA_SET;

    int res;

    if(shelf_id.empty())
    {
        shelf_id = getShelfidFromPhysicalAddressLoop(physicAdd);
    }

    if(!shelf_id.empty() && _blade_filter)
    {
        res = _blade_filter->setBladeMIALed(tenantId, "",shelf_id,slot_id,led_status,xml_filter);
        //ACS_APBM_LOG(LOG_LEVEL_DEBUG, "%s", xml_filter.c_str());
    }
    else
    {
        res=1;
    }
    return res;
}


int acs_apbm_nc_bladecommand::setBladeLedStatus(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_name,std::string led_status)
{
    xtype=xmltype::XML_DATA_SET;

    int res;

    if(shelf_id.empty())
    {
    	shelf_id = getShelfidFromPhysicalAddressLoop(physicAdd);
    }

    if(!shelf_id.empty() && _blade_filter)
    {
    	res = _blade_filter->setBladeLedStatus(tenantId, "",shelf_id,slot_id,led_name,led_status,xml_filter);
    	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "%s", xml_filter.c_str());
    }
    else
    {
    	res=1;
    }
    return res;
}


int acs_apbm_nc_bladecommand::setXmlFatalEventLog(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id)
{
    xtype=xmltype::XML_DATA;

    int res;

    if (_blade_filter)
    	res = _blade_filter->setXmlFatalEventLog(tenantId, physicAdd,shelf_id,slot_id,xml_filter);
    else
    	res = 1;

    return res;

}
int acs_apbm_nc_bladecommand::setAdministrativeStateLocked(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id)
{
    xtype=xmltype::XML_DATA_SET;

    int res = 0;

    if(shelf_id.empty())
    {
    	shelf_id = getShelfidFromPhysicalAddressLoop(physicAdd);
    }

    if(!shelf_id.empty() && _blade_filter)
    {
    	res = _blade_filter->setAdministrativeStateLocked(tenantId, "",shelf_id,slot_id,xml_filter);
    	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "%s", xml_filter.c_str());
    }
    else
    {
    	res=1;
    	//printf("shelf id not found!\n");
    }

    return res;

}

int acs_apbm_nc_bladecommand::setAdministrativeStateUnlocked(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id)
{

    xtype=xmltype::XML_DATA_SET;

    int res=0;


    if(shelf_id.empty())
    {
    	shelf_id = getShelfidFromPhysicalAddressLoop(physicAdd);
    }

    if(!shelf_id.empty() && _blade_filter)
    {
    	res = _blade_filter->setAdministrativeStateUnlocked(tenantId, "",shelf_id,slot_id,xml_filter);
    	//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "%s", xml_filter.c_str());
    }
    else
    {
    	res=1;
    }

    return res;

}

int acs_apbm_nc_bladecommand::setReset(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id)
{

    xtype=xmltype::XML_DATA_ACTION;

    int res=0;


    if(shelf_id.empty())
    {
    	shelf_id = getShelfidFromPhysicalAddressLoop(physicAdd);
    }


    if(!shelf_id.empty() && _blade_filter)
    {
    	res = _blade_filter->setReset(tenantId, "",shelf_id,slot_id,xml_filter);
    }
    else
    {
    	res=1;
    }

    return res;


}

int acs_apbm_nc_bladecommand::getBladeUserLabel(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id)
{
    xtype=xmltype::XML_DATA;

    int res;

    if (_blade_filter)
    	res = _blade_filter->getBladeUserLabel(tenantId, physicAdd,shelf_id,slot_id,xml_filter);
    else
    	res = 1;

    return res;

}


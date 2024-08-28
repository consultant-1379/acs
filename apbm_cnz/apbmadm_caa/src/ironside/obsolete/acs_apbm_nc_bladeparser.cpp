#include "acs_apbm_nc_bladeparser.h"
#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"

#include <stdio.h>


acs_apbm_nc_bladeparser::acs_apbm_nc_bladeparser()
{
	resetFlags();
}


acs_apbm_nc_bladeparser::~acs_apbm_nc_bladeparser()
{
 
}
    

void acs_apbm_nc_bladeparser::warning(const SAXParseException &/*exception*/) {

}

//	Metod to handle errors that occured during parsing

void acs_apbm_nc_bladeparser::error(const SAXParseException &/*exception*/) {

}


//	Metod to handle errors that occured during parsing

void acs_apbm_nc_bladeparser::fatalError(const SAXParseException &/*exception*/) {

}

//	Method to handle the start events that are generated for each element when
//	an XML file is parsed

void acs_apbm_nc_bladeparser::characters(const XMLCh * const chars, const unsigned int length)
{
    if(length < 1)
        return;
    
    char *str = new char [length+1];
    memset(str,0,length+1);
    XMLString::transcode(chars, str, length);
    str[length] = 0;
    
    nresponse::iterator it;


    if(fl_shelf)
    {
        //printf("%s = %s\n","Shelf",str);
        it = _response.find("Shelf");
        if (it != _response.end())
        	it->second=std::string("")+str;
    
    }

    if(fl_shelfId)
    {
        //printf("%s = %s\n","shelfId",str);
        it = _response.find("shelfId");

        if (it != _response.end())
        	it->second=std::string("")+str;
    }
    
    if(fl_Slot)
    {
        //printf("%s = %s\n","Slot",str);
        it = _response.find("Slot");
        if (it != _response.end())
        	it->second=std::string("")+str;
    }
            
    if(fl_slotId)
    {
        //printf("%s = %s\n","slotId",str);
        it = _response.find("slotId");
        if (it != _response.end())
        	it->second=std::string("")+str;
    }

    if(fl_Blade)
    {
        //printf("%s = %s\n","Blade",str);
        it = _response.find("Blade");
        if (it != _response.end())
        	it->second=std::string("")+str;
    }
    
    if(fl_bladeId)
    {
       //printf("valore %s = %s\n","bladeId",str);
        it = _response.find("bladeId");
        if (it != _response.end())
        	it->second=std::string("")+str;
    }
    
    if(fl_productName)
    {
       //printf("%s = %s\n","productName",str);
       it = _response.find("productName");
       if (it != _response.end())
    	   it->second=std::string("")+str;
    }
    
    if(fl_productNumber)
    {
        //printf("%s = %s\n","productNumber",str);

        it = _response.find("productNumber");
        if (it != _response.end())
        	it->second=std::string("")+str;
    }
    
    if(fl_productRevisionState)
    {
        //printf("%s = %s\n","productRevisionState",str);
        it = _response.find("productRevisionState");
        if (it != _response.end())
        	it->second=std::string("")+str;
    }

    if(fl_serialNumber)
    {
    	it = _response.find("serialNumber");
    	if (it != _response.end())
    		it->second=std::string("")+str;
    }

    if(fl_manufacturingDate)
    {
    	it = _response.find("manufacturingDate");
    	if (it != _response.end())
    		it->second=std::string("")+str;
    }

    if(fl_vendorName)
    {

    	it = _response.find("vendorName");
    	if (it != _response.end())
    		it->second = std::string("")+str;
    }
    
    if(fl_firstMacAddress)
    {
        //printf("%s = %s\n","firstMacAddress",str);
        it = _response.find("firstMacAddress");
        if (it != _response.end())
        	it->second=std::string("")+str;
    }
    
     if(m_biosRunMode)
     {
    	 std::string name = XMLString::transcode(chars);
    	 it = _response.find("biosRunMode");
    	 if (it != _response.end())
    		 it->second=std::string("")+str;
     }

     if (m_fatalEventLog)
     {
    	 std::string name = XMLString::transcode(chars);
    	 it = _response.find("fatalEventLog");
    	 if (it != _response.end())
    		 it->second=std::string("")+str;
     }

     if(m_powerState)
     {
    	 std::string name = XMLString::transcode(chars);
    	 it = _response.find("pwr");
    	 if (it != _response.end())
    		 it->second=std::string("")+str;

     }
     if(m_ipmidata)
     {
	std::string name = XMLString::transcode(chars);
         it = _response.find("ipmiData");
         if (it != _response.end())
                 it->second=std::string("")+str;
     }         
     if(m_ledType)
     {
         std::string name = XMLString::transcode(chars);
         it = _response.find("ledType");
         if (it != _response.end())
                 it->second=std::string("")+str;

     }

    if(m_ledStatus)
	{
    	m_ledStatus=false;

    	if(m_maintenanceLed)
    	{
    		it = _response.find("maintenanceLed");
    		if (it != _response.end())
    			it->second=std::string("")+str;
    		m_maintenanceLed=false;
    	}

    	if(m_operationalLed)
    	{
    		it = _response.find("operationalLed");
    		if (it != _response.end())
    			it->second=std::string("")+str;
    		m_operationalLed=false;
    	}

    	if(m_statusLed)
    	{
    		it = _response.find("statusLed");
    		if (it != _response.end())
    			it->second=std::string("")+str;
    		m_statusLed=false;
    	}

    	if(m_faultLed)
    	{
    		it = _response.find("faultLed");
    		if (it != _response.end())
    			it->second=std::string("")+str;
    		m_faultLed=false;
    	}

	}
      

    resetFlags();

    XMLString::release(&str);

//    memset(str,0,length+1);
    //delete(str);
    str=NULL;


}

void acs_apbm_nc_bladeparser::startElement(const XMLCh * const name, AttributeList& ) {

//    std::cout << "I saw element: " << XMLString::transcode(name) << std::endl;

    char* elem = XMLString::transcode(name);
    std::string element(elem);

    resetFlags();
    
    if(element.compare("bladeId")==0)
    {

    	printf("start element bladeId\n");
        
        std::string pN("bladeId");
        std::string emp("");
       
        _response.insert(std::make_pair(pN,emp));
        
        fl_bladeId=true;
     
    }
	
    else if(element.compare("productName")==0)
    {
        fl_productName=true;
        
        std::string pN("productName");
        std::string emp("");
       
        _response.insert(std::make_pair(pN,emp));
    }

    else if(element.compare("pwr")==0)
    {
    	m_powerState=true;

    	std::string pN("pwr");
    	std::string emp("");

    	_response.insert(std::make_pair(pN,emp));
    }


    else if(element.compare("productNumber")==0)
    {
        
        fl_productNumber=true;
     
        std::string pN("productNumber");
        std::string emp("");
       
        _response.insert(std::make_pair(pN,emp));
    }


    else if(element.compare("productRevisionState")==0)
    {
        
        fl_productRevisionState=true;
     
        std::string pN("productRevisionState");
        std::string emp("");
       
        _response.insert(std::make_pair(pN,emp));
    }

    else if(element.compare("serialNumber")==0)
    {
    	fl_serialNumber=true;

    	std::string pN("serialNumber");
    	std::string emp("");

    	_response.insert(std::make_pair(pN,emp));
    }


    else if(element.compare("manufacturingDate")==0)
    {

    	fl_manufacturingDate=true;

    	std::string pN("manufacturingDate");
    	std::string emp("");

    	_response.insert(std::make_pair(pN,emp));
    }


    else if(element.compare("vendorName")==0)
    {

    	fl_vendorName=true;

    	std::string pN("vendorName");
    	std::string emp("");

    	_response.insert(std::make_pair(pN,emp));
    }

    
    else if(element.compare("firstMacAddress")==0)
    {
        fl_firstMacAddress=true;
     
        std::string pN("firstMacAddress");
        std::string emp("");
       
        _response.insert(std::make_pair(pN,emp));
    }

    else if((element.compare("operationalLed")==0))
    {
    	m_operationalLed=true;

    	printf("start element operationalLed\n");
        std::string pN("operationalLed");
        std::string emp("");
        _response.insert(std::make_pair(pN,emp));
    }


    else if((element.compare("maintenanceLed")==0))
    {
    	printf("start element maintenanceLed\n");
    	m_maintenanceLed=true;
        std::string pN("maintenanceLed");
        std::string emp("");
        _response.insert(std::make_pair(pN,emp));
    }


    else if((element.compare("faultLed")==0))
    {
    	printf("start element faultLed\n");
    	m_faultLed=true;
        std::string pN("faultLed");
        std::string emp("");
        _response.insert(std::make_pair(pN,emp));
    }


    else if((element.compare("statusLed")==0))
    {
    	m_statusLed=true;
        std::string pN("statusLed");
        std::string emp("");
        _response.insert(std::make_pair(pN,emp));
    }

    else if((element.compare("status")==0))
    {
    	m_ledStatus=true;
    }

    else if((element.compare("biosRunMode")==0))
    {
    	m_biosRunMode=true;

    	std::string pN("biosRunMode");
    	std::string emp("");

    	_response.insert(std::make_pair(pN,emp));
    }

    else if((element.compare("ipmiData")==0))
    {
        m_ipmidata=true;

        std::string pN("ipmiData");
        std::string emp("");

        _response.insert(std::make_pair(pN,emp));

    }

    else if((element.compare("ledType")==0))
    {
        m_ledType=true;

        std::string pN("ledType");
        std::string emp("");

        _response.insert(std::make_pair(pN,emp));
    }


    else if((element.compare("fatalEventLog")==0))
    {
    	m_fatalEventLog=true;

    	std::string pN("fatalEventLog");
    	std::string emp("");

    	_response.insert(std::make_pair(pN,emp));
	
    }


    // XMLString::release(&elem);
}

 void acs_apbm_nc_bladeparser::getResponse(nresponse &res)
 {
     res = _response;
 }

// Additional Public Declarations


void acs_apbm_nc_bladeparser::resetFlags()
{
     fl_shelf=false;
     fl_shelfId=false;
     fl_Slot=false;
     fl_slotId=false;
     fl_Blade=false;
     fl_bladeId=false;
     fl_productName=false;
     fl_productNumber=false;
     fl_productRevisionState=false;
     fl_serialNumber = false;
     fl_manufacturingDate = false;
     fl_vendorName = false;
     fl_firstMacAddress=false;
     m_biosRunMode=false;
     m_powerState=false;
     m_fatalEventLog = false;
     m_ledType=false;
     m_ipmidata=false;
}


void acs_apbm_nc_bladeparser::endElement(const XMLCh* const name)
{
     char* elem = XMLString::transcode(name);

    std::string element(elem);
    
    if (element == "ipmiData")
    {
  //  	std::cout <<"END elementrev = " << element.c_str() << std::endl;
	m_ipmidata = false;
    }

   /* if (element == "productNumber")
    {
    //	std::cout <<"END elementPr = " << element.c_str() << std::endl;
    	m_IpmiFirmwareDataProductNumber=false;
    }

    if (element == "type")
    {
    //		std::cout <<"END elementty = " << element.c_str() << std::endl;
    	m_IpmiFirmwareDataType=false;

    }

    if (element == "productRevisionState")
    {
    //	std::cout <<"END elementrev = " << element.c_str() << std::endl;
    	m_IpmiFirmwareDataProductRevisionState=false;
    }

    if (element == "version")
    {
    	m_IpmiFirmwareDataVersion=false;
    }

    if (element == "runningVersion")
    {
    	//std::cout <<"END element = " << element.c_str() << std::endl;
        m_IpmiFirmwareDataRunningVersion=false;
    }

*/
        
}

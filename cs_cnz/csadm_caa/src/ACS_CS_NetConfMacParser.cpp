#include "ACS_CS_NetConfMacParser.h"
#include <stdio.h>


ACS_CS_NetConfMacParser::ACS_CS_NetConfMacParser():
tenantCounter(0)
{
     resetFlags();   

}


ACS_CS_NetConfMacParser::~ACS_CS_NetConfMacParser()
{
   for (netConfResponse::iterator ii = nresponse.begin(); ii != nresponse.end(); ++ii)
   {
        nresponse.erase(ii);
   }
 
}
    

void ACS_CS_NetConfMacParser::warning(const SAXParseException &/*exception*/) {

}

//	Metod to handle errors that occured during parsing

void ACS_CS_NetConfMacParser::error(const SAXParseException &/*exception*/) {

}


//	Metod to handle errors that occured during parsing

void ACS_CS_NetConfMacParser::fatalError(const SAXParseException &/*exception*/) {

}

//	Method to handle the start events that are generated for each element when
//	an XML file is parsed

void ACS_CS_NetConfMacParser::characters(const XMLCh * const chars, const unsigned int length) 
{
    
    if(length<1)
        return;
    
    char * str=(char*)malloc(sizeof(int)*(length+1));
    
    memset(str,0,length+1);
    
    for(unsigned int t=0;t<length;t++)
    {
        str[t]=chars[t];
    }
    

    if(fl_shelf)
    {
        //printf("%s = %s\n","Shelf",str);
        nresponse.find("Shelf")->second=std::string("")+str;
    
    }

    if(fl_shelfId)
    {
        //printf("%s = %s\n","shelfId",str);
        nresponse.find("shelfId")->second=std::string("")+str;
    }
    
    if(fl_physicalAddress)
    {
       nresponse.find("physicalAddress")->second=std::string("")+str;
    }
    
    if(fl_Slot)
    {
        //printf("%s = %s\n","Slot",str);
        nresponse.find("Slot")->second=std::string("")+str;  
    }
            
    if(fl_slotId)
    {
        //printf("%s = %s\n","slotId",str);
        nresponse.find("slotId")->second=std::string("")+str;
    }

    if(fl_Blade)
    {
        //printf("%s = %s\n","Blade",str);
        nresponse.find("Blade")->second=std::string("")+str;        
    }
    
    if(fl_bladeId)
    {
       //printf("valore %s = %s\n","bladeId",str);
        nresponse.find("bladeId")->second=std::string("")+str;    
    }
    
    if(fl_productName)
    {
        
       //printf("%s = %s\n","productName",str);
       nresponse.find("productName")->second=std::string("")+str;

        
    }
    
    if(fl_productNumber)
    {
        //printf("%s = %s\n","productNumber",str);

        nresponse.find("productNumber")->second=std::string("")+str;  
    }
    
    if(fl_productRevisionState)
    {
        //printf("%s = %s\n","productRevisionState",str);
        nresponse.find("productRevisionState")->second=std::string("")+str; 
    }
    
    if(fl_firstMacAddress)
    {
        //printf("%s = %s\n","firstMacAddress",str);
        nresponse.find("firstMacAddr")->second=std::string("")+str; 
    }
    


    if(fl_vendorName)
    {
    	nresponse.find("vendorName")->second=std::string("")+str;
    }

    if(fl_serialNumber)
    {
    	nresponse.find("serialNumber")->second=std::string("")+str;
    }

    if(fl_manufacturingDate)
    {
    	nresponse.find("manufacturingDate")->second=std::string("")+str;
    }

    if(fl_virtualEquipmentId)
    {
    	std::stringstream ss;
    	ss << tenantCounter;
    	std::string mapKey = "virtualEquipmentId_" + ss.str();
    	nresponse.find(mapKey)->second=std::string("")+str;

    	nresponse.find("tenantCounter")->second=std::string("")+ss.str();
    }

    resetFlags();

    memset(str,0,length+1);
    free(str);
    str=NULL;
    
    

}

void ACS_CS_NetConfMacParser::startElement(const XMLCh * const name, AttributeList& ) {

   // std::cout << "I saw element: " << XMLString::transcode(name) << std::endl;

    char* elem = XMLString::transcode(name);
    std::string element(elem);

    resetFlags();
    
    if(element.compare("bladeId")==0)
    {
        
        std::string pN("bladeId");
        std::string emp("");
       
        nresponse.insert(std::make_pair(pN,emp));
        
        fl_bladeId=true;
     
    }
    
    if(element.compare("shelfId")==0)
    {
        
        std::string pN("shelfId");
        std::string emp("");
       
        nresponse.insert(std::make_pair(pN,emp));
        
        fl_shelfId=true;
     
    }

    if(element.compare("physicalAddress")==0)
    {
        std::string pN("physicalAddress");
        std::string emp("");

        nresponse.insert(std::make_pair(pN,emp));

        fl_physicalAddress=true;

    }

    if(element.compare("productName")==0)
    {
        fl_productName=true;
        
        std::string pN("productName");
        std::string emp("");
       
        nresponse.insert(std::make_pair(pN,emp));
    }


    if(element.compare("productNumber")==0)
    {
        
        fl_productNumber=true;
     
        std::string pN("productNumber");
        std::string emp("");
       
        nresponse.insert(std::make_pair(pN,emp));
    }


    if(element.compare("productRevisionState")==0)
    {
        
        fl_productRevisionState=true;
     
        std::string pN("productRevisionState");
        std::string emp("");
       
        nresponse.insert(std::make_pair(pN,emp));
    }

    
    if(element.compare("firstMacAddr")==0)
    {
     
        fl_firstMacAddress=true;
     
        std::string pN("firstMacAddr");
        std::string emp("");
       
        nresponse.insert(std::make_pair(pN,emp));
    }

    if(element.compare("vendorName")==0)
    {

    	fl_vendorName=true;

    	std::string pN("vendorName");
    	std::string emp("");

    	nresponse.insert(std::make_pair(pN,emp));
    }

    if(element.compare("serialNumber")==0)
    {

    	fl_serialNumber=true;

    	std::string pN("serialNumber");
    	std::string emp("");

    	nresponse.insert(std::make_pair(pN,emp));
    }

    if(element.compare("manufacturingDate")==0)
    {

    	fl_manufacturingDate=true;

    	std::string pN("manufacturingDate");
    	std::string emp("");

    	nresponse.insert(std::make_pair(pN,emp));
    }

    if(element.compare("virtualEquipmentId")==0)
    {

    	if (tenantCounter == 0)
    	{
    		std::string pN("tenantCounter");
    		std::string emp("");

    		nresponse.insert(std::make_pair(pN,emp));
    	}

    	std::stringstream ss;
    	ss << ++tenantCounter;

    	std::string pN = "virtualEquipmentId_" + ss.str();
    	std::string emp("");

    	nresponse.insert(std::make_pair(pN,emp));


    	fl_virtualEquipmentId=true;

    }

   /* 
    if (element == "WizardOpi") {
        std::cout << "DBG: " << __FUNCTION__ << " " << __LINE__ << std::endl;
        for (XMLSize_t i = 0; i < attributes.getLength(); i++) {
            char* nm = XMLString::transcode(attributes.getName(i));
            std::string name(nm);
            char* vl = XMLString::transcode(attributes.getValue(i));
            std::string value(vl);


            std::cout << "name: " << nm << std::endl;
            std::cout << "value: " << vl << std::endl;

            if (name == "name") {
                if (value == "scxb-kernel") {
                } else {
                    XMLString::release(&vl);
                    XMLString::release(&nm);
                    break;
                }
            } else if (name == "id") {
            } else if (name == "rstate") {
            } else {

            }

            XMLString::release(&vl);
            XMLString::release(&nm);
        }
    } else if (element == "file") {
        std::cout << "DBG: " << __FUNCTION__ << " " << __LINE__ << std::endl;
        for (XMLSize_t i = 0; i < attributes.getLength(); i++) {
            char* nm = XMLString::transcode(attributes.getName(i));
            std::string name(nm);
            char* vl = XMLString::transcode(attributes.getValue(i));
            std::string value(vl);

            if (name == "relpath") {
            } else if (name == "name") {
            } else if (name == "provider_prodnr") {
            } else if (name == "provider_rstate") {
            } else {

            }

            XMLString::release(&vl);
            XMLString::release(&nm);
        }
    }

    XMLString::release(&elem);
*/
     XMLString::release(&elem);
}

 void ACS_CS_NetConfMacParser::getResponse(netConfResponse &res)
 {
     res=nresponse;
 }

// Additional Public Declarations


void ACS_CS_NetConfMacParser::resetFlags()
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
     fl_firstMacAddress=false;      
     fl_physicalAddress=false;
     fl_vendorName = false;
     fl_serialNumber  = false;
     fl_manufacturingDate  = false;
     fl_virtualEquipmentId = false;
     
}


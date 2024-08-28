#include "acs_apbm_netconfparser.h"
#include "acs_apbm_netconfmanager.h"

acs_apbm_netconfparser::acs_apbm_netconfparser():
biosImage(-1),
biosPointer(-1),
tenantCounter(0)
{
	resetFlags();
}

acs_apbm_netconfparser::~acs_apbm_netconfparser()
{

}


void acs_apbm_netconfparser::warning(const SAXParseException &/*exception*/) {

}

//	Metod to handle errors that occured during parsing

void acs_apbm_netconfparser::error(const SAXParseException &/*exception*/) {

}


//	Metod to handle errors that occured during parsing

void acs_apbm_netconfparser::fatalError(const SAXParseException &/*exception*/) {

}

//	Method to handle the start events that are generated for each element when
//	an XML file is parsed

void acs_apbm_netconfparser::characters(const XMLCh * const chars, const unsigned int length)
{
    if(length<1)
        return;

    char * str=(char*)malloc(sizeof(int)*(length+1));

    memset(str,0,length+1);

    for(unsigned int t=0;t<length;t++)
    {
        str[t]=chars[t];
    }



    if(fl_shelfId)
    {
        shelfId = str;
    }

    if(fl_physicalAddress)
    {
    	physicaladdress = str;
    }

    if(fl_biosImage)
    {

        if(strcmp(str,"BIOS2") == 0)
        {
          biosImage = 1;
        }
        else if(strcmp(str,"BIOS1") == 0)
        {
          biosImage = 0;
        }
        else
        {
           biosImage = 2;
        }
    }

    if(fl_biosPointer)
    {

        if(strcmp(str,"BIOS2") == 0)
        {
          biosPointer = 1;
        }
        else if(strcmp(str,"BIOS1") == 0)
        {
          biosPointer = 0;
        }
        else
        {
           biosPointer = 2;
        }
    }

    if(fl_tenantId)
    {
    	std::stringstream ss;
    	ss << tenantCounter;
    	std::string mapKey = "tenantId_" + ss.str();
    	_tenantNamelist.find(mapKey)->second=std::string("")+str;

    	_tenantNamelist.find("tenantCounter")->second=std::string("")+ss.str();
    }

    resetFlags();
    memset(str,0,length+1);
    free(str);
    str=NULL;


}

void acs_apbm_netconfparser::startElement(const XMLCh * const name, AttributeList& ) {

    char* elem = XMLString::transcode(name);
    std::string element(elem);

    resetFlags();

    if(element.compare("shelfId")==0)
    {
        fl_shelfId = true;
    }

    if(element.compare("physicalAddress")==0)
    {
        fl_physicalAddress = true;

    }

    if(element.compare("biosDefaultImage") == 0)
    {

    	fl_biosImage = true;
    }


    if(element.compare("biosPointer")==0)
    {
    	fl_biosPointer = true;

    }
    
    if(element.compare("tenantId")==0)
    {

    	if (tenantCounter == 0)
    	{
    		std::string pN("tenantCounter");
    		std::string emp("");

    		_tenantNamelist.insert(std::make_pair(pN,emp));
    	}

    	std::stringstream ss;
    	ss << ++tenantCounter;

    	std::string pN = "tenantId_" + ss.str();
    	std::string emp("");

    	_tenantNamelist.insert(std::make_pair(pN,emp));


    	fl_tenantId=true;

    }

     XMLString::release(&elem);
}


// Additional Public Declarations


void acs_apbm_netconfparser::resetFlags()
{
     fl_shelfId=false;
     fl_physicalAddress=false;
     fl_biosPointer = false;
     fl_biosImage = false; 
     fl_tenantId =false;
}

void acs_apbm_netconfparser::getphysicaladdress(std::string &ret)
{
        ret = physicaladdress;
}

void acs_apbm_netconfparser::endElement(const XMLCh* const)
{

}
int acs_apbm_netconfparser::getBiosDefaultImage()
{
  return biosImage;
}
int acs_apbm_netconfparser::getBiosPointer()
{
  return biosPointer;
}
void acs_apbm_netconfparser::setBiosDefaultImage(int m_biosimage)
{
  biosImage = m_biosimage;
  return ;
}
void acs_apbm_netconfparser::setBiosPointer(int m_biospointer)
{
  biosPointer = m_biospointer;
  return;
}
int acs_apbm_netconfparser::getTenants(tenantNamelist &tenantlist)
{
  tenantlist =_tenantNamelist;
  return 0;
}


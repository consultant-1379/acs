/* 
 * File:   acs_apbm_nc_bladefilter.cpp
 * Author: renato
 * 
 * Created on 10 ottobre 2012, 17.08
 */

#include "acs_apbm_nc_bladefilter.h"
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>
#include <stdio.h>
#include <algorithm>
#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"


acs_apbm_nc_bladefilter::~acs_apbm_nc_bladefilter() {
}

acs_apbm_nc_bladefilter::acs_apbm_nc_bladefilter()
{
    
}
acs_apbm_nc_bladefilter::acs_apbm_nc_bladefilter(std::string mag,int slot)
{
    this->mag=mag;
    this->slot=slot;
}
    
int acs_apbm_nc_bladefilter::setBladeQuery(std::string tenantId, std::string physicalAddress,std::string shelf_id, std::string slot_id, std::string &xml)
{
    try 
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) 
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                << "  Exception message:"
                << pMsg;
        XMLString::release(&pMsg);
        return 1;
    }

    // Watch for special case help request
    int errorCode = 0;

    DOMDocument* doc=0;
  
    DOMElement* element=0;
 
    getBladeStructureData(&doc,&element,tenantId,physicalAddress,shelf_id,slot_id);

               try
               {
                 /* doc =impl->createDocument(0,X("ManagedElement"),0);

		  DOMElement* rootElem = doc->getDocumentElement();

		  rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));

		  DOMElement* equipfElem = doc->createElement(X("Equipment"));

		  rootElem->appendChild(equipfElem);

		  DOMElement* shelf = doc->createElement(X("Shelf"));

		  equipfElem->appendChild(shelf);

		  DOMElement* physicalAddress_tag = doc->createElement(X("physicalAddress"));

		  if(!physicalAddress.empty())
		  {
			  DOMText* physicalAddressText = doc->createTextNode(X(physicalAddress.c_str()));
			  physicalAddress_tag->appendChild(physicalAddressText);
		  }

		  shelf->appendChild(physicalAddress_tag);

		  DOMElement* slot = doc->createElement(X("Slot"));

		  shelf->appendChild(slot);

		  DOMElement* slotId = doc->createElement(X("slotId"));

		  if(!slot_id.empty())
		  {
			  DOMText* slotIdVal = doc->createTextNode(X(slot_id.c_str()));
			  slotId->appendChild(slotIdVal);
		  }

		  slot->appendChild(slotId);

		  DOMElement* bladeElem = doc->createElement(X("Blade"));

		  slot->appendChild(bladeElem);

		  DOMElement* bladeId = doc->createElement(X("bladeId"));
		  DOMText* bladeIdVal = doc->createTextNode(X("1"));
		  bladeId->appendChild(bladeIdVal);

		  bladeElem->appendChild(bladeId);*/
                   
                             
		  DOMElement* productName = doc->createElement(X("productName"));
		  DOMElement* productNumber = doc->createElement(X("productNumber"));
		  DOMElement* productRevisionState = doc->createElement(X("productRevisionState"));
		  DOMElement* serialNumber = doc->createElement(X("serialNumber"));
		  DOMElement* manufacturingDate = doc->createElement(X("manufacturingDate"));
		  DOMElement* vendorName = doc->createElement(X("vendorName"));


		  element->appendChild(productName);
		  element->appendChild(productNumber);
		  element->appendChild(productRevisionState);
		  element->appendChild(serialNumber);
		  element->appendChild(manufacturingDate);
		  element->appendChild(vendorName);

            getXml(doc,xml);
            doc->release();

        } catch (const OutOfMemoryException&)
        {
        	if(doc)
        		doc->release();

            XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
            errorCode = 5;
        } catch (const DOMException& e) {
        	if(doc)
        		doc->release();

            XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
            errorCode = 2;
        } catch (...)
        {
        	if(doc)
        		doc->release();
            XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
            errorCode = 3;
        } 

    XMLPlatformUtils::Terminate();
    return errorCode;
}

int acs_apbm_nc_bladefilter::getXml(DOMDocument* Doc,std::string &xml) {

    XMLCh* gOutputEncoding = 0;

    gOutputEncoding = XMLString::transcode("UTF-8");
    
    std::string xmlStr;

    try {
        // get a serializer, an instance of DOMWriter
        XMLCh tempStr[10000];
        XMLString::transcode("LS", tempStr, 10000);
        DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
        DOMWriter *theSerializer = ((DOMImplementationLS*) impl)->createDOMWriter();

        // set user specified output encoding
        theSerializer->setEncoding(gOutputEncoding);

        XMLCh* out = theSerializer->writeToString(*Doc);

        char *msg = XERCES_CPP_NAMESPACE::XMLString::transcode(out);

        xmlStr=msg;

        int found = xmlStr.find_first_of("?");
        int last = xmlStr.find_first_of("?", found + 1);

        last += 1;
        std::string t = xmlStr.replace(found - 1, last + 1, "");
//        printf("%s\n %d %d\n", t.c_str(), found - 1, last + 1);

        free(out);
        free(msg);
        
        xmlStr=t;
        
        
        delete theSerializer;
    //    delete impl;

    } catch (const OutOfMemoryException&) {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
    } catch (XMLException& e) {
    }
    
    free(gOutputEncoding);
   
    ACS_APBM_LOG(LOG_LEVEL_INFO, "Entering Method %s",	xmlStr.c_str());	 
    xml=xmlStr;

    return 0;
}



int acs_apbm_nc_bladefilter::makeXmlgetFirmwareDataAct(std::string tenantId, std::string physicalAdd, std::string shelf_id,std::string slot_id,std::string &xml)
{
   try 
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) 
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                << "  Exception message:"
                << pMsg;
        XMLString::release(&pMsg);
        return 1;
    }

    // Watch for special case help request
    int errorCode = 0;
    DOMDocument* doc=0;
    DOMElement* bladeElem=0;  
 
    getBladeStructureData(&doc,&bladeElem,tenantId,physicalAdd,shelf_id,slot_id);

        try {
	
	    DOMElement*  ipmiDataElem = doc->createElement(X("ipmiData"));
                        bladeElem->appendChild(ipmiDataElem);

            getXml(doc,xml);
            doc->release();

        } catch (const OutOfMemoryException&) {
            XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
            errorCode = 5;
        } catch (const DOMException& e) {
            XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
            errorCode = 2;
        } catch (...) {
            XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
            errorCode = 3;
        }

    XMLPlatformUtils::Terminate();
    return errorCode;    
}

int acs_apbm_nc_bladefilter::get_blade_led_type(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml)
{
        try
        {
                XMLPlatformUtils::Initialize();
        }
        catch (const XMLException& toCatch)
        {
                char *pMsg = XMLString::transcode(toCatch.getMessage());
                XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                                << "  Exception message:"
                                << pMsg;
                XMLString::release(&pMsg);
                return 1;
        }

        // Watch for special case help request
        int errorCode =0;
        DOMDocument* doc=0;
        DOMElement* bladeElem=0;

        getBladeStructureData(&doc,&bladeElem,tenantId, physicAdd,shelf_id,slot_id);

               try{

                        DOMElement* ledTypeElem = doc->createElement(X("ledType"));
                        bladeElem->appendChild(ledTypeElem);

                        getXml(doc,xml);

                        doc->release();

                } catch (const OutOfMemoryException&)
                {
                        xml="";
                        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
                        errorCode = 5;

                        if(doc)
                                doc->release();

                } catch (const DOMException& e)
                {
                        xml="";
                        if(doc)
                                doc->release();

                        XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
                        errorCode = 2;

                } catch (...)
                {
                        xml="";

                        if(doc)
                                doc->release();
                        XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
                        errorCode = 3;
                }


        XMLPlatformUtils::Terminate();
        return errorCode;

}

int acs_apbm_nc_bladefilter::setXmlGetLedInfo(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml)
{
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch)
	{
		char *pMsg = XMLString::transcode(toCatch.getMessage());
		XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
				<< "  Exception message:"
				<< pMsg;
		XMLString::release(&pMsg);
		return 1;
	}

	// Watch for special case help request
        int errorCode =0; 
	DOMDocument* doc=0;
        DOMElement* bladeElem=0;

        getBladeStructureData(&doc,&bladeElem,tenantId,physicAdd,shelf_id,slot_id);

               try{

			DOMElement* operationalLed = doc->createElement(X("operationalLed"));
			bladeElem->appendChild(operationalLed);

			DOMElement* operledStatus = doc->createElement(X("status"));

			operationalLed->appendChild(operledStatus);

			DOMElement* faultLed = doc->createElement(X("faultLed"));
			bladeElem->appendChild(faultLed);

			DOMElement* faultledStatus = doc->createElement(X("status"));
			faultLed->appendChild(faultledStatus);

			DOMElement* statusLed = doc->createElement(X("statusLed"));
			bladeElem->appendChild(statusLed);

			DOMElement* statusledStatus = doc->createElement(X("status"));
			statusLed->appendChild(statusledStatus);

			DOMElement* maintenanceLed = doc->createElement(X("maintenanceLed"));
			bladeElem->appendChild(maintenanceLed);

			DOMElement* maintenanceledStatus = doc->createElement(X("status"));
			maintenanceLed->appendChild(maintenanceledStatus);

			getXml(doc,xml);

			doc->release();

		} catch (const OutOfMemoryException&)
		{
			xml="";
			XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
			errorCode = 5;

			if(doc)
				doc->release();

		} catch (const DOMException& e)
		{
			xml="";
			if(doc)
				doc->release();

			XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
			errorCode = 2;

		} catch (...)
		{
			xml="";

			if(doc)
				doc->release();
			XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
			errorCode = 3;
		}
/*	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}*/

	XMLPlatformUtils::Terminate();
	return errorCode;

}
/*
<Equipment>
               <Shelf>
                   <physicalAddress>1057</physicalAddress>
                   <Slot>
                       <slotId>14</slotId>
                       <Blade>
                           <bladeId>1</bladeId>
							<biosRunMode/>
                       </Blade>
                   </Slot>
               </Shelf>
           </Equipment>

*/




int acs_apbm_nc_bladefilter::setXmlBiosRunMode(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml)
{


	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch)
	{
		char *pMsg = XMLString::transcode(toCatch.getMessage());
		XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
				<< "  Exception message:"
				<< pMsg;
		XMLString::release(&pMsg);
		return 1;
	}

	int errorCode = 0;

	DOMDocument *doc=0;
	DOMElement *element=0;

	getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);

	try
	{
		DOMElement* biosRunModeElem = ((DOMDocument*)doc)->createElement(X("biosRunMode"));

		element->appendChild(biosRunModeElem);

		getXml(doc,xml);

	}
	catch (const XMLException& toCatch)
	{
		char *pMsg = XMLString::transcode(toCatch.getMessage());

		XMLString::release(&pMsg);

		return 1;
	}

	doc->release();

	XMLPlatformUtils::Terminate();


	return errorCode;



}


int acs_apbm_nc_bladefilter::setXmlPowerState(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string powerStateVal,std::string &xml)
{

	try
		{
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& toCatch)
		{
			char *pMsg = XMLString::transcode(toCatch.getMessage());
			XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
					<< "  Exception message:"
					<< pMsg;
			XMLString::release(&pMsg);
			return 1;
		}

	int errorCode = 0;

	DOMDocument *doc=0;
	DOMElement *element=0;

	getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);

	try
	{
		DOMElement* powerState = ((DOMDocument*)doc)->createElement(X("pwr"));

		element->appendChild(powerState);
              
		DOMText* pwrStateVal=0; 

		pwrStateVal =((DOMDocument*)doc)->createTextNode(X(powerStateVal.c_str()));

		powerState->appendChild(pwrStateVal);

		getXml(doc,xml);

	}
	catch (const XMLException& toCatch)
	{
		char *pMsg = XMLString::transcode(toCatch.getMessage());

		XMLString::release(&pMsg);

		return 1;
	}

	doc->release();

	XMLPlatformUtils::Terminate();


	return errorCode;

}


int acs_apbm_nc_bladefilter::getXmlPowerState(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml)
{

	try
		{
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& toCatch)
		{
			char *pMsg = XMLString::transcode(toCatch.getMessage());
			XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
					<< "  Exception message:"
					<< pMsg;
			XMLString::release(&pMsg);
			return 1;
		}

	int errorCode = 0;

	DOMDocument *doc=0;
	DOMElement *element=0;

	getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);

	try
	{
		DOMElement* powerState = ((DOMDocument*)doc)->createElement(X("pwr"));

		element->appendChild(powerState);
		getXml(doc,xml);

	}
	catch (const XMLException& toCatch)
	{
		char *pMsg = XMLString::transcode(toCatch.getMessage());

		XMLString::release(&pMsg);

		return 1;
	}

	doc->release();

	XMLPlatformUtils::Terminate();


	return errorCode;


}

int acs_apbm_nc_bladefilter::getBladeStructureData(DOMDocument **docOut,DOMElement **element, std::string tenantId, std::string /*physicAdd*/,std::string shelf_id,std::string slot_id)
{

	int errorCode = 0;

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	if (impl != NULL)
	{

		try
		{
			//#### Managed Element ####

			(*docOut)=impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = (*docOut)->getDocumentElement();

			DOMElement* managedElementIdElem = (*docOut)->createElement(X("managedElementId"));

			DOMText* managedElementIdVal = (*docOut)->createTextNode(X(std::string("1").c_str()));

			managedElementIdElem->appendChild(managedElementIdVal);

			rootElem->appendChild(managedElementIdElem);

			//#### DmxcFunctions ####

			DOMElement* DmxcFunctionElem = (*docOut)->createElement(X("DmxcFunction"));

			rootElem->appendChild(DmxcFunctionElem);

			DOMElement* dmxcFunctionIdElem = (*docOut)->createElement(X("dmxcFunctionId"));

			DOMText* dmxcFunctionIdVal = (*docOut)->createTextNode(X(std::string("1").c_str()));

			dmxcFunctionIdElem->appendChild(dmxcFunctionIdVal);

			DmxcFunctionElem->appendChild(dmxcFunctionIdElem);

			//#### Eqm ####

			DOMElement* EqmElem = (*docOut)->createElement(X("Eqm"));

			DmxcFunctionElem->appendChild(EqmElem);

			DOMElement* eqmIdElem = (*docOut)->createElement(X("eqmId"));

			DOMText* eqmIdVal = (*docOut)->createTextNode(X(std::string("1").c_str()));

			eqmIdElem->appendChild(eqmIdVal);

			EqmElem->appendChild(eqmIdElem);

			//#### VirtualEquipment ####

			DOMElement* VirtualEquipmentElem = (*docOut)->createElement(X("VirtualEquipment"));

			EqmElem->appendChild(VirtualEquipmentElem);

			DOMElement* virtualEquipmentIdElem = (*docOut)->createElement(X("virtualEquipmentId"));

			//###TODO ##Check value of VirtualEquipmentId with Actual BSP ##

			DOMText* virtualEquipmentIdVal = (*docOut)->createTextNode(X(tenantId.c_str()));

			virtualEquipmentIdElem->appendChild(virtualEquipmentIdVal);

			VirtualEquipmentElem->appendChild(virtualEquipmentIdElem);

			// #### Blade ####

			DOMElement* BladeElem =(*docOut)->createElement(X("Blade"));

			VirtualEquipmentElem->appendChild(BladeElem);

			DOMElement* BladeIdElem = (*docOut)->createElement(X("bladeId"));

			if((!shelf_id.empty()) && (!slot_id.empty()))
			{
				std::string bladeId_str = shelf_id +std::string("-")+slot_id;
				DOMText* BladeIdText = (*docOut)->createTextNode(X(bladeId_str.c_str()));
				BladeIdElem->appendChild(BladeIdText);
				BladeElem->appendChild(BladeIdElem);
			}



			*element=BladeElem;


		} catch (const OutOfMemoryException&)
		{
			XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
			errorCode = 5;

			if(*docOut)
			{
				(*docOut)->release();
				*docOut=0;
			}


		} catch (const DOMException& e)
		{
			if(*docOut)
			{
				(*docOut)->release();
				*docOut=0;
			}


			XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
			errorCode = 2;

		} catch (...)
		{

			if(*docOut)
			{
				(*docOut)->release();
				*docOut=0;
			}
			XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
			errorCode = 3;
		}
	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}

	//XMLPlatformUtils::Terminate();
	return errorCode;

}


int acs_apbm_nc_bladefilter::setXmlFatalEventLog(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml)
{

	try
			{
				XMLPlatformUtils::Initialize();
			}
			catch (const XMLException& toCatch)
			{
				char *pMsg = XMLString::transcode(toCatch.getMessage());
				XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
						<< "  Exception message:"
						<< pMsg;
				XMLString::release(&pMsg);
				return 1;
			}

		int errorCode = 0;

		DOMDocument *doc=0;
		DOMElement *element=0;

		getBladeStructureData(&doc,&element, tenantId, physicAdd,shelf_id,slot_id);

		try
		{
			DOMElement* fatalEventLog = ((DOMDocument*)doc)->createElement(X("fatalEventLogInfo"));

			element->appendChild(fatalEventLog);

			getXml(doc,xml);

		}
		catch (const XMLException& toCatch)
		{
			char *pMsg = XMLString::transcode(toCatch.getMessage());

			XMLString::release(&pMsg);

			return 1;
		}

		doc->release();

		XMLPlatformUtils::Terminate();


		return errorCode;


}
int acs_apbm_nc_bladefilter::setAdministrativeStateLocked(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml)
{

	try
			{
				XMLPlatformUtils::Initialize();
			}
			catch (const XMLException& toCatch)
			{
				char *pMsg = XMLString::transcode(toCatch.getMessage());
				XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
						<< "  Exception message:"
						<< pMsg;
				XMLString::release(&pMsg);
				return 1;
			}

		int errorCode = 0;

		DOMDocument *doc=0;
		DOMElement *element=0;



		getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);

		try
		{
			DOMElement* administrativeState = ((DOMDocument*)doc)->createElement(X("administrativeState"));

			element->appendChild(administrativeState);

			DOMText* administrativeStateText = ((DOMDocument*)doc)->createTextNode(X("locked"));

			administrativeState->appendChild(administrativeStateText);

			getXml(doc,xml);

		}
		catch (const XMLException& toCatch)
		{
			char *pMsg = XMLString::transcode(toCatch.getMessage());

			XMLString::release(&pMsg);

			return 1;
		}

		doc->release();

		XMLPlatformUtils::Terminate();


		return errorCode;



}

int acs_apbm_nc_bladefilter::setAdministrativeStateUnlocked(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml)
{

	try
			{
				XMLPlatformUtils::Initialize();
			}
			catch (const XMLException& toCatch)
			{
				char *pMsg = XMLString::transcode(toCatch.getMessage());
				XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
						<< "  Exception message:"
						<< pMsg;
				XMLString::release(&pMsg);
				return 1;
                        }

		int errorCode = 0;

		DOMDocument *doc=0;
		DOMElement *element=0;

		getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);

		try
		{
			DOMElement* administrativeState = ((DOMDocument*)doc)->createElement(X("administrativeState"));

			element->appendChild(administrativeState);

			DOMText* administrativeStateText =((DOMDocument*)doc)->createTextNode(X("unlocked"));

			administrativeState->appendChild(administrativeStateText);

			getXml(doc,xml);

		}
		catch (const XMLException& toCatch)
		{
			char *pMsg = XMLString::transcode(toCatch.getMessage());

			XMLString::release(&pMsg);

			return 1;
		}

		doc->release();

		XMLPlatformUtils::Terminate();

		return errorCode;
}


int acs_apbm_nc_bladefilter::setBladeREDLed(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_status,std::string &xml)
{
	
        try
        {
        	XMLPlatformUtils::Initialize();
        }
        catch (const XMLException& toCatch)
        {
        	char *pMsg = XMLString::transcode(toCatch.getMessage());
                XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                << "  Exception message:"
                << pMsg;
                XMLString::release(&pMsg);
                return 1;
        }

        int errorCode = 0;

        DOMDocument *doc=0;
        DOMElement *element=0;

        getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);
        getXml(doc,xml);

        try
        {
		DOMElement* led = ((DOMDocument*)doc)->createElement(X("faultLed"));
                element->appendChild(led);
                  
		DOMElement* ledcolor = ((DOMDocument*)doc)->createElement(X("color"));
                led->appendChild(ledcolor);
                
		DOMText* ledcolorVal = 0;
                ledcolorVal =((DOMDocument*)doc)->createTextNode(X("RED"));
                ledcolor->appendChild(ledcolorVal);
                
                DOMElement* ledStatusop= ((DOMDocument*)doc)->createElement(X("status"));
                led->appendChild(ledStatusop);

                DOMText* ledStatusopVal = 0;
                ledStatusopVal =((DOMDocument*)doc)->createTextNode(X(led_status.c_str()));
                ledStatusop->appendChild(ledStatusopVal);
                  
		DOMElement* ledsupported= ((DOMDocument*)doc)->createElement(X("supported"));
                led->appendChild(ledsupported);

                DOMText* ledsupportedVal = 0;
                ledsupportedVal =((DOMDocument*)doc)->createTextNode(X("true"));
                ledsupported->appendChild(ledsupportedVal);                

                getXml(doc,xml);
        }
        catch (const XMLException& toCatch)
        {
        	char *pMsg = XMLString::transcode(toCatch.getMessage());

                XMLString::release(&pMsg);

                return 1;
        }
        doc->release();
        XMLPlatformUtils::Terminate();
        return errorCode;
}

int acs_apbm_nc_bladefilter::setBladeBlueLed(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_status,std::string &xml)
{
	
        try
        {
        	XMLPlatformUtils::Initialize();
        }
        catch (const XMLException& toCatch)
        {
        	char *pMsg = XMLString::transcode(toCatch.getMessage());
                XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                << "  Exception message:"
                << pMsg;
                XMLString::release(&pMsg);
                return 1;
        }

        int errorCode = 0;

        DOMDocument *doc=0;
        DOMElement *element=0;

        getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);
        getXml(doc,xml);

        try
        {
		DOMElement* led = ((DOMDocument*)doc)->createElement(X("maintenanceLed"));
                element->appendChild(led);
                  
		DOMElement* ledcolor = ((DOMDocument*)doc)->createElement(X("color"));
                led->appendChild(ledcolor);
                
		DOMText* ledcolorVal = 0;
                ledcolorVal =((DOMDocument*)doc)->createTextNode(X("BLUE"));
                ledcolor->appendChild(ledcolorVal);
                
                DOMElement* ledStatusop= ((DOMDocument*)doc)->createElement(X("status"));
                led->appendChild(ledStatusop);

                DOMText* ledStatusopVal = 0;
                ledStatusopVal =((DOMDocument*)doc)->createTextNode(X(led_status.c_str()));
                ledStatusop->appendChild(ledStatusopVal);
                  
		DOMElement* ledsupported= ((DOMDocument*)doc)->createElement(X("supported"));
                led->appendChild(ledsupported);

                DOMText* ledsupportedVal = 0;
                ledsupportedVal =((DOMDocument*)doc)->createTextNode(X("true"));
                ledsupported->appendChild(ledsupportedVal);                

                getXml(doc,xml);
        }
        catch (const XMLException& toCatch)
        {
        	char *pMsg = XMLString::transcode(toCatch.getMessage());

                XMLString::release(&pMsg);

                return 1;
        }
        doc->release();
        XMLPlatformUtils::Terminate();
        return errorCode;
}


int acs_apbm_nc_bladefilter::setBladeMIALed(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_status,std::string &xml)
{
	
        try
        {
        	XMLPlatformUtils::Initialize();
        }
        catch (const XMLException& toCatch)
        {
        	char *pMsg = XMLString::transcode(toCatch.getMessage());
                XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                << "  Exception message:"
                << pMsg;
                XMLString::release(&pMsg);
                return 1;
        }

        int errorCode = 0;

        DOMDocument *doc=0;
        DOMElement *element=0;

        getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);
        getXml(doc,xml);

        try
        {
		DOMElement* led = ((DOMDocument*)doc)->createElement(X("maintenanceLed"));
                element->appendChild(led);
                  
		DOMElement* ledcolor = ((DOMDocument*)doc)->createElement(X("color"));
                led->appendChild(ledcolor);
                
		DOMText* ledcolorVal = 0;
                ledcolorVal =((DOMDocument*)doc)->createTextNode(X("YELLOW"));
                ledcolor->appendChild(ledcolorVal);
                
                DOMElement* ledStatusop= ((DOMDocument*)doc)->createElement(X("status"));
                led->appendChild(ledStatusop);

                DOMText* ledStatusopVal = 0;
                ledStatusopVal =((DOMDocument*)doc)->createTextNode(X(led_status.c_str()));
                ledStatusop->appendChild(ledStatusopVal);
                  
		DOMElement* ledsupported= ((DOMDocument*)doc)->createElement(X("supported"));
                led->appendChild(ledsupported);

                DOMText* ledsupportedVal = 0;
                ledsupportedVal =((DOMDocument*)doc)->createTextNode(X("true"));
                ledsupported->appendChild(ledsupportedVal);                

                getXml(doc,xml);
        }
        catch (const XMLException& toCatch)
        {
        	char *pMsg = XMLString::transcode(toCatch.getMessage());

                XMLString::release(&pMsg);

                return 1;
        }
        doc->release();
        XMLPlatformUtils::Terminate();
        return errorCode;
}
int acs_apbm_nc_bladefilter::setBladeLedStatus(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string led_name,std::string led_status,std::string &xml)
{

	try
              		{
				XMLPlatformUtils::Initialize();
			}
			catch (const XMLException& toCatch)
			{
				char *pMsg = XMLString::transcode(toCatch.getMessage());
				XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
						<< "  Exception message:"
						<< pMsg;
				XMLString::release(&pMsg);
				return 1;
			}

		int errorCode = 0;

		DOMDocument *doc=0;
		DOMElement *element=0;

		getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);

		getXml(doc,xml);


		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering LED LOOP ");
		try
		{       
        
			/*
			if (led_name.compare("operationLed") == 0)
			{

				DOMElement* operationalLed = ((DOMDocument*)doc)->createElement(X("operationalLed"));

				element->appendChild(operationalLed);

				DOMElement* ledStatusop = ((DOMDocument*)doc)->createElement(X("ledStatus"));

				operationalLed->appendChild(ledStatusop);

				DOMText* ledStatusopVal = 0;

				if (led_status.compare("on") == 0)
					ledStatusopVal =((DOMDocument*)doc)->createTextNode(X("on"));
				else
					ledStatusopVal =((DOMDocument*)doc)->createTextNode(X("off"));

				ledStatusop->appendChild(ledStatusopVal);

				getXml(doc,xml);


			}
			else if(led_name.compare("faultLed") == 0){

				DOMElement* faultLed = ((DOMDocument*)doc)->createElement(X("faultLed"));

				element->appendChild(faultLed);

				DOMElement* ledStatusft = ((DOMDocument*)doc)->createElement(X("ledStatus"));

				faultLed->appendChild(ledStatusft);

				DOMText* ledStatusftVal =0;

				if (led_status.compare("on") == 0)
					ledStatusftVal =((DOMDocument*)doc)->createTextNode(X("on"));
				else
					ledStatusftVal =((DOMDocument*)doc)->createTextNode(X("off"));

				ledStatusft->appendChild(ledStatusftVal);

				getXml(doc,xml);

			}
			else if(led_name.compare("statusLed") == 0){


				DOMElement* statusLed = ((DOMDocument*)doc)->createElement(X("statusLed"));

				element->appendChild(statusLed);

				DOMElement* ledStatusst = ((DOMDocument*)doc)->createElement(X("ledStatus"));

				statusLed->appendChild(ledStatusst);

				DOMText* ledStatusstVal =0;

				if (led_status.compare("on") == 0)
					ledStatusstVal =((DOMDocument*)doc)->createTextNode(X("on"));
				else
					ledStatusstVal =((DOMDocument*)doc)->createTextNode(X("off"));

				ledStatusst->appendChild(ledStatusstVal);

				getXml(doc,xml);

			}

			else if(led_name.compare("maintenanceLed") == 0){
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MIALED branch");

				DOMElement* maintenanceLed = ((DOMDocument*)doc)->createElement(X("maintenanceLed"));

				element->appendChild(maintenanceLed);

				DOMElement* ledStatusmia = ((DOMDocument*)doc)->createElement(X("ledStatus"));

				maintenanceLed->appendChild(ledStatusmia);

				DOMText* ledStatusmiaVal =0;

				if (led_status.compare("on") == 0)
					ledStatusmiaVal =((DOMDocument*)doc)->createTextNode(X("on"));
				else
					ledStatusmiaVal =((DOMDocument*)doc)->createTextNode(X("off"));

				ledStatusmia->appendChild(ledStatusmiaVal);

				getXml(doc,xml);

			}
			else {

			}
			*/

			DOMElement* led = ((DOMDocument*)doc)->createElement(X(led_name.c_str()));
			element->appendChild(led);

			DOMElement* ledStatusop = ((DOMDocument*)doc)->createElement(X("status"));
			led->appendChild(ledStatusop);

			DOMText* ledStatusopVal = 0;
			ledStatusopVal =((DOMDocument*)doc)->createTextNode(X(led_status.c_str()));
			ledStatusop->appendChild(ledStatusopVal);

			getXml(doc,xml);


		}
		catch (const XMLException& toCatch)
		{
			char *pMsg = XMLString::transcode(toCatch.getMessage());

			XMLString::release(&pMsg);

			return 1;
		}

		doc->release();

		XMLPlatformUtils::Terminate();

		return errorCode;

}

int acs_apbm_nc_bladefilter::setReset(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml)
{
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch)
	{
		char *pMsg = XMLString::transcode(toCatch.getMessage());
		XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
				<< "  Exception message:"
				<< pMsg;
		XMLString::release(&pMsg);
		return 1;
	}

	int errorCode = 0;

	DOMDocument *doc=0;
	DOMElement *element=0;

	getBladeStructureData(&doc,&element,tenantId,physicAdd,shelf_id,slot_id);

	try
	{
		DOMElement* resetElem = ((DOMDocument*)doc)->createElement(X("reset"));

		element->appendChild(resetElem);

		DOMElement* resetTypeElem = ((DOMDocument*)doc)->createElement(X("resetType"));

		resetElem->appendChild(resetTypeElem);

		DOMText* resetTypeText = ((DOMDocument*)doc)->createTextNode(X("HARD"));

		resetTypeElem->appendChild(resetTypeText);

		DOMElement* gracefulResetElem = ((DOMDocument*)doc)->createElement(X("gracefulReset"));

		resetElem->appendChild(gracefulResetElem);

		DOMText* gracefulResetText = ((DOMDocument*)doc)->createTextNode(X("FALSE"));

		gracefulResetElem->appendChild(gracefulResetText);

		getXml(doc,xml);


	}
	catch (const XMLException& toCatch)
	{
		char *pMsg = XMLString::transcode(toCatch.getMessage());

		XMLString::release(&pMsg);

		return 1;
	}

	doc->release();

	XMLPlatformUtils::Terminate();


	return errorCode;


}
int acs_apbm_nc_bladefilter::setReset(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string resetType,std::string gracefulReset,std::string &xml)
{
        try
        {
                XMLPlatformUtils::Initialize();
        }
        catch (const XMLException& toCatch)
        {
                char *pMsg = XMLString::transcode(toCatch.getMessage());
                XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
                                << "  Exception message:"
                                << pMsg;
                XMLString::release(&pMsg);
                return 1;
        }

        int errorCode = 0;

        DOMDocument *doc=0;
        DOMElement *element=0;

        getBladeStructureData(&doc,&element,tenantId, physicAdd,shelf_id,slot_id);

        try
        {
                DOMElement* resetElem = ((DOMDocument*)doc)->createElement(X("reset"));

                element->appendChild(resetElem);
                 
                
                DOMElement* resetTypeElem = ((DOMDocument*)doc)->createElement(X("resetType"));

                resetElem->appendChild(resetTypeElem);
                
                DOMElement* resetTypeText = ((DOMDocument*)doc)->createElement(X(resetType.c_str()));

                resetTypeElem->appendChild(resetTypeText);
                 

                DOMElement* gracefulResetElem = ((DOMDocument*)doc)->createElement(X("gracefulReset"));

                resetElem->appendChild(gracefulResetElem);
                
                DOMElement* gracefulResetText = ((DOMDocument*)doc)->createElement(X(gracefulReset.c_str()));

                gracefulResetElem->appendChild(gracefulResetText);
                  
                getXml(doc,xml);

        }
        catch (const XMLException& toCatch)
        {
                char *pMsg = XMLString::transcode(toCatch.getMessage());

                XMLString::release(&pMsg);

                return 1;
        }

        doc->release();

        XMLPlatformUtils::Terminate();


        return errorCode;


}

int acs_apbm_nc_bladefilter::getBladeUserLabel(std::string tenantId, std::string physicAdd,std::string shelf_id,std::string slot_id,std::string &xml)
{

	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch)
	{
		char *pMsg = XMLString::transcode(toCatch.getMessage());
		XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
				<< "  Exception message:"
				<< pMsg;
		XMLString::release(&pMsg);
		return 1;
	}

	int errorCode = 0;

	DOMDocument *doc=0;
	DOMElement *element=0;

	getBladeStructureData(&doc,&element,tenantId,physicAdd,shelf_id,slot_id);

	try
	{
		DOMElement* userLabel = ((DOMDocument*)doc)->createElement(X("userLabel"));
		element->appendChild(userLabel);
		getXml(doc,xml);

	}
	catch (const XMLException& toCatch)
	{
		char *pMsg = XMLString::transcode(toCatch.getMessage());
		XMLString::release(&pMsg);
		return 1;
	}

	doc->release();
	XMLPlatformUtils::Terminate();

	return errorCode;
}



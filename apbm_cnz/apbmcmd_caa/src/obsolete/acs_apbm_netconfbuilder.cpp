/*acs_apbm_netconfbuilder.cpp
 
 *  Created on:July 6, 2013
 *      Author:xsabjha
 */
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>

#include "acs_apbm_netconfbuilder.h"

acs_apbm_netconfbuilder::acs_apbm_netconfbuilder() {
	// TODO Auto-generated constructor stub

}

acs_apbm_netconfbuilder::~acs_apbm_netconfbuilder() {
	// TODO Auto-generated destructor stub
}
int acs_apbm_netconfbuilder::getXml(DOMDocument* Doc,std::string &xml) {

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

    xml=xmlStr;

    return 0;
}


int acs_apbm_netconfbuilder::createBladeQuery(std::string tenantId, std::string blade_id,std::string &xml)
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

    getBladeStructure(&doc,&element,tenantId,blade_id);

    try
    {
            DOMElement* biosImage = ((DOMDocument*)doc)->createElement(X("biosDefaultImage"));
            element->appendChild(biosImage);
            DOMElement* biosPointer =((DOMDocument*)doc)->createElement(X("biosPointer"));
            element->appendChild(biosPointer);
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

int acs_apbm_netconfbuilder::createShelfQuery(std::string shelfId,std::string physicaladdress,std::string &xml)
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

    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (impl != NULL) {

        try
        {

            DOMDocument* doc =impl->createDocument(0,X("ManagedElement"),0);

            DOMElement* rootElem = doc->getDocumentElement();


            DOMElement* managedElementIdElem = doc->createElement(X("managedElementId"));
            DOMText* managedElementIdVal = doc->createTextNode(X(std::string("1").c_str()));
            managedElementIdElem->appendChild(managedElementIdVal);

            rootElem->appendChild(managedElementIdElem);

            DOMElement* equipfElem = doc->createElement(X("Equipment"));
            rootElem->appendChild(equipfElem);

            DOMElement* equipIdElem = doc->createElement(X("equipmentId"));
            DOMText* equipIdVal = doc->createTextNode(X(std::string("1").c_str()));
            equipIdElem->appendChild(equipIdVal);
            equipfElem->appendChild(equipIdElem);


            DOMElement* shelf = doc->createElement(X("Shelf"));

            equipfElem->appendChild(shelf);

            DOMElement* shelfId_tag = doc->createElement(X("shelfId"));;

            if(!shelfId.empty())
            {
                    DOMText* shelfIdText = doc->createTextNode(X(shelfId.c_str()));
                    shelfId_tag->appendChild(shelfIdText);

            }

            shelf->appendChild(shelfId_tag);

            DOMElement* physicalAddress_tag=doc->createElement(X("physicalAddress"));

            if(!physicaladdress.empty())
            {
                    DOMText* physicalAddressText = doc->createTextNode(X(physicaladdress.c_str()));
                    physicalAddress_tag->appendChild(physicalAddressText);
            }
            shelf->appendChild(physicalAddress_tag);

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
    }// (inpl != NULL)
    else {
        XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
        errorCode = 4;
    }

    XMLPlatformUtils::Terminate();
    return errorCode;
}

int acs_apbm_netconfbuilder::createTenantQuery(std::string &xml)
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

    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (impl != NULL) {

        try
        {

            //#### Managed Element ####

            DOMDocument* doc =impl->createDocument(0,X("ManagedElement"),0);

            DOMElement* rootElem = doc->getDocumentElement();

            DOMElement* managedElementIdElem = doc->createElement(X("managedElementId"));

            DOMText* managedElementIdVal = doc->createTextNode(X(std::string("1").c_str()));

            managedElementIdElem->appendChild(managedElementIdVal);

            rootElem->appendChild(managedElementIdElem);

            //#### DmxcFunctions ####

            DOMElement* DmxcFunctionElem = doc->createElement(X("DmxcFunction"));

	    rootElem->appendChild(DmxcFunctionElem);

	    DOMElement* dmxcFunctionIdElem = doc->createElement(X("dmxcFunctionId"));

            DOMText* dmxcFunctionIdVal = doc->createTextNode(X(std::string("1").c_str()));

	    dmxcFunctionIdElem->appendChild(dmxcFunctionIdVal);

	    DmxcFunctionElem->appendChild(dmxcFunctionIdElem);

            //#### Tenants ####

	    DOMElement* TenantsElem = doc->createElement(X("Tenant"));

	    DmxcFunctionElem->appendChild(TenantsElem);

	    DOMElement* tenantsIdElem = doc->createElement(X("tenantId"));

	    TenantsElem->appendChild(tenantsIdElem);

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
    }// (inpl != NULL)
    else {
        XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
        errorCode = 4;
    }

    XMLPlatformUtils::Terminate();
    return errorCode;
}



int acs_apbm_netconfbuilder::createsetbiosimageQuery(std::string tenantId, std::string blade_id,std::string bios_image,std::string &xml)
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

    getBladeStructure(&doc,&element,tenantId,blade_id);
    try
    {
            DOMElement* biosImage = ((DOMDocument*)doc)->createElement(X("biosDefaultImage"));
            if(!bios_image.empty())
            {
               DOMText* bios_image_Val = ((DOMDocument*)doc)->createTextNode(X(bios_image.c_str()));
               biosImage->appendChild(bios_image_Val);
               element->appendChild(biosImage);
            }

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

int acs_apbm_netconfbuilder::createsetbiospointerQuery(std::string tenantId, std::string blade_id,std::string bios_pointer,std::string &xml)
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

    getBladeStructure(&doc,&element,tenantId,blade_id);

    try
    {
            DOMElement* biosPointer =((DOMDocument*)doc)->createElement(X("biosPointer"));
            if (!bios_pointer.empty())
            {
               DOMText* bios_pointer_Val = ((DOMDocument*)doc)->createTextNode(X(bios_pointer.c_str()));
               biosPointer->appendChild(bios_pointer_Val);
               element->appendChild(biosPointer);
            }

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

int acs_apbm_netconfbuilder::getBladeUserLabel(std::string tenantId, std::string blade_id, std::string &xml)
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

    getBladeStructure(&doc,&element,tenantId,blade_id);

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

int acs_apbm_netconfbuilder::getBladeStructure(DOMDocument **docOut,DOMElement **element, std::string tenantId, std::string blade_Id)
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

			if((!blade_Id.empty())) 
			{
				DOMText* BladeIdText = (*docOut)->createTextNode(X(blade_Id.c_str()));
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


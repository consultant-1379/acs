/*
 * acs_apbm_nc_dmxfunctionsfilter.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: qrenbor
 */

#include "acs_apbm_nc_dmxfunctionsfilter.h"
#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"

acs_apbm_nc_dmxfunctionsfilter::acs_apbm_nc_dmxfunctionsfilter() {
	// TODO Auto-generated constructor stub

}

acs_apbm_nc_dmxfunctionsfilter::~acs_apbm_nc_dmxfunctionsfilter() {
	// TODO Auto-generated destructor stub
}



int acs_apbm_nc_dmxfunctionsfilter::setXmlSetSnmpTwoSnmpTargets(std::string address1,std::string userLabel1,std::string address2,std::string userLabel2,std::string &xml)
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

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	DOMDocument* doc=0;

	if (impl != NULL) {
		try {

			doc =impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = doc->getDocumentElement();

			rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));


			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="managedElementId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,rootElem,mt);
			}

			DOMElement* dmxFunctionElem = doc->createElement(X("DmxFunctions"));

			rootElem->appendChild(dmxFunctionElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxFunctionsId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,dmxFunctionElem,mt);
			}


			DOMElement* DmxSysMElem = doc->createElement(X("DmxSysM"));
			dmxFunctionElem->appendChild(DmxSysMElem);


			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxSysMId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,DmxSysMElem,mt);
			}


			DOMElement* SnmpElem = doc->createElement(X("Snmp"));
			DmxSysMElem->appendChild(SnmpElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="snmpId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,SnmpElem,mt);
			}

			DOMElement* SnmpTargetElem = doc->createElement(X("SnmpTarget"));
			SnmpElem->appendChild(SnmpTargetElem);

			{
				std::vector<xmtag> mt;

				xmtag t1,t2,t3,t4,t5;

				t1.tagname="snmpTargetId";
				t1.tagval="1";

				t2.tagname="address";

				if(!address1.empty())
					t2.tagval=address1;

				t3.tagname="userLabel";

				if(!userLabel1.empty())
					t3.tagval=userLabel1;

				t4.tagname="informEnable";
				t4.tagval="false";

				t5.tagname="administrativeState";
				t5.tagval="unlocked";

				mt.push_back(t1);
				mt.push_back(t2);
				mt.push_back(t3);
				mt.push_back(t4);
				mt.push_back(t5);

				createElement(doc,SnmpTargetElem,mt);

			}

			DOMElement* SnmpTargetElem2 = doc->createElement(X("SnmpTarget"));
			SnmpElem->appendChild(SnmpTargetElem2);

			{
				std::vector<xmtag> mt;

				xmtag t1,t2,t3,t4,t5;

				t1.tagname="snmpTargetId";
				t1.tagval="2";

				t2.tagname="address";

				if(!address2.empty())
					t2.tagval=address2;

				t3.tagname="userLabel";

				if(!userLabel2.empty())
					t3.tagval=userLabel2;

				t4.tagname="informEnable";
				t4.tagval="false";

				t5.tagname="administrativeState";
				t5.tagval="unlocked";

				mt.push_back(t1);
				mt.push_back(t2);
				mt.push_back(t3);
				mt.push_back(t4);
				mt.push_back(t5);

				createElement(doc,SnmpTargetElem2,mt);
			}

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
	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}

	XMLPlatformUtils::Terminate();
	return errorCode;


}



int acs_apbm_nc_dmxfunctionsfilter::setXmlSetSnmpOneSnmpTarget(std::string address1,std::string userLabel1,std::string &xml)
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

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	DOMDocument* doc=0;

	if (impl != NULL) {
		try {

			doc =impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = doc->getDocumentElement();

			rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));


			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="managedElementId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,rootElem,mt);
			}

			DOMElement* dmxFunctionElem = doc->createElement(X("DmxFunctions"));

			rootElem->appendChild(dmxFunctionElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxFunctionsId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,dmxFunctionElem,mt);
			}


			DOMElement* DmxSysMElem = doc->createElement(X("DmxSysM"));
			dmxFunctionElem->appendChild(DmxSysMElem);


			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxSysMId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,DmxSysMElem,mt);
			}


			DOMElement* SnmpElem = doc->createElement(X("Snmp"));
			DmxSysMElem->appendChild(SnmpElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="snmpId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,SnmpElem,mt);
			}

			DOMElement* SnmpTargetElem = doc->createElement(X("SnmpTarget"));
			SnmpElem->appendChild(SnmpTargetElem);

			{
				std::vector<xmtag> mt;

				xmtag t1,t2,t3,t4,t5;

				t1.tagname="snmpTargetId";
				t1.tagval="1";

				t2.tagname="address";

				if(!address1.empty())
					t2.tagval=address1;

				t3.tagname="userLabel";

				if(!userLabel1.empty())
					t3.tagval=userLabel1;

				t4.tagname="informEnable";
				t4.tagval="false";

				t5.tagname="administrativeState";
				t5.tagval="unlocked";

				mt.push_back(t1);
				mt.push_back(t2);
				mt.push_back(t3);
				mt.push_back(t4);
				mt.push_back(t5);

				createElement(doc,SnmpTargetElem,mt);

			}

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
	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}

	XMLPlatformUtils::Terminate();
	return errorCode;


	return 0;
}



void acs_apbm_nc_dmxfunctionsfilter::createElement(DOMDocument *doc, DOMElement *prec,std::vector<xmtag> c)
{
  std::vector<xmtag>::iterator it;

  for(it = c.begin(); it != c.end(); ++it)
  {
		xmtag x=*it;

		DOMElement* elem = doc->createElement(X(x.tagname.c_str()));

		if(!x.tagval.empty())
		{
			DOMText* val = doc->createTextNode(X(x.tagval.c_str()));
			elem->appendChild(val);
		}

		prec->appendChild(elem);
  }

}



int acs_apbm_nc_dmxfunctionsfilter::getXml(DOMDocument* Doc,std::string &xml) {

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

    ACS_APBM_LOG(LOG_LEVEL_INFO, "Entering Method %s",  xmlStr.c_str());

    xml=xmlStr;

    return 0;
}


int acs_apbm_nc_dmxfunctionsfilter::setXmlSetAdministrativeStateLockSnmpTarget(std::string snmpTargetId,std::string &xml)
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

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	DOMDocument* doc=0;

	if (impl != NULL) {
		try {

			doc =impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = doc->getDocumentElement();

			rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));


			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="managedElementId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,rootElem,mt);
			}

			DOMElement* dmxFunctionElem = doc->createElement(X("DmxFunctions"));

			rootElem->appendChild(dmxFunctionElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxFunctionsId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,dmxFunctionElem,mt);
			}


			DOMElement* DmxSysMElem = doc->createElement(X("DmxSysM"));
			dmxFunctionElem->appendChild(DmxSysMElem);


			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxSysMId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,DmxSysMElem,mt);
			}


			DOMElement* SnmpElem = doc->createElement(X("Snmp"));
			DmxSysMElem->appendChild(SnmpElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="snmpId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,SnmpElem,mt);
			}

			DOMElement* SnmpTargetElem = doc->createElement(X("SnmpTarget"));
			SnmpElem->appendChild(SnmpTargetElem);

			{
				std::vector<xmtag> mt;

				xmtag t1,t2;

				t1.tagname="snmpTargetId";
				t1.tagval=snmpTargetId;

				t2.tagname="administrativeState";
				t2.tagval="locked";

				mt.push_back(t1);
				mt.push_back(t2);

				createElement(doc,SnmpTargetElem,mt);
			}

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
	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}

	XMLPlatformUtils::Terminate();
	return errorCode;


}


int acs_apbm_nc_dmxfunctionsfilter::setXmlSetAdministrativeStateLockBothSnmpTargets(std::string &xml)
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

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	DOMDocument* doc=0;

	if (impl != NULL) {
		try {

			doc =impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = doc->getDocumentElement();

			rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));


			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="managedElementId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,rootElem,mt);
			}

			DOMElement* dmxFunctionElem = doc->createElement(X("DmxFunctions"));

			rootElem->appendChild(dmxFunctionElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxFunctionsId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,dmxFunctionElem,mt);
			}


			DOMElement* DmxSysMElem = doc->createElement(X("DmxSysM"));
			dmxFunctionElem->appendChild(DmxSysMElem);


			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxSysMId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,DmxSysMElem,mt);
			}


			DOMElement* SnmpElem = doc->createElement(X("Snmp"));
			DmxSysMElem->appendChild(SnmpElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="snmpId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,SnmpElem,mt);
			}

			DOMElement* SnmpTargetElem = doc->createElement(X("SnmpTarget"));
			SnmpElem->appendChild(SnmpTargetElem);

			{
				std::vector<xmtag> mt;

				xmtag t1,t2;

				t1.tagname="snmpTargetId";
				t1.tagval="1";

				t2.tagname="administrativeState";
				t2.tagval="locked";

				mt.push_back(t1);
				mt.push_back(t2);

				createElement(doc,SnmpTargetElem,mt);

			}

			DOMElement* SnmpTargetElem2 = doc->createElement(X("SnmpTarget"));
			SnmpElem->appendChild(SnmpTargetElem2);

			{
				std::vector<xmtag> mt;

				xmtag t1,t2;

				t1.tagname="snmpTargetId";
				t1.tagval="2";

				t2.tagname="administrativeState";
				t2.tagval="unlocked";

				mt.push_back(t1);
				mt.push_back(t2);

				createElement(doc,SnmpTargetElem2,mt);
			}

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
	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}

	XMLPlatformUtils::Terminate();
	return errorCode;


}



int acs_apbm_nc_dmxfunctionsfilter::setXmlUpgradeJobs(std::string /*physicalAddress*/,std::string shelf_id,std::string slot_id,std::string &xml)
{
/*
	<filter type="subtree">
		<ManagedElement xmlns="http://www.ericsson.com/dmx">
			<managedElementId>1</managedElementId>
			<DmxFunctions>
				<dmxFunctionsId>1</dmxFunctionsId>
					<UpgradeJobs>
						<type>ipmi</type>
						<notStarted>0-20</notStarted>
					</UpgradeJobs>
					<UpgradeJobs>
						<type>ipmi</type>
						<ongoing>0-20</ongoing>
					</UpgradeJobs>
					<UpgradeJobs>
						<type>ipmi</type>
						<succeded>0-20</succeded>
					</UpgradeJobs>
					<UpgradeJobs>
						<type>ipmi</type>
						<failed>0-20</failed>
					</UpgradeJobs>
			</DmxFunctions>
		</ManagedElement>
	</filter>
*/


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

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	DOMDocument* doc=0;

	if (impl != NULL) {
		try {

			doc =impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = doc->getDocumentElement();

			rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));


			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="managedElementId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,rootElem,mt);
			}

			DOMElement* dmxFunctionElem = doc->createElement(X("DmxFunctions"));

			rootElem->appendChild(dmxFunctionElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxFunctionsId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,dmxFunctionElem,mt);
			}

			DOMElement* UpgradeJobslem = doc->createElement(X("UpgradeJobs"));
			dmxFunctionElem->appendChild(UpgradeJobslem);


			{
				std::vector<xmtag> mt;
				xmtag t1,t2;
				t1.tagname="type";
				t1.tagval="ipmi";

				t2.tagname="notStarted";
				t2.tagval=shelf_id+"-"+slot_id;

				mt.push_back(t1);
				mt.push_back(t2);

				createElement(doc,UpgradeJobslem,mt);
			}

			{
				std::vector<xmtag> mt;
				xmtag t1,t2;
				t1.tagname="type";
				t1.tagval="ipmi";

				t2.tagname="ongoing";
				t2.tagval=shelf_id+"-"+slot_id;

				mt.push_back(t1);
				mt.push_back(t2);

				createElement(doc,UpgradeJobslem,mt);
			}

			{
				std::vector<xmtag> mt;
				xmtag t1,t2;
				t1.tagname="type";
				t1.tagval="ipmi";

				t2.tagname="succeded";
				t2.tagval=shelf_id+"-"+slot_id;

				mt.push_back(t1);
				mt.push_back(t2);

				createElement(doc,UpgradeJobslem,mt);
			}

			{
				std::vector<xmtag> mt;
				xmtag t1,t2;
				t1.tagname="type";
				t1.tagval="ipmi";

				t2.tagname="failed";
				t2.tagval=shelf_id+"-"+slot_id;

				mt.push_back(t1);
				mt.push_back(t2);

				createElement(doc,UpgradeJobslem,mt);
			}



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
	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}

	XMLPlatformUtils::Terminate();

	return errorCode;
}

int acs_apbm_nc_dmxfunctionsfilter::setXmlActUpgradeIpmiFirmware(std::string shelf_id,std::string slot_id,std::string ipAddress,std::string port,std::string username,std::string password,std::string filename,	std::string &xml)
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

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	DOMDocument* doc=0;

	if (impl != NULL) {
		try {

			doc =impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = doc->getDocumentElement();

			rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="managedElementId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,rootElem,mt);
			}

			DOMElement* dmxFunctionElem = doc->createElement(X("DmxFunctions"));

			rootElem->appendChild(dmxFunctionElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxFunctionsId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,dmxFunctionElem,mt);
			}


			DOMElement* upgradeIpmiFirmwareElem = doc->createElement(X("upgradeIpmiFirmware"));
			dmxFunctionElem->appendChild(upgradeIpmiFirmwareElem);

			{
				std::vector<xmtag> mt;

				xmtag t1,t2,t3,t4,t5;
				t1.tagname="ipAddress";
				t1.tagval=ipAddress;

				t2.tagname="port";
				t2.tagval=port;

				t3.tagname="username";
				t3.tagval=username;

				t4.tagname="password";
				t4.tagval=password;

				t5.tagname="filename";
				t5.tagval=filename;

				mt.push_back(t1);
				mt.push_back(t2);
				mt.push_back(t3);
				mt.push_back(t4);
				mt.push_back(t5);

				createElement(doc,upgradeIpmiFirmwareElem,mt);
			}

			DOMElement* BladeElem = doc->createElement(X("Blade"));
			upgradeIpmiFirmwareElem->appendChild(BladeElem);

			{
				std::vector<xmtag> mt;
				xmtag t1,t2;
				t1.tagname="shelf";
				t1.tagval=shelf_id;
				t2.tagname="slot";
				t2.tagval=slot_id;

				mt.push_back(t1);
				mt.push_back(t2);
				createElement(doc,BladeElem,mt);
			}


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
	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}

	XMLPlatformUtils::Terminate();
	return errorCode;

}

int acs_apbm_nc_dmxfunctionsfilter::setXmlGetSnmpAddressAttribute(std::string &xml)
{

	return setXmlGetSnmpAttribute("address",xml);
}

int acs_apbm_nc_dmxfunctionsfilter::setXmlGetSnmpAttribute(std::string attribute,std::string &xml)
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

	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

	DOMDocument* doc=0;

	if (impl != NULL) {
		try {

			doc =impl->createDocument(0,X("ManagedElement"),0);

			DOMElement* rootElem = doc->getDocumentElement();

			rootElem->setAttribute(X("xmlns"), X("http://www.ericsson.com/dmx"));

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="managedElementId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,rootElem,mt);
			}

			DOMElement* dmxFunctionElem = doc->createElement(X("DmxFunctions"));

			rootElem->appendChild(dmxFunctionElem);

			{
				std::vector<xmtag> mt;
				xmtag msnmpId;
				msnmpId.tagname="dmxFunctionsId";
				msnmpId.tagval="1";
				mt.push_back(msnmpId);
				createElement(doc,dmxFunctionElem,mt);
			}


			DOMElement* DmxSysMElem = doc->createElement(X("DmxSysM"));
			dmxFunctionElem->appendChild(DmxSysMElem);

			{
				std::vector<xmtag> mt;

				xmtag t1;
				t1.tagname="dmxSysMId";
				t1.tagval="1";
				mt.push_back(t1);
				createElement(doc,DmxSysMElem,mt);
			}

			DOMElement* SnmpElem = doc->createElement(X("Snmp"));
			DmxSysMElem->appendChild(SnmpElem);
			{
				std::vector<xmtag> mt;
				xmtag t1;
				t1.tagname="snmpId";
				t1.tagval="1";
				mt.push_back(t1);
				createElement(doc,SnmpElem,mt);
			}

			DOMElement* SnmpTargetElem = doc->createElement(X("SnmpTarget"));
			SnmpElem->appendChild(SnmpTargetElem);

			{
				std::vector<xmtag> mt;
				xmtag t1;
				t1.tagname=attribute;
				t1.tagval="";
				mt.push_back(t1);
				createElement(doc,SnmpTargetElem,mt);
			}



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
	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}

	XMLPlatformUtils::Terminate();
	return errorCode;
}


int acs_apbm_nc_dmxfunctionsfilter::getIpV4controlNetworkIds(std::string tenantId, std::string &xml)
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

        DOMDocument* doc=0;


        if (impl != NULL)
        {

                try
                {
                        //#### Managed Element ####

                        doc=impl->createDocument(0,X("ManagedElement"),0);

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

                        //#### Tenant ####

                        DOMElement* TenantElem = doc->createElement(X("Tenant"));

                        DmxcFunctionElem->appendChild(TenantElem);

                        DOMElement* tenantIdElem = doc->createElement(X("tenantId"));
                        
                        DOMText* tenantIdVal = doc->createTextNode(X(tenantId.c_str()));

                        tenantIdElem->appendChild(tenantIdVal);

                        TenantElem->appendChild(tenantIdElem);

                        //#### Control Network #######

                        DOMElement* ControlNetworkElem = doc->createElement(X("ControlNetwork"));
                        TenantElem->appendChild(ControlNetworkElem);

                        DOMElement* ctrlNetIdElem = doc->createElement(X("controlNetworkId"));
                        DOMElement* switchSideElem = doc->createElement(X("switchSide"));
                        ControlNetworkElem->appendChild(ctrlNetIdElem);
                        ControlNetworkElem->appendChild(switchSideElem);

                        getXml(doc,xml);                 
                        
                        doc->release();


                } catch (const OutOfMemoryException&)
                {
                        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
                        errorCode = 5;

                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }


                } catch (const DOMException& e)
                {
                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }


                        XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
                        errorCode = 2;

                } catch (...)
                {

                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }
                        XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
                        errorCode = 3;
                }
        }// (inpl != NULL)
        else {
                XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
                errorCode = 4;
  }

         return errorCode;

}


int acs_apbm_nc_dmxfunctionsfilter::getIpV4Connection(std::string ipV4NetworkId_str,std::string ipV4ConnectionId_str,std::string &xml)
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

        DOMDocument* doc=0;


        if (impl != NULL)
        {

                try
                {
                        //#### Managed Element ####

                        doc=impl->createDocument(0,X("ManagedElement"),0);

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

                        //#### NetWorks ####

                        DOMElement* NetworksElem = doc->createElement(X("Networks"));

                        DmxcFunctionElem->appendChild(NetworksElem);

                        DOMElement* networksIdElem = doc->createElement(X("networksId"));

                        DOMText* networksIdVal = doc->createTextNode(X(std::string("1").c_str()));

                        networksIdElem->appendChild(networksIdVal);

                        NetworksElem->appendChild(networksIdElem);
                        
                        //#### IpV4Network ####
                        
                        DOMElement* IpV4NetworkElem = doc->createElement(X("IPv4Network"));
                        
                        NetworksElem->appendChild(IpV4NetworkElem);

                        DOMElement* ipV4NetworkIdElem = doc->createElement(X("iPv4NetworkId"));

                        DOMText* ipV4NetworkIdVal = doc->createTextNode(X(ipV4NetworkId_str.c_str()));

                        ipV4NetworkIdElem->appendChild(ipV4NetworkIdVal);

                        IpV4NetworkElem->appendChild(ipV4NetworkIdElem);

                        //#####IpV4Connection #####

                        DOMElement* IpV4ConnectionElem = doc->createElement(X("IPv4Connection"));
                        
                        IpV4NetworkElem->appendChild(IpV4ConnectionElem);

                        DOMElement* ipV4ConnectionIdElem = doc->createElement(X("iPv4ConnectionId"));

                        DOMText* ipV4ConnectionIdVal = doc->createTextNode(X(ipV4ConnectionId_str.c_str()));

                        ipV4ConnectionIdElem->appendChild(ipV4ConnectionIdVal);

                        IpV4ConnectionElem->appendChild(ipV4ConnectionIdElem);                         

                        //Ipaddress

                        DOMElement* ipAddrElem = doc->createElement(X("ipAddr"));
                        IpV4ConnectionElem->appendChild(ipAddrElem);

                        //Port 

                        DOMElement* portElem = doc->createElement(X("port"));
                        IpV4ConnectionElem->appendChild(portElem);
                        
                        //Protocol

                        DOMElement* protocolElem = doc->createElement(X("protocol")); 
                        IpV4ConnectionElem->appendChild(protocolElem); 

                        
                        getXml(doc,xml);                 
                        
                        doc->release();


                } catch (const OutOfMemoryException&)
                {
                        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
                        errorCode = 5;

                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }


                } catch (const DOMException& e)
                {
                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }


                        XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
                        errorCode = 2;

                } catch (...)
                {

                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }
                        XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
                        errorCode = 3;
                }
        }// (inpl != NULL)
        else {
                XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
                errorCode = 4;
  }

         return errorCode;

}


int acs_apbm_nc_dmxfunctionsfilter::createIpV4Connection(std::string ipV4NetworkId_str,std::string ipV4ConnectionId_str,std::string Ipaddr_str,std::string &xml)
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

        DOMDocument* doc=0;


        if (impl != NULL)
        {

                try
                {
                        //#### Managed Element ####

                        doc=impl->createDocument(0,X("ManagedElement"),0);

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

                        //#### NetWorks ####

                        DOMElement* NetworksElem = doc->createElement(X("Networks"));

                        DmxcFunctionElem->appendChild(NetworksElem);

                        DOMElement* networksIdElem = doc->createElement(X("networksId"));

                        DOMText* networksIdVal = doc->createTextNode(X(std::string("1").c_str()));

                        networksIdElem->appendChild(networksIdVal);

                        NetworksElem->appendChild(networksIdElem);
                        
                        //#### IpV4Network ####
                        
                        DOMElement* IpV4NetworkElem = doc->createElement(X("IPv4Network"));
                        
                        NetworksElem->appendChild(IpV4NetworkElem);
	
                        DOMElement* ipV4NetworkIdElem = doc->createElement(X("iPv4NetworkId"));

                        DOMText* ipV4NetworkIdVal = doc->createTextNode(X(ipV4NetworkId_str.c_str()));

                        ipV4NetworkIdElem->appendChild(ipV4NetworkIdVal);

                        IpV4NetworkElem->appendChild(ipV4NetworkIdElem);

                        //#####IpV4Connection #####

                        DOMElement* IpV4ConnectionElem = doc->createElement(X("IPv4Connection"));
                        
                        IpV4NetworkElem->appendChild(IpV4ConnectionElem);

                        DOMElement* ipV4ConnectionIdElem = doc->createElement(X("iPv4ConnectionId"));

                        DOMText* ipV4ConnectionIdVal = doc->createTextNode(X(ipV4ConnectionId_str.c_str()));

                        ipV4ConnectionIdElem->appendChild(ipV4ConnectionIdVal);

                        IpV4ConnectionElem->appendChild(ipV4ConnectionIdElem);                         

                        //Ipaddress

                        DOMElement* ipAddrElem = doc->createElement(X("ipAddr"));

                        DOMText* ipAddrVal = doc->createTextNode(X(Ipaddr_str.c_str()));
                        
                        ipAddrElem->appendChild(ipAddrVal);
                               
                        IpV4ConnectionElem->appendChild(ipAddrElem);

                        //Port 

                        DOMElement* portElem = doc->createElement(X("port"));

                        DOMText* portVal = doc->createTextNode(X(std::string("162").c_str()));
                        
                        portElem->appendChild(portVal);  

                        IpV4ConnectionElem->appendChild(portElem);
                        
                        //Protocol

                        DOMElement* protocolElem = doc->createElement(X("protocol"));
                        
                        DOMText* protocolVal = doc->createTextNode(X("UDP"));
                        
                        protocolElem->appendChild(protocolVal); 
 
                        IpV4ConnectionElem->appendChild(protocolElem); 

                        
                        getXml(doc,xml);                 
                        
                        doc->release();


                } catch (const OutOfMemoryException&)
                {
                        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
                        errorCode = 5;

                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }


                } catch (const DOMException& e)
                {
                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }


                        XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
                        errorCode = 2;

                } catch (...)
                {

                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }
                        XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
                        errorCode = 3;
                }
        }// (inpl != NULL)
        else {
                XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
                errorCode = 4;
  }

         return errorCode;

}


int acs_apbm_nc_dmxfunctionsfilter::removeIpV4Connection(std::string ipV4NetworkId_str,std::string ipV4ConnectionId_str,std::string &xml)
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

        DOMDocument* doc=0;


        if (impl != NULL)
        {

                try
                {
                        //#### Managed Element ####

                        doc=impl->createDocument(0,X("ManagedElement"),0);

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

                        //#### NetWorks ####

                        DOMElement* NetworksElem = doc->createElement(X("Networks"));

                        DmxcFunctionElem->appendChild(NetworksElem);

                        DOMElement* networksIdElem = doc->createElement(X("networksId"));

                        DOMText* networksIdVal = doc->createTextNode(X(std::string("1").c_str()));

                        networksIdElem->appendChild(networksIdVal);

                        NetworksElem->appendChild(networksIdElem);
                        
                        //#### IpV4Network ####
                        
                        DOMElement* IpV4NetworkElem = doc->createElement(X("IPv4Network"));
                        
                        NetworksElem->appendChild(IpV4NetworkElem);

                        DOMElement* ipV4NetworkIdElem = doc->createElement(X("iPv4NetworkId"));

                        DOMText* ipV4NetworkIdVal = doc->createTextNode(X(ipV4NetworkId_str.c_str()));

                        ipV4NetworkIdElem->appendChild(ipV4NetworkIdVal);

                        IpV4NetworkElem->appendChild(ipV4NetworkIdElem);

                        //#####IpV4Connection #####

                        DOMElement* IpV4ConnectionElem = doc->createElement(X("IPv4Connection"));

                        IpV4ConnectionElem->setAttribute(X("xc:operation"), X("delete"));
                                                
                        IpV4NetworkElem->appendChild(IpV4ConnectionElem);

                        DOMElement* ipV4ConnectionIdElem = doc->createElement(X("iPv4ConnectionId"));

                        DOMText* ipV4ConnectionIdVal = doc->createTextNode(X(ipV4ConnectionId_str.c_str()));

                        ipV4ConnectionIdElem->appendChild(ipV4ConnectionIdVal);

                        IpV4ConnectionElem->appendChild(ipV4ConnectionIdElem);                         
                                                
                        getXml(doc,xml);                 
                        
                        doc->release();


                } catch (const OutOfMemoryException&)
                {
                        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
                        errorCode = 5;

                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }


                } catch (const DOMException& e)
                {
                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }


                        XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
                        errorCode = 2;

                } catch (...)
                {

                        if(doc)
                        {
                                doc->release();
                                doc=0;
                        }
                        XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
                        errorCode = 3;
                }
        }// (inpl != NULL)
        else {
                XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
                errorCode = 4;
  }

         return errorCode;

}

int acs_apbm_nc_dmxfunctionsfilter::trapsubscription_all(std::string tenantId,std::string contrlNetworkId_str,std::string bgciManagerId_str,std::string &xml)
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

        getBgciManagerStructure(&doc,&element,tenantId,contrlNetworkId_str,bgciManagerId_str);


        try
        {


                //### bladeStatusSubscriber ######

                DOMElement* bladeStatusSubscriberElem = ((DOMDocument*)doc)->createElement(X("bladeStatusSubscriber"));
                  
                DOMText* bladeStatusSubscriberVal=((DOMDocument*)doc)->createTextNode(X("true"));

                bladeStatusSubscriberElem->appendChild(bladeStatusSubscriberVal);

                element->appendChild(bladeStatusSubscriberElem);
                
                
                //### groupLinkStatusSubscriber ####
 
                DOMElement* groupLinkStatusSubscriberElem = ((DOMDocument*)doc)->createElement(X("groupLinkStatusSubscriber"));
                  
                DOMText* groupLinkStatusSubscriberVal=((DOMDocument*)doc)->createTextNode(X("false"));

                groupLinkStatusSubscriberElem->appendChild(groupLinkStatusSubscriberVal);

                element->appendChild(groupLinkStatusSubscriberElem);  
                
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
int acs_apbm_nc_dmxfunctionsfilter::getBgciManagerStructure(DOMDocument **docOut,DOMElement **element,std::string tenantId,std::string controlNetworkId_str,std::string bgciManagerId_str)
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

                        //#### Tenant ####

                        DOMElement* TenantElem = (*docOut)->createElement(X("Tenant"));

                        DmxcFunctionElem->appendChild(TenantElem);

                        DOMElement* tenantIdElem = (*docOut)->createElement(X("tenantId"));

                        DOMText* tenantIdVal = (*docOut)->createTextNode(X(tenantId.c_str()));

                        tenantIdElem->appendChild(tenantIdVal);

                        TenantElem->appendChild(tenantIdElem);
                        
                        //#### ControlNetwork ####
                        
                        DOMElement* ControlNetworkElem = (*docOut)->createElement(X("ControlNetwork"));
                        
                        TenantElem->appendChild(ControlNetworkElem);

                        DOMElement* controlNetworkIdElem = (*docOut)->createElement(X("controlNetworkId"));

                        DOMText* controlNetworkIdVal = (*docOut)->createTextNode(X(controlNetworkId_str.c_str()));

                        controlNetworkIdElem->appendChild(controlNetworkIdVal);
                        
                        ControlNetworkElem->appendChild(controlNetworkIdElem); 
                        
                        //#### BgciManager ####
                        
                        DOMElement* BgciManagerElem = (*docOut)->createElement(X("BgciManager"));
                        
                        ControlNetworkElem->appendChild(BgciManagerElem);

                        DOMElement* bgciManagerIdElem = (*docOut)->createElement(X("bgciManagerId"));

                        DOMText* bgciManagerIdVal = (*docOut)->createTextNode(X(bgciManagerId_str.c_str()));

                        bgciManagerIdElem->appendChild(bgciManagerIdVal);
                        
                        BgciManagerElem->appendChild(bgciManagerIdElem);                       

                         *element=BgciManagerElem;


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

int acs_apbm_nc_dmxfunctionsfilter::getTenants(std::string &xml)
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

	DOMDocument* doc=0;


	if (impl != NULL)
	{

		try
		{
			//#### Managed Element ####

			doc=impl->createDocument(0,X("ManagedElement"),0);

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


		} catch (const OutOfMemoryException&)
		{
			XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
			errorCode = 5;

			if(doc)
			{
				doc->release();
				doc=0;
			}


		} catch (const DOMException& e)
		{
			if(doc)
			{
				doc->release();
				doc=0;
			}


			XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
			errorCode = 2;

		} catch (...)
		{

			if(doc)
			{
				doc->release();
				doc=0;
			}
			XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
			errorCode = 3;
		}
	}// (inpl != NULL)
	else {
		XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
		errorCode = 4;
	}

	return errorCode;
}

//======================================================================
//
// NAME
//      DocumentXml.cpp
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-05-02 by EEDSTL
// CHANGES
//     
//======================================================================

#include <iostream>
#include <sstream>
#include <string>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "ACS_APGCC_CommonLib.h"

#include "acs_hcs_directory.h"
#include "acs_hcs_documentxml.h"

using namespace std;
using namespace xercesc;

namespace AcsHcs
{
	//================================================================================
	// Friends
	//================================================================================

	ostream& operator<<(ostream& os, const DocumentXml& doc)
	{
		return os << doc.getRoot();
	}

	ostream& operator<<(ostream& os, const DocumentXml::Node& node)
	{
		stringstream ss;

		return os << node.toString(ss, 0).str();
	}

	//================================================================================
	// Class DocumentXml
	//================================================================================

	DocumentXml::DocumentXml(const string& fileName) : parser(0), errHandler(0), initiated(false)
	{
		//DEBUG("%s","Entering  DocumentXml::DocumentXml(const string& fileName) ");
		try
		{
			XMLPlatformUtils::Initialize();
			this->initiated = true;

			parser = 0;
			errHandler = 0; // initialized in case parser throws error, shall not be deleted in destructor

			parser = new XercesDOMParser();
			parser->setValidationScheme(XercesDOMParser::Val_Always);
			parser->setDoSchema(true);
			parser->setDoNamespaces(true);    // optional
			parser->setValidationSchemaFullChecking(false);
		
			errHandler = (ErrorHandler*) new HandlerBase();
			parser->setErrorHandler(errHandler);

			this->read(fileName);
		}
		catch (const XMLException& ex)
		{
#if 0
			char* message = XMLString::transcode(ex.getMessage());
			string details = message;
			//DEBUG("%s%s","Entering  DocumentXml::DocumentXml XMLException  ",details);
			XMLString::release(&message);
#endif
			this->xmlTerminate();
			ACS_HCS_THROW(DocumentXml::ExceptionRead, "DocumentXml()", "Error reading file '" << fileName << "'.", "XMLException");
		}
		catch (const Exception&)
		{
			//DEBUG("%s","Entering  DocumentXml::DocumentXml Exception ");
			this->xmlTerminate();
			ACS_HCS_THROW(DocumentXml::ExceptionRead, "DocumentXml()", "Error reading file '" << fileName << "'.", "Exception");

		}
		catch (...)
		{
			//DEBUG("%s","Entering  DocumentXml::DocumentXml ... unknown Exception ");
			this->xmlTerminate();
			ACS_HCS_THROW(DocumentXml::ExceptionRead, "DocumentXml()", "Error reading file '" << fileName << "'.", "Unspecified error.");
		}
		//DEBUG("%s","Leaving  DocumentXml::DocumentXml(const string& fileName) ");
	}

	void DocumentXml::xmlTerminate()
	{
		//DEBUG("%s","Entering  DocumentXml::xmlTerminate() ");
		if (this->parser != 0)
			delete this->parser;

		if (this->errHandler != 0)
				delete this->errHandler;		

		if (this->initiated)
		{
			XMLPlatformUtils::Terminate();
			this->initiated = false;
		}
		//DEBUG("%s","Leaving  DocumentXml::xmlTerminate() ");
	}

	DocumentXml::~DocumentXml()
	{
		//DEBUG("%s","In DocumentXml::~DocumentXml( ");
		this->xmlTerminate();
	}

	DocumentXml::Nodes* DocumentXml::getNodes(Nodes* const nodes, const char* xPath) const
	{
		//DEBUG("%s","In   DocumentXml::Nodes* DocumentXml::getNodes ");
		return DocumentXml::getNodes(nodes, this->getRoot(), xPath);
	}

	DocumentXml::Nodes& DocumentXml::getNodes(Nodes& nodes, const char* xPath) const
	{
		//DEBUG("%s","In DocumentXml::Nodes& DocumentXml::getNodes( ");
		return *(this->getNodes(&nodes, xPath));
	}
	
	DocumentXml::Nodes* DocumentXml::getNodesList(Nodes* const nodes, const char* xPath, const Node& currentLevel ) const
        {
		//DEBUG("%s","In DocumentXml::Nodes* DocumentXml::getNodesList( ");
                return DocumentXml::getNodesList(nodes, this->getRoot(), xPath, currentLevel);
        }
	DocumentXml::Nodes& DocumentXml::getNodesList(Nodes& nodes, const char* xPath, const Node& currentLevel ) const
        {
		//DEBUG("%s","In DocumentXml::Nodes& DocumentXml::getNodesList ");
                return *(this->getNodesList(&nodes, xPath, currentLevel));
        }

	DocumentXml::Node DocumentXml::getNode(const char* xPath, bool strict) const
	{
		//DEBUG("%s"," In DocumentXml::Node DocumentXml::getNode");
		return DocumentXml::getNode(this->getRoot(), xPath, strict);
	}

	DocumentXml::Node DocumentXml::getSpecifiedNode(const char* xPath, const Node& currentLevel, bool strict ) const
        {
		//DEBUG("%s","In  DocumentXml::Node DocumentXml::getSpecifiedNode ");
                return DocumentXml::getSpecifiedNode(this->getRoot(),xPath, currentLevel, strict);
        }

	DocumentXml::Node DocumentXml::getAttribute(const char* name, bool strict) const
	{
		//DEBUG("%s","In DocumentXml::Node DocumentXml::getAttribute ");
		return DocumentXml::getAttribute(this->getRoot(), name, strict);
	}

	DocumentXml::Node DocumentXml::getRoot() const
	{
		//DEBUG("%s","In  DocumentXml::Node DocumentXml::getRoot()  ");
		return this->parser->getDocument()->getDocumentElement();
	}

	string DocumentXml::getName(const Node& node)
	{
		//DEBUG("%s","Entering  string DocumentXml::getName(");
		char* tcName = XMLString::transcode(((const DOMNode*)node)->getNodeName());
		string name = tcName;
		XMLString::release(&tcName);
		//DEBUG("%s","Leaving  string DocumentXml::getName( ");
		return name;
	}

	string DocumentXml::getValue(const Node& node)
	{
		//DEBUG("%s","Entering  string DocumentXml::getValue(  ");
		char* tcName = XMLString::transcode(((const DOMNode*)node)->getTextContent());
		string name = tcName;
		XMLString::release(&tcName);
		//DEBUG("%s","Leaving  string DocumentXml::getValue( ");
		return trim(name);
	}

	DocumentXml::Node DocumentXml::getAttribute(const Node& node, const char* name, bool strict)
	{
		//DEBUG("%s","Entering  DocumentXml::Node DocumentXml::getAttribute( ");
		DOMNamedNodeMap* attrs = ((const DOMNode*)node)->getAttributes();

		if (attrs == 0)
			ACS_HCS_THROW(DocumentXml::ExceptionAttributeUndefined, "getAttribute()", "Undefined attribute '" << name << "'.", "");

		XMLCh* tcName = XMLString::transcode(name);
		DOMNode* attr = attrs->getNamedItem(tcName);
		XMLString::release(&tcName);

		if (attr == 0 && strict)
			ACS_HCS_THROW(DocumentXml::ExceptionAttributeUndefined, "getAttribute()", "Undefined attribute '" << name << "'.", "");
		//DEBUG("%s"," Leaving  DocumentXml::Node DocumentXml::getAttribute(c");
		return attr;
	}

	DocumentXml::Nodes* DocumentXml::getNodes(Nodes* const nodes, const Node& root, const char* xPath)
	{
		//DEBUG("%s","Entering  DocumentXml::Nodes* DocumentXml::getNodes( ");
		XMLCh*          xpath = XMLString::transcode(xPath);
		const DOMNode* temp1_root = root;
		DOMNode* temp2_root = const_cast<DOMNode*> (temp1_root);
		DOMXPathResult* xpres = reinterpret_cast<DOMXPathResult*>(root.getOwnerDocument()->evaluate(xpath, temp2_root, 0, DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE, 0));
		XMLString::release(&xpath);

		if ( !xpres )
		{
			 //DEBUG("%s","Leaving  DocumentXml::Nodes* DocumentXml::getNodes( ");
			 return nodes;
		}

		for (unsigned int i = 0; xpres->snapshotItem(i); i++)
		{
			nodes->append(xpres->getNodeValue());
//Commented for SLES12 environment. Latest xerces doesn't support this function anymore and thif is not being called from anywhere
			//nodes->append(xpres->getSingleNodeValue());
		}
		
		delete xpres;
		//DEBUG("%s","Leaving  DocumentXml::Nodes* DocumentXml::getNodes( ");
		return nodes;

	}
	
	
	DocumentXml::Nodes* DocumentXml::getNodesList(Nodes* const nodes, const Node& root, const char* xPath, const Node& currentLevel)
        {
		//DEBUG("%s","Entering  DocumentXml::Nodes* DocumentXml::getNodesList( ");
		char* token;
        	int levelToTraverse = 0;
        	char delimArray[10][128];

		const DOMNode* temp1_root = root;
                DOMNode* temp2_root = const_cast<DOMNode*> (temp1_root);

		char xPath_temp[256];
		memset(xPath_temp,0, 256);
		strcpy (xPath_temp,xPath);

        	token = strtok(xPath_temp, "/" );
        	if (!strcmp(token,"." ))
        	{
                	token = strtok(NULL, "/");
			strcpy(delimArray[levelToTraverse],token);
			levelToTraverse++;
                        DEBUG("token 2 value is %s",token);

                        if(strcmp(token, "end")== 0)
                        {
                                token = strtok(NULL, "/");
                                strcpy(delimArray[levelToTraverse],token);
                                levelToTraverse++;
                                DEBUG("token 3 value is %s",token);
                        }

			if( currentLevel != NULL)
                	{
                        	const DOMNode* temp1_current = currentLevel;
                        	DOMNode* temp2_current = const_cast<DOMNode*> (temp1_current);
				temp2_root = temp2_current ;

			}
			else
			{
				  //DEBUG("%s","Leaving  DocumentXml::Nodes* DocumentXml::getNodesList( ");
				 //ACS_HCS_THROW(DocumentXml::ExceptionNodeUndefined, "getNodesList()", "Invalid Current Node Handle");
				 cout<<"Invalid Current Node Handle: "<<endl;
				 return NULL;
			}	
        	}
        	else
        	{
                	/* walk through other tokens */
                	while( token != NULL )
                	{
                        	strcpy(delimArray[levelToTraverse],token);
                        	levelToTraverse++;
                        	token = strtok(NULL, "/");
                	}
        	}
		int tokenNo = 0;
       		while ( levelToTraverse > 0 )
        	{
                	//navigate to the path
                	DOMElement* element = dynamic_cast<DOMElement*>(temp2_root);
                	if( element != NULL)
                	{
                        	DOMNodeList* childrenBelowElement = element->getChildNodes();
                         	if ( childrenBelowElement != NULL )
                         	{
                                	XMLSize_t nodeCount =  childrenBelowElement->getLength();
                                 	for ( XMLSize_t it = 0 ; it < nodeCount ; it++ )
                                 	{
                                        	const XMLCh* nodename =  childrenBelowElement->item(it)->getNodeName();
                                         	if( nodename != NULL )
                                         	{
                                                	char* message = XMLString::transcode(nodename);
                                                	if (! strcmp ( message, delimArray[tokenNo]))
                                                	{
                                                        	//found the node
                                                     		temp2_root = childrenBelowElement->item(it);
                                                        	if( levelToTraverse == 1)
								{
									 nodes->append(temp2_root);
                                                	        }
							}
							XMLString::release(&message); 
                                                                                   
                                                }

                                        }
                                }

                        }
			
                	tokenNo++;
               		levelToTraverse -- ;
        	}
		//DEBUG("%s","Leaving  DocumentXml::Nodes* DocumentXml::getNodesList( ");
		return nodes;
	}

	DocumentXml::Node DocumentXml::getNode(const Node& root, const char* xPath, bool strict)
	{
		//DEBUG("%s","Entering  DocumentXml::Node DocumentXml::getNode( ");
		XMLCh* xpath = XMLString::transcode(xPath);
                const DOMNode* temp1_root = root;
                DOMNode* temp2_root = const_cast<DOMNode*> (temp1_root);
		DOMXPathResult* xpres = reinterpret_cast<DOMXPathResult*>(root.getOwnerDocument()->evaluate(xpath, temp2_root, 0, DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE, 0));
		XMLString::release(&xpath);
		DOMNode* node = 0;

		if (!xpres && strict )
		{
			ACS_HCS_THROW(DocumentXml::ExceptionNodeUndefined, "getNode()", "Undefined node '" << DocumentXml::getPath(root) << "/" << xPath << "'.", "");
		}
		else
		{
			DOMNode* node = xpres->getNodeValue();
		//Commented for SLES12 environment. Latest xerces doesn't support this function anymore and thif is not being called from anywhere

		//	node = xpres->getSingleNodeValue();	
			delete xpres;
		}

		if (node == 0 && strict)
			ACS_HCS_THROW(DocumentXml::ExceptionNodeUndefined, "getNode()", "Undefined node '" << DocumentXml::getPath(root) << "/" << xPath << "'.", "");
		//DEBUG("%s","Leaving  DocumentXml::Node DocumentXml::getNode( ");
		return node;


	}


	DocumentXml::Node DocumentXml::getSpecifiedNode(const Node& root, const char* xPath, const Node& currentLevel,bool strict) 	
	{
		//DEBUG("%s","Entering DocumentXml::Node DocumentXml::getSpecifiedNode( ");
		cout << "Inside getSpecified node" << endl;
		char* token;
                int levelToTraverse = 0;
                char delimArray[10][128];

                const DOMNode* temp1_root = root;
                DOMNode* temp2_root = const_cast<DOMNode*> (temp1_root);

                char xPath_temp[256];
                memset(xPath_temp,0, 256);
                strcpy (xPath_temp,xPath);

		token = strtok(xPath_temp, "/" );
		if( token != NULL )
		{	
			if (!strcmp(token,"." ))
			{
				//we will ignore . and go to the next token.
				while( token != NULL )
				{
					token = strtok(NULL, "/" );
					if (token == NULL)
						break;		
					strcpy(delimArray[levelToTraverse],token);
					levelToTraverse++;
				}
				if( currentLevel != NULL)
				{
					const DOMNode* temp1_current = currentLevel;
					DOMNode* temp2_current = const_cast<DOMNode*> (temp1_current);
					temp2_root = temp2_current ;

				}
				else
				{
					 if ( strict )
					 {	
		
						ACS_HCS_THROW(DocumentXml::ExceptionNodeUndefined, "getSpecifiedNode()", "Invalid Current Node Handle" << DocumentXml::getPath(root) << "/" << xPath << "'.", "");

						 cout<<"Invalid Current Node Handle: "<<endl;
					 }
					//DEBUG("%s","Leaving DocumentXml::Node DocumentXml::getSpecifiedNode( ");	
					 return NULL;
				}
			}
			else
			{
				/* walk through other tokens */
				while( token != NULL )
				{
					strcpy(delimArray[levelToTraverse],token);
					levelToTraverse++;
					token = strtok(NULL, "/");
				}
			
			}
		}
		//Now get the node handle
		int tokenNo = 0;
		DOMNode* node = NULL;
		while ( levelToTraverse > 0 )
                {
                        //navigate to the path
                        DOMElement* element = dynamic_cast<DOMElement*>(temp2_root);
                        if( element != NULL)
                        {
                                DOMNodeList* childrenBelowElement = element->getChildNodes();
                                if ( childrenBelowElement != NULL )
                                {
                                        XMLSize_t nodeCount =  childrenBelowElement->getLength();
                                        for ( XMLSize_t it = 0 ; it < nodeCount ; it++ )
                                        {
                                                const XMLCh* nodename =  childrenBelowElement->item(it)->getNodeName();
                                                if( nodename != NULL )
                                                {
                                                        char* message = XMLString::transcode(nodename);
                                                        if (! strcmp ( message, delimArray[tokenNo]))
                                                        {
                                                                //found the node
								temp2_root =  childrenBelowElement->item(it);
								node =  childrenBelowElement->item(it);
								XMLString::release(&message);
								break;	
                                                        }
                                                        XMLString::release(&message);

                                                }

                                        }
                                }

                        }

                        tokenNo++;
                        levelToTraverse -- ;
                }
		if (node == 0 && strict)
                        ACS_HCS_THROW(DocumentXml::ExceptionNodeUndefined, "getSpecifiedNode()", "Undefined node '" << DocumentXml::getPath(root) << "/" << xPath << "'.", "");
		//DEBUG("%s"," Leaving DocumentXml::Node DocumentXml::getSpecifiedNode(");
                cout << "Out of getSpecified node" << endl;
		return node;

	}

	string DocumentXml::getPath(const Node& node, bool isLeaf)
	{
		//DEBUG("%s","Entering  DocumentXml::getPath( ");
		if (!node.isValid() || node.getName() == "#document")
		{
			//DEBUG("%s","Leaving  DocumentXml::getPath( ");
			return "";
		}

		string path = DocumentXml::getPath(node.getParent(), false);
		//DEBUG("%s","Leaving  DocumentXml::getPath( ");
		return (path == "" ? "" : path + "/" + (isLeaf ? "" : node.getName()));
	}

	string& DocumentXml::trim(string& str)
	{
		//DEBUG("%s"," Entering DocumentXml::trim(string& str)");
		str.erase(str.find_last_not_of(" \t\r\n") + 1).erase(0, str.find_first_not_of(" \t\r\n"));

		size_t pos = string::npos;

		while ((pos = str.find_first_of("\t\r\n", ++pos)) != string::npos)
			str.at(pos) = ' ';

		pos = string::npos;

		while ((pos = str.rfind("  ", pos)) != string::npos)
			str.replace(pos, 2, " ");
		//DEBUG("%s","Leaving DocumentXml::trim(string& str) ");
		return str;
	}

	void DocumentXml::read(const string& fileName)
	{
		//DEBUG("%s","Entering  DocumentXml::read(const string& fileName) ");
		Directory dir("");
		cout<<" In DocumentXml::read , filename is "<<fileName<<endl;		
		
		if (!dir.exists(fileName))
		{
			ACS_HCS_THROW(DocumentXml::ExceptionFileNotFound, "read()", "Error reading file '" << fileName << "'.", "File not found.");
		}

		try
		{
			parser->parse(fileName.c_str());
		}
		catch (const XMLException& ex)
		{

			cout<<"Entering DocumentXml::read XMLException  "<<endl;
#if 0
			char* message = XMLString::transcode(ex.getMessage());
			string details = message;
			//DEBUG("%s%s","Entering DocumentXml::read XMLException  ",details);
			
			XMLString::release(&message);
#endif
			ACS_HCS_THROW(DocumentXml::ExceptionRead, "read()", "Error reading file '" << fileName << "'.", "XMLException");
		}
		catch (const DOMException& ex)
		{
			cout<<"Entering DocumentXml::read DOMException"<<endl;
#if 0
			char* message = XMLString::transcode(ex.getMessage());
			string details = message;
			//DEBUG("%s%s","Entering DocumentXml::read DOMException  ",details);
			XMLString::release(&message);
#endif
			ACS_HCS_THROW(DocumentXml::ExceptionRead, "read()", "Error reading file '" << fileName << "'.", "DOMException");
		}
		catch (const SAXParseException& ex)
		{
			cout<<"Entering DocumentXml::read SAXException"<<endl;
			ACS_HCS_THROW(DocumentXml::ExceptionRead, "read()", "Error reading file '" << fileName << "'.", "SAXParseException");
		}
		catch (...)
		{
			//DEBUG("%s","Entering DocumentXml::read ... unknown exception ");
			ACS_HCS_THROW(DocumentXml::ExceptionRead, "read()", "Error reading file '" << fileName << "'.", "Unspecified error.");
		}
		//DEBUG("%s","Leaving  DocumentXml::read(const string& fileName) ");
	}

        std::string DocumentXml::getRulesetIdValue()
        {
		//DEBUG("%s","Entering std::string DocumentXml::getRulesetIdValue() ");
		std::string rs = "";
	        char *message;		
                DOMDocument* xmlDoc = this->parser->getDocument();
		if ( xmlDoc == NULL )
                {
                        cout << "Couldn't get Document Handle in DocumentXml::getRulesetIdValue"<<endl;
                        //DEBUG("%s","Leaving DocumentXml::getRulesetIdValue ");
                        return rs;
                }

                DOMElement* elementRoot = xmlDoc->getDocumentElement();
		if(!elementRoot)
                {
                        cout<<" empty XML document in DocumentXml::getRulesetIdValue"<<endl;
                        //DEBUG("%s","Leaving DocumentXml::getRulesetIdValue ");
                        return rs;
                }

                XMLCh* ATTR_id = XMLString::transcode("id");
                XMLCh* tag_ruleset = XMLString::transcode("ruleset");
		if( XMLString::equals(elementRoot->getTagName(), tag_ruleset))
		{
			const XMLCh* xmlch_id  = elementRoot->getAttribute(ATTR_id);
			message = XMLString::transcode(xmlch_id); 
			if( message!= NULL)
				rs = message;
			XMLString::release(&message);	
		}
		XMLString::release(&tag_ruleset);
		XMLString::release(&ATTR_id);
		cout<<"before returning rs is "<<rs<<endl;
                //DEBUG("%s","Leaving std::string DocumentXml::getRulesetIdValue() ");
		return rs;
        }

	void DocumentXml::updateCategoriesAndRules(MultiMap& theMap)
	{
		//DEBUG("%s","Entering DocumentXml::updateCategoriesAndRules(MultiMap& theMap) ");
		cout << "updateCategoriesAndRules" << endl;
		//Now as the xml file is parsed, read the xml file
		char ruleSet[256];
		memset(ruleSet,0,256);
		DOMDocument* xmlDoc = NULL;
		xmlDoc = parser->getDocument();
		if ( xmlDoc == NULL )
		{
			cout << "Couldn't get Document Handle"<<endl;
			//DEBUG("%s","Leaving DocumentXml::updateCategoriesAndRules(MultiMap& theMap) ");
			return;
		}
		
		DOMElement* elementRoot = xmlDoc->getDocumentElement();
		if(!elementRoot)
		{
			cout<<" empty XML document"<<endl;
			//DEBUG("%s","Leaving DocumentXml::updateCategoriesAndRules(MultiMap& theMap) ");
			return;
		}
		else
		{
			XMLCh* temp = XMLString::transcode("id");
			if(temp != NULL)
			{
				char* attrVal = XMLString::transcode(elementRoot->getAttribute(temp));
				if(attrVal != NULL)
				{
					strcpy(ruleSet,attrVal);
					XMLString::release(&attrVal);
				}
				XMLString::release(&temp);
			}
			cout << "Rule set file: " << ruleSet << endl;	
		}
		
		if(elementRoot->hasChildNodes() == false)
		{
			cout<<"No Children. Empty File"<<endl;
			//DEBUG("%s","LEaving DocumentXml::updateCategoriesAndRules(MultiMap& theMap) ");
			return;
		}

		DOMNodeList* childrenBelowRuleSet = elementRoot->getChildNodes();
		if(childrenBelowRuleSet == NULL)
		{
			cout<<"couldn't get the DOMNodeList handle";
			//DEBUG("%s","Leaving DocumentXml::updateCategoriesAndRules(MultiMap& theMap) ");
			return;
		}

		XMLSize_t nodeCount =  childrenBelowRuleSet->getLength();
		rule *ruleDetails;
		ruleDetails =(rule*) malloc(sizeof( rule));
		for ( XMLSize_t it = 0 ; it < nodeCount ; it++ )
		{
			char* message = XMLString::transcode(((const DOMNode*)childrenBelowRuleSet->item(it))->getNodeName());
			if (!strcmp(message,"rule"))
			{
				getRuleDetails((DOMNode*)childrenBelowRuleSet->item(it), ruleDetails, ruleSet);
				processNStoreEachCategory(childrenBelowRuleSet->item(it),ruleDetails);

			}
			XMLString::release(&message);
		}
		
	        if( ruleDetails != NULL)
        	{
                	free(ruleDetails);
	        }
		theMap = CatRule;
		cout << "Exiting Update" << endl;
		//DEBUG("%s","Leaving DocumentXml::updateCategoriesAndRules(MultiMap& theMap) ");
	}

	void DocumentXml::getRuleDetails(DOMNode* ruleNode, rule* ruleptr, string ruleSet)
	{
		//DEBUG("%s","Entering DocumentXml::getRuleDetails(DOMNode* ruleNode, rule* ruleptr, string ruleSet) ");
	        char ruleno[64];
		memset(ruleno,0,64);
		if( ruleNode != NULL )
		{
			DOMElement* elementRoot = dynamic_cast<DOMElement*>(ruleNode);
			if( elementRoot != NULL )
			{
				XMLCh* temp = XMLString::transcode("id");
				if(temp != NULL)
				{
				       char* attrVal = XMLString::transcode(elementRoot->getAttribute(temp));
				       if( attrVal != NULL )
					{	
						strcpy(ruleno,attrVal);
						XMLString::release(&attrVal);
					}
					XMLString::release(&temp);
				}


				XMLCh* temp1 = XMLString::transcode("name");
				if(temp1 != NULL)
				{
					char* attrVal1 = XMLString::transcode(elementRoot->getAttribute(temp1));
					if(attrVal1 != NULL )
					{
						strcpy(ruleptr->rule_name,attrVal1);
						XMLString::release(&attrVal1);
					}
					XMLString::release(&temp1);
				}

         

				//take the rule id , and convert it to integer no
				//int ruleID = atoi(ruleno);
				string ruleID(ruleno);
				string fullID = ruleSet + "_" + ruleID;
				strcpy(ruleptr->rule_no, ruleID.c_str());
				
				//Now get the slogan node
				DOMNodeList* childrenBelowRule = elementRoot->getChildNodes();
				XMLSize_t nodeCount = 0;
				if ( childrenBelowRule != NULL )
				{
					nodeCount =  childrenBelowRule->getLength();
					for ( XMLSize_t it = 0 ; it < nodeCount ; it++ )
					{
						const XMLCh* nodename = childrenBelowRule->item(it)->getNodeName();
						if( nodename != NULL )
						{
							char* message = XMLString::transcode(nodename);
							if (! strcmp ( message, "slogan"))
							{
								//for every category, read the category value
								DOMElement* sloganNode = dynamic_cast<DOMElement*>(childrenBelowRule->item(it));

								if( sloganNode != NULL )
								{
									char* temp2 = XMLString::transcode(sloganNode->getTextContent());
									strcpy(ruleptr->rule_slogan,temp2);
									XMLString::release(&temp2);
								}
							}
							XMLString::release(&message);
						}
					}
				}
			}
		}
		//DEBUG("%s","Leaving DocumentXml::getRuleDetails(DOMNode* ruleNode, rule* ruleptr, string ruleSet) ");
	}

	void DocumentXml::processNStoreEachCategory(DOMNode* ruleNode, rule* ruleDetails)
	{
		//DEBUG("%s","Entering DocumentXml::processNStoreEachCategory( ");
	        DOMNodeList* childrenBelowRule = NULL;
		const XMLCh* nodename = NULL;

		if( ruleNode != NULL )
		{
			DOMElement* elementRoot = dynamic_cast<DOMElement*>(ruleNode);
			if( elementRoot != NULL )
			{
				childrenBelowRule = elementRoot->getChildNodes();
				XMLSize_t nodeCount = 0;
				if ( childrenBelowRule != NULL )
				{
					nodeCount =  childrenBelowRule->getLength();
					for ( XMLSize_t it = 0 ; it < nodeCount ; it++ )
					{
						nodename =  childrenBelowRule->item(it)->getNodeName();
						if( nodename != NULL )
						{
							char* message = XMLString::transcode(nodename);
							if (! strcmp ( message, "category"))
							{
								//for every category, read the category value
								DOMElement* category = dynamic_cast<DOMElement*>(childrenBelowRule->item(it));
								string value ="";
								if( category != NULL )
								{
									char* temp2 = XMLString::transcode(category->getTextContent());
									value = temp2;
									XMLString::release(&temp2);
									//Now insert the category -rule pair into the map
									insertpair (value, ruleDetails );
								}
							}
							XMLString::release(&message);
						}
					}

				}
			}
		}
		//DEBUG("%s","Leaving DocumentXml::processNStoreEachCategory( ");
	}

	void DocumentXml::insertpair(string category, rule* ruleDetails)
	{
		//DEBUG("%s","Entering DocumentXml::insertpair( ");
		//first we will check if the pair already exists, if yes we will not append
		if ( !ispairexists(category, ruleDetails))
		{
			CatRule.insert(make_pair(category,*ruleDetails));
		}
		//DEBUG("%s","Leaving DocumentXml::insertpair( ");
	}
	
	bool DocumentXml::ispairexists(string category, rule* ruleDetails)
	{
		//DEBUG("%s","Entering DocumentXml::ispairexists( ");
		multimap<string, rule>::const_iterator start, end ;
		//obtain the begining and end range of the elements with key_name that is category.
		start = CatRule.lower_bound(category);
		end =  CatRule.upper_bound(category);
		for (; start != end; ++ start )
		{
			if( (!(strcmp((start->second).rule_no, ruleDetails->rule_no))) &&( !(strcmp((start->second).rule_name, ruleDetails->rule_name)))
					&& ( !(strcmp((start->second).rule_slogan, ruleDetails->rule_slogan ))))
			{
				//DEBUG("%s","Leaving DocumentXml::ispairexists( ");
				//element already exists
				return(true);
			}
		}
		//DEBUG("%s","Leaving DocumentXml::ispairexists( ");
		return (false);
	}




	//================================================================================
	// Class DocumentXml::Node
	//================================================================================

	DocumentXml::Node::Node(const DOMNode* const node) : node(node)
	{
	}

	DocumentXml::Node::Node(const DOMNode& node) : node(&node)
	{
	}

	DocumentXml::Node::Node(const Node& node) : node(node.node)
	{
	}

	DocumentXml::Node::~Node()
	{
	}

	DocumentXml::Node& DocumentXml::Node::operator=(const Node& node)
	{
		//DEBUG("%s","Entering DocumentXml::Node& DocumentXml::Node::operator= ");
		this->node = node.node;
		//DEBUG("%s","Leaving DocumentXml::Node& DocumentXml::Node::operator= ");
		return *this;
	}

	DocumentXml::Node::operator const DOMNode*() const
	{
		//DEBUG("%s","In DocumentXml::Node::operator const DOMNode*() ");
		return this->node;
	}

	string DocumentXml::Node::getName() const
	{
		//DEBUG("%s","In string DocumentXml::Node::getName() ");
		return DocumentXml::getName(*this);
	}

	string DocumentXml::Node::getValue() const
	{
		//DEBUG("%s","In  string DocumentXml::Node::getValue( ");
		return DocumentXml::getValue(*this);
	}

	DocumentXml::Node DocumentXml::Node::getAttribute(const char* name, bool strict) const
	{
		//DEBUG("%s","In   DocumentXml::Node DocumentXml::Node::getAttribute( ");
		return DocumentXml::getAttribute(*this, name, strict);
	}

	DocumentXml::Nodes* DocumentXml::Node::getNodes(Nodes* const nodes, const char* xPath ) const
	{
		//DEBUG("%s","In DocumentXml::Nodes* DocumentXml::Node::getNodes( ");
		return DocumentXml::getNodes(nodes, *this, xPath);
	}

	DocumentXml::Nodes& DocumentXml::Node::getNodes(Nodes& nodes, const char* xPath) const
	{
		//DEBUG("%s","In   DocumentXml::Nodes& DocumentXml::Node::getNodes( ");
		return *(this->getNodes(&nodes, xPath));
	}

 	DocumentXml::Nodes* DocumentXml::Node::getNodesList(Nodes* const nodes, const char* xPath, const Node& currentLevel ) const
        {
		//DEBUG("%s","In  DocumentXml::Nodes* DocumentXml::Node::getNodesList ");
                return DocumentXml::getNodesList(nodes, *this, xPath, currentLevel);
        }
	
        DocumentXml::Nodes& DocumentXml::Node::getNodesList(Nodes& nodes, const char* xPath,  const Node& currentLevel) const
        {
		//DEBUG("%s","In  DocumentXml::Nodes& DocumentXml::Node::getNodesList ");
                return *(this->getNodesList(&nodes, xPath, currentLevel));
        }

	DocumentXml::Node DocumentXml::Node::getNode(const char* xPath, bool strict) const
	{
		//DEBUG("%s","In DocumentXml::Node DocumentXml::Node::getNode ");
		return DocumentXml::getNode(*this, xPath, strict);
	}
	

	DocumentXml::Node DocumentXml::Node::getSpecifiedNode(const char* xPath, const Node& currentLevel,bool strict) const
        {
		//DEBUG("%s","In  DocumentXml::Node DocumentXml::Node::getSpecifiedNode ");
                return DocumentXml::getSpecifiedNode(*this,xPath,currentLevel, strict);
        }

	DocumentXml::Node DocumentXml::Node::getFirstChild() const
	{
		//DEBUG("%s","Entering DocumentXml::Node DocumentXml::Node::getFirstChild() ");
		if (! this->isValid())
		{
			//DEBUG("%s","Leaving DocumentXml::Node DocumentXml::Node::getFirstChild() ");
			return *this;
		}
		const DOMNode* child = this->node->getFirstChild();

		if (child != 0 && child->getNodeType() == DOMNode::ELEMENT_NODE)
		{
			//DEBUG("%s","Leaving DocumentXml::Node DocumentXml::Node::getFirstChild() ");
			return child;
		}
		//DEBUG("%s","Leaving DocumentXml::Node DocumentXml::Node::getFirstChild() ");
		return Node(child).getNextSibling();
	}

	DocumentXml::Node DocumentXml::Node::getNextSibling() const
	{
		//DEBUG("%s","Entering DocumentXml::Node DocumentXml::Node::getNextSibling() ");
		if (! this->isValid())
		{
			//DEBUG("%s","Leaving DocumentXml::Node DocumentXml::Node::getNextSibling( ");
			return *this;
		}

		const DOMNode* sibling = this->node->getNextSibling();

		while (sibling != 0 && sibling->getNodeType() != DOMNode::ELEMENT_NODE)
			sibling = sibling->getNextSibling();
		//DEBUG("%s","LEaving DocumentXml::Node DocumentXml::Node::getNextSibling( ");
		return sibling;
	}

	DocumentXml::Node DocumentXml::Node::getParent() const
	{
		//DEBUG("%s","Entering  DocumentXml::Node DocumentXml::Node::getParent() ");
		if (! this->isValid())
		{
			//DEBUG("%s","Leaving  DocumentXml::Node DocumentXml::Node::getParent() ");
			return *this;
		}
		//DEBUG("%s","Leaving  DocumentXml::Node DocumentXml::Node::getParent() ");
		return this->node->getParentNode();
	}

	bool DocumentXml::Node::isValid() const
	{
		//DEBUG("%s","In DocumentXml::Node::isValid() ");
		return this->node != 0;
	}

	DOMDocument* DocumentXml::Node::getOwnerDocument() const
	{
		//DEBUG("%s","Entering   DOMDocument* DocumentXml::Node::getOwnerDocument() ");
		if (! this->isValid())
		{
			//DEBUG("%s","Leavinf  DOMDocument* DocumentXml::Node::getOwnerDocument() ");
			return 0;
		}
		//DEBUG("%s"," Leaving   DOMDocument* DocumentXml::Node::getOwnerDocument()");

		return this->node->getOwnerDocument();
	}

	string DocumentXml::Node::toString() const
	{
		//DEBUG("%s","Entering DocumentXml::Node::toString() ");
		stringstream s;
		s << *this;
		//DEBUG("%s","Leaving DocumentXml::Node::toString() ");
		return s.str();
	}

	stringstream& DocumentXml::Node::toString(stringstream& ss, unsigned int level) const
	{
		//DEBUG("%s","Entering  DocumentXml::Node::toString( ");
		if (! this->isValid())
		{
			//DEBUG("%s","Leaving  DocumentXml::Node::toString( ");
			return ss;
		}

		ss << "<" << this->getName();

		DOMNamedNodeMap* attrs = ((const DOMNode*)node)->getAttributes();

		if (attrs != 0)
		{
			for (unsigned int i = 0; i < attrs->getLength(); i++)
			{
				Node attr = attrs->item(i);
				ss << " " << attr.getName() << "=\"" << attr.getValue() << "\"";
			}
		}

		ss << ">";

		const Node& child = this->getFirstChild();

		if (child.isValid())
		{
			child.toString(ss, level + 1);
		}
		else
		{
			ss << this->getValue();
		}

		ss << "</" << this->getName() << ">";

		if (level)
			this->getNextSibling().toString(ss, level);
		//DEBUG("%s","Leaving  DocumentXml::Node::toString( ");
		return ss;
	}

	//================================================================================
	// Class DocumentXml::Nodes
	//================================================================================

	DocumentXml::Nodes::Nodes()
	{
	}

	DocumentXml::Nodes::~Nodes()
	{
	}

	void DocumentXml::Nodes::append(const DOMNode* node)
	{
		//DEBUG("%s","Entering  DocumentXml::Nodes::append(const DOMNode* node) ");
		this->nodes.push_back(node);
		//DEBUG("%s","Leaving  DocumentXml::Nodes::append(const DOMNode* node) ");
	}

	DocumentXml::Nodes::Iterator DocumentXml::Nodes::begin() const
	{
		//DEBUG("%s","In   DocumentXml::Nodes::Iterator DocumentXml::Nodes::begin() ");
		return this->nodes.begin();
	}

	DocumentXml::Nodes::Iterator DocumentXml::Nodes::end() const
	{
		//DEBUG("%s","In DocumentXml::Nodes::Iterator DocumentXml::Nodes::end( ");
		return this->nodes.end();
	}

	bool DocumentXml::Nodes::isEmpty() const
	{
		//DEBUG("%s","In DocumentXml::Nodes::isEmpty() ");
		return this->nodes.empty();
	}

	unsigned int DocumentXml::Nodes::size() const
	{
		//DEBUG("%s","In  DocumentXml::Nodes::size() ");
		return this->nodes.size();
	}

	//================================================================================
	// Class DocumentXml::Nodes::Iterator
	//================================================================================

	DocumentXml::Nodes::Iterator::Iterator()
	{
	}

	DocumentXml::Nodes::Iterator::Iterator(std::list<const DOMNode*>::const_iterator& it) : it(it)
	{
	}

        DocumentXml::Nodes::Iterator::Iterator(const std::list<const DOMNode*>::const_iterator& it) : it(it)
        {
        }


	DocumentXml::Nodes::Iterator::~Iterator()
	{
	}

	DocumentXml::Node DocumentXml::Nodes::Iterator::operator*() const
	{
		//DEBUG("%s"," In   DocumentXml::Node DocumentXml::Nodes::Iterator::operator*");
		return *(this->it);
	}

	bool DocumentXml::Nodes::Iterator::operator==(const Iterator& right) const
	{
		//DEBUG("%s","In DocumentXml::Nodes::Iterator::operator== ");
		return (this->it == right.it);
	}

	bool DocumentXml::Nodes::Iterator::operator!=(const Iterator& right) const
	{
		//DEBUG("%s","In DocumentXml::Nodes::Iterator::operator!= ");
		return (!(*this == right));
	}

	DocumentXml::Nodes::Iterator& DocumentXml::Nodes::Iterator::operator++()
	{
		//DEBUG("%s","Entering  DocumentXml::Nodes::Iterator& DocumentXml::Nodes::Iterator::operator++() ");
		++(this->it);
		//DEBUG("%s","Leaving  DocumentXml::Nodes::Iterator& DocumentXml::Nodes::Iterator::operator++() ");
		return *this;
	}
}

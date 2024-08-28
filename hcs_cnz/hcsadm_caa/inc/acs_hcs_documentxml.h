//======================================================================
//
// NAME
//      DocumentXml.h
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
//      Implementation of the Basic XML properties of XERCES Lib.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_DocumentXml_h
#define AcsHcs_DocumentXml_h

#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <map>
#include<vector>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include "acs_hcs_exception.h"


namespace AcsHcs
{
	/**
	* DocumentXml - Representation of an XML-document.
	* DocumentXml provides the functionality to
	* - parse XML-files
	* - get random access to all Nodes and their attributes and values.
	*
	* Example:
	*
	* Sample XML-file:
	* <pre>
	* <?xml version="1.0"?>
	* <xscRoot xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="sample.xsd">
	*     <ExternallyVisible>yes</ExternallyVisible>
	*     <ListeningStates>
	*         <state id="HealthAngel-1" age="31">a-state-1</state>
	*         <hello><![CDATA[unsl]]></hello>
	*         <state id="HealthAngel-2" age="35">a-state-2</state>
	*     </ListeningStates>
	* </xscRoot>
	* </pre>
	* Sample code:
	* <pre>
	* try
	* {
	*     // Read XML-document 'sample.xml'.
	*     DocumentXml doc("sample.xml");
	*
	*     // Retrieve all Nodes tagged 'hello' from the document.
	*     // There is only one in 'sample.xml'.
	*     const DocumentXml::Node& hello = doc.getNode("//hello");
	*     cout << "hello  name=" << hello.getName()  << endl;
	*     cout << "hello value=" << hello.getValue() << endl;
	* 
	*     // Retrieve all Nodes tagged 'state' from the document.
	*     // There are two in 'sample.xml'.
	*     // Iterate over all those Nodes and print their attributes, too.
	*     DocumentXml::Nodes states;
	*     doc.getNodes(states, "//state");
	*     DocumentXml::Nodes::Iterator it;
	* 
	*     for (it = states.begin(); it != states.end(); ++it)
	*     {
	*         const DocumentXml::Node& node = *it;
	*         cout << "state value=" << node.getValue() << endl;
	* 
	*         const DocumentXml::Node& id = node.getAttribute("id");
	*         cout << "state.id: " << id.getName() << "=" << id.getValue() << endl;
	*         const DocumentXml::Node& age = node.getAttribute("age");
	*         cout << "state.age: " << age.getName() << "=" << age.getValue() << endl;
	*     }
	* }
	* catch (const DocumentXml::ExceptionAttributeUndefined& ex)
	* {
	*     cout << ex.getMessage() << endl;
	* }
	* catch (const DocumentXml::ExceptionRead& ex)
	* {
	*     cout << ex.getMessage() << endl;
	* }
	* </pre>
	*/
	class DocumentXml
	{
		friend std::ostream& operator<<(std::ostream& os, const DocumentXml& doc);

	public:
		/**
		* DocumentXml::ExceptionFileNotFound - File not found exception.
		* DocumentXml::ExceptionFileNotFound is thrown in the case the XML-file could not be found.
		*/
		class ExceptionFileNotFound : public Exception
		{
		public:
			ExceptionFileNotFound(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionFileNotFound() throw(){}
		};

		/**
		* DocumentXml::ExceptionRead - Exception reading XML-file.
		* DocumentXml::ExceptionRead is thrown on problems reading an XML-file.
		*/
		class ExceptionRead : public Exception
		{
		public:
			ExceptionRead(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionRead() throw() {}
		};

		/**
		* DocumentXml::ExceptionAttributeUndefined - Exception accessing undefined attribute.
		* DocumentXml::ExceptionAttributeUndefined is thrown when trying to access an undefined Node-attribute.
		*/
		class ExceptionAttributeUndefined : public Exception
		{
		public:
			ExceptionAttributeUndefined(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionAttributeUndefined() throw() {}
		};

		/**
		* DocumentXml::ExceptionNodeUndefined - Exception accessing undefined Node.
		* DocumentXml::ExceptionNodeUndefined is thrown when trying to access an undefined Node.
		*/
		class ExceptionNodeUndefined : public Exception
		{
		public:
			ExceptionNodeUndefined(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionNodeUndefined() throw() {}
		};

		std::string getRulesetIdValue();
		
		struct rule_struct
		{
			char rule_no[64];
			char rule_slogan[256];
			char rule_name[256];
		};

		typedef struct rule_struct rule;
		typedef std::multimap<std::string,rule> MultiMap;
		MultiMap CatRule;
		void updateCategoriesAndRules(MultiMap&);

		void getRuleDetails(xercesc::DOMNode* ruleNode, rule* ruleptr, std::string ruleSet);
		void processNStoreEachCategory(xercesc::DOMNode* ruleNode, rule* ruleDetails);
		void insertpair(std::string category, rule* ruleDetails);
		bool ispairexists (std::string category, rule* ruleDetails);

		class Nodes;

		/**
		* DocumentXml::Node - Representation of a Node of an XML-document.
		* DocumentXml::Node encapsulates all information of a Node in an XML document,
		* such as Element-Nodes and Attribute-Nodes.
		*/
		class Node
		{
			friend std::ostream& operator<<(std::ostream& os, const Node& node);

		public:
			Node(const xercesc::DOMNode* const node);
			Node(const xercesc::DOMNode& node);
			Node(const Node& node);
			~Node();

			/**
			* Assignment operator.
			*/
			Node& operator=(const Node& node);

			/**
			* Conversion operator.
			*/
			operator const xercesc::DOMNode*() const;
			//operator xercesc::DOMNode*();

			/**
			* Get the name of this Node.
			* The name of this Node is its tag-name in the XML-file.
			* @return The name of this Node.
			*/
			std::string getName() const;

			/**
			* Get the value of this Node.
			* The value of this Node is its trimmed text in the XML-file.
			* @return The trimmed value of this Node.
			*/
			std::string getValue() const;

			/**
			* Get attribute identified by parameter 'name'.
			* @param[in] name   The name of the attribute to be returned.
			* @param[in] strict If true, throws exception if the attribute is not defined.
			* @return The matching attribute.
			* @throws DocumentXml::ExceptionAttributeUndefined
			*/
			Node getAttribute(const char* name, bool strict = true) const;

			/**
			* Get Node identified by parameter 'xpath'.
			* @param[in] xPath  The x-path expression filtering the Node to be returned.
			* @param[in] strict If true, throws exception if the Node is not defined.
			* @return The matching Node.
			* @throws DocumentXml::ExceptionNodeUndefined
			*/
			Node getNode(const char* xPath, bool strict = true) const;
			
			  /**
                        * Get Node identified by parameter 'xpath'.
                        * @param[in] xPath  The x-path expression filtering the Node to be returned.
                        * @param[in] strict If true, throws exception if the Node is not defined.
			* @param[in]    currentLevel Pointer to the curret Node handle.
                        * @return The matching Node.
                        * @throws DocumentXml::ExceptionNodeUndefined
                        */
			Node getSpecifiedNode(const char* xPath, const Node& currentLevel,bool strict= true) const;

			/**
			* Get list of Nodes identified by parameter 'xpath'.
			* The Nodes are stored and returned in the user-provided list of Nodes
			* passed in parameter 'nodes'.
			* @param[inout] nodes User-provided list to store the Nodes.
			* @param[in]    xPath The x-path expression filtering the Nodes to be returned.
			* @return The list of Nodes as passed in 'nodes', filled with matching Nodes.
			*/
			Nodes* getNodes(Nodes* const nodes, const char* xPath) const;

			/**
			* Get list of Nodes identified by parameter 'xpath'.
			* The Nodes are stored and returned in the user-provided list of Nodes
			* passed in parameter 'nodes'.
			* @param[inout] nodes User-provided list to store the Nodes to be returned.
			* @param[in]    xPath The x-path expression filtering the Nodes to be returned.
			* @return The list of Nodes as passed in 'nodes', filled with matching Nodes.
			*/
			Nodes& getNodes(Nodes& nodes, const char* xPath) const;
			
			/**
                        * Get list of Nodes identified by parameter 'xpath'.
                        * The Nodes are stored and returned in the user-provided list of Nodes
                        * passed in parameter 'nodes'.
                        * @param[inout] nodes User-provided list to store the Nodes.
                        * @param[in]    xPath The x-path expression filtering the Nodes to be returned.
			* @param[in]    currentLevel Pointer to the curret Node handle.
                        * @return The list of Nodes as passed in 'nodes', filled with matching Nodes.
                        */
                        Nodes* getNodesList(Nodes* const nodes, const char* xPath, const Node& currentLevel  ) const;

			
			/**
                        * Get list of Nodes identified by parameter 'xpath'.
                        * The Nodes are stored and returned in the user-provided list of Nodes
                        * passed in parameter 'nodes'.
                        * @param[inout] nodes User-provided list to store the Nodes to be returned.
                        * @param[in]    xPath The x-path expression filtering the Nodes to be returned.
			* @param[in]	currentLevel Pointer to the curret Node handle. 
                        * @return The list of Nodes as passed in 'nodes', filled with matching Nodes.
                        */
                        Nodes& getNodesList( Nodes& nodes, const char* xPath, const Node& currentLevel ) const;

			


			/**
			* Get the first child of this Node.
			* @return The first child of this Node.
			*/
			Node getFirstChild() const;

			/**
			* Get the next sibling of this Node.
			* @return The next sibling of this Node.
			*/
			Node getNextSibling() const;

			/**
			* Get the parent of this Node.
			* @return The parent of this Node.
			*/
			Node getParent() const;

			/**
			* Find out if this Node is a valid Node.
			* @return true if this Node is a valid Node, otherwise false.
			*/
			bool isValid() const;

			/*xercesc::DOMNode::NodeType getType() const*/
			short getType() const
			{
				return this->node->getNodeType();
			}

			/**
			* Get the owning DOMDocument.
			*/
			xercesc::DOMDocument* getOwnerDocument() const;

			/**
			* Stringify this Node.
			*/
			std::string toString() const;

		protected:
			virtual std::stringstream& toString(std::stringstream& ss, unsigned int level = 0) const;

		private:
			const xercesc::DOMNode* node;
		};

		/**
		* DocumentXml::Nodes - List of Nodes.
		*/
		class Nodes
		{
		public:
			/**
			* DocumentXml::Node::Iterator - Iterator for DocumentXml::Nodes.
			*/
			class Iterator 
			{
			public:
				Iterator();
				Iterator(std::list<const xercesc::DOMNode*>::const_iterator& it);
				Iterator(const std::list<const xercesc::DOMNode*>::const_iterator& it);
				~Iterator();

				Node operator*() const;
				bool operator==(const Iterator& right) const;
				bool operator!=(const Iterator& right) const;
				Iterator& operator++();

			private:
				std::list<const xercesc::DOMNode*>::const_iterator it;
			};

			/**
			* Constructor.
			*/
			Nodes();

			/**
			* Destructor.
			*/
			~Nodes();

			/**
			* Append a Node to this list of Nodes.
			* @param[in] node The Node to be appended.
			*/
			void append(const xercesc::DOMNode* node);

			/**
			* Get an iterator pointing to the beginning of this list of Nodes.
			* @return The iterator to the beginning of this list of Nodes.
			*/
			Iterator begin() const;

			/**
			* Get an iterator pointing to the end of this list of Nodes.
			* @return The iterator to the end of this list of Nodes.
			*/
			Iterator end() const;

			/**
			* Returns whether this list of Nodes is empty, i.e. whether its size is 0.
			* @return Returns true if this list of Nodes is empty.
			*/
			bool isEmpty() const;

			/**
			* Returns the number of Nodes.
			* @return The number of Nodes.
			*/
			unsigned int size() const;

		private:
			Nodes(const Nodes&);
			Nodes& operator=(const Nodes&);

			std::list<const xercesc::DOMNode*> nodes;
		};

		/**
		* Constructor.
		* @param[in] fileName The file name of the XML-file to be read.
		* @throws DocumentXml::ExceptionFileNotFound
		* @throws DocumentXml::ExceptionRead
		*/
		DocumentXml(const std::string& fileName);

		virtual ~DocumentXml(void);

		/**
		* Get attribute identified by parameter 'name'.
		* @param[in] name The name of the attribute to be returned.
		* @param[in] strict If true, throws exception if the attribute is not defined.
		* @return The matching attribute.
		* @throws DocumentXml::ExceptionAttributeUndefined
		*/
		Node getAttribute(const char* name, bool strict = true) const;

		/**
		* Get Node identified by parameter 'xpath'.
		* @param[in] xPath The x-path expression filtering the Node to be returned.
		* @param[in] strict If true, throws exception if the Node is not defined.
		* @return The matching Node.
		* @throws DocumentXml::ExceptionNodeUndefined
		*/
		Node getNode(const char* xPath, bool strict = true) const;
		
		 /**
                * Get Node identified by parameter 'xpath'.
                * @param[in] xPath The x-path expression filtering the Node to be returned.
		* @param[in] currentLevel Pointer to the current Node handle. 
                * @param[in] strict If true, throws exception if the Node is not defined.
                * @return The matching Node.
                * @throws DocumentXml::ExceptionNodeUndefined
                */
		Node getSpecifiedNode(const char* xPath, const Node& currentLevel,bool strict= true) const;	
		/**
		* Get list of Nodes identified by parameter 'xpath'.
		* The Nodes are stored and returned in the user-provided list of Nodes
		* passed in parameter 'nodes'.
		* @param[inout] nodes User-provided list to store the Nodes.
		* @param[in]    xPath The x-path expression filtering the Nodes to be returned.
		* @return The list of Nodes as passed in 'nodes', filled with matching Nodes.
		*/
		Nodes* getNodes(Nodes* const nodes, const char* xPath) const;

		/**
		* Get list of Nodes identified by parameter 'xpath'.
		* The Nodes are stored and returned in the user-provided list of Nodes
		* passed in parameter 'nodes'.
		* @param[inout] nodes User-provided list to store the Nodes to be returned.
		* @param[in]    xPath The x-path expression filtering the Nodes to be returned.
		* @return The list of Nodes as passed in 'nodes', filled with matching Nodes.
		*/
		Nodes& getNodes(Nodes& nodes, const char* xPath) const;

		/**
                * Get list of Nodes identified by parameter 'xpath'.
                * The Nodes are stored and returned in the user-provided list of Nodes
                * passed in parameter 'nodes'.
                * @param[inout] nodes User-provided list to store the Nodes.
                * @param[in]    xPath The x-path expression filtering the Nodes to be returned.
                * @param[in]    currentLevel Pointer to the current Node handle.
                * @return The list of Nodes as passed in 'nodes', filled with matching Nodes.
                */
                Nodes* getNodesList(Nodes* const nodes, const char* xPath, const Node& currentLevel ) const;

                /**
                * Get list of Nodes identified by parameter 'xpath'.
                * The Nodes are stored and returned in the user-provided list of Nodes
                * passed in parameter 'nodes'.
                * @param[inout] nodes User-provided list to store the Nodes to be returned.
                * @param[in]    xPath The x-path expression filtering the Nodes to be returned.
                * @param[in]    currentLevel Pointer to the curret Node handle.
                * @return The list of Nodes as passed in 'nodes', filled with matching Nodes.
                */
                Nodes& getNodesList( Nodes& nodes, const char* xPath, const Node& currentLevel ) const;


		/**
		* Get the root Node of this document.
		* @return The root Node of this document.
		*/
		Node getRoot() const;

	protected:
		static std::string getName(const Node& node);
		static std::string getValue(const Node& node);
		static Node getAttribute(const Node& node, const char* name, bool strict);
		static Node getNode(const Node& root, const char* xPath, bool strict);
		static Node getSpecifiedNode(const Node& root,const char* xPath, const Node& currentLevel,bool strict); 
		static Nodes* getNodes(Nodes* const nodes, const Node& root, const char* xPath);
		static Nodes* getNodesList(Nodes* const nodes, const Node& root, const char* xPath, const Node& currentLevel);
		static std::string getPath(const Node& node, bool isLeaf = true);
		static std::string& trim(std::string& str);

		virtual void read(const std::string& fileName);

	private:
		DocumentXml(const DocumentXml&);
		DocumentXml& operator=(const DocumentXml&);

		xercesc::XercesDOMParser* parser;
		xercesc::ErrorHandler*    errHandler;
		bool initiated;
		void xmlTerminate();

		//xercesc::XercesDOMParser* parser;
		//xercesc::SAXParser* parser;
		//xercesc::ErrorHandler*    errHandler;
	};
}

#endif

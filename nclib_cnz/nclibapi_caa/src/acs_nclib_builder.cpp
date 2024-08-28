/*
 * acs_nclib_builder.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: estevol
 */

#include "acs_nclib_trace.h"
#include "acs_nclib_builder.h"

//#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>


using namespace boost::property_tree;

ACS_NCLIB_TRACE_DEFINE(acs_nclib_builder);

acs_nclib_builder::acs_nclib_builder() {


}

acs_nclib_builder::~acs_nclib_builder() {

}


acs_nclib_rpc_reply* acs_nclib_builder::build_rpc_reply(const char* dump)
{
	ACS_NCLIB_TRACE_FUNCTION;
	//Stream dump data to a stringstream
	std::stringstream ss_reply;
	ss_reply << dump;


	//Define a property tree to contain rpc-reply
	ptree tree_reply;

	//create a new blank rpc-reply
	acs_nclib_rpc_reply* new_reply = 0;

	try
	{
		//Try to read xml structure from data
		read_xml(ss_reply, tree_reply, boost::property_tree::xml_parser::trim_whitespace );
        
        new_reply = build_rpc_reply(tree_reply);
		
	}
	catch(xml_parser_error& e)
	{
        if (new_reply)
        {
            acs_nclib_factory::dereference(new_reply);
            new_reply = 0;
        }
        ACS_NCLIB_TRACE_MESSAGE("ERROR: Build rpc-reply failed! Exception caught, parser error: %s", e.what());

		//std::cout << "Exception caught parser error" << std::endl;
	}
	catch(ptree_bad_path& e)
	{
		if (new_reply)
        {
            acs_nclib_factory::dereference(new_reply);
            new_reply = 0;
        }
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build rpc-reply failed! Exception caught, bad path: %s", e.what());

		//std::cout << "Exception caught bad path" << std::endl;
	}
    


	return new_reply;

}

acs_nclib_rpc* acs_nclib_builder::build_rpc(const char* dump)
{
	ACS_NCLIB_TRACE_FUNCTION;
	//Stream dump data to a stringstream
	std::stringstream ss;
	ss << dump;


	//Define a property tree to contain rpc-reply
	ptree tree_rpc;

	//create a new blank rpc
	acs_nclib_rpc* new_rpc = 0;

	try
	{
		//Try to read xml structure from data
		read_xml(ss, tree_rpc, boost::property_tree::xml_parser::trim_whitespace );

		new_rpc = build_rpc(tree_rpc);
	}

	catch(xml_parser_error& e)
	{
		if (new_rpc)
			acs_nclib_factory::dereference(new_rpc);
		new_rpc = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build rpc failed! Exception caught, parser error: %s", e.what());

		//std::cout << "Exception caught, parser error: " << e.what() << std::endl;
	}
	catch(ptree_bad_path& e)
	{
		if (new_rpc)
			acs_nclib_factory::dereference(new_rpc);
		new_rpc = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build rpc failed! Exception caught, bad path: %s", e.what());

		//std::cout << "Exception caught, bad path: " << e.what() << std::endl;
	}
    

	return new_rpc;

}

acs_nclib_hello* acs_nclib_builder::build_hello(const char* dump)
{
	ACS_NCLIB_TRACE_FUNCTION;
    //Stream dump data to a stringstream
	std::stringstream ss;
	ss << dump;
    
    
	//Define a property tree to contain rpc-reply
	ptree tree_rpc;
    
	//create a new blank rpc
	acs_nclib_hello* new_hello = 0;
    
    try
	{
		//Try to read xml structure from data
		read_xml(ss, tree_rpc, boost::property_tree::xml_parser::trim_whitespace );
        
		new_hello = build_hello(tree_rpc);
        
    }
    catch(xml_parser_error& e)
	{
		if (new_hello)
			acs_nclib_factory::dereference(new_hello);
		new_hello = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build hello failed! Exception caught, parser error: %s", e.what());

		//std::cout << "Exception caught, parser error: " << e.what() << std::endl;
	}
	catch(ptree_bad_path& e)
	{
		if (new_hello)
			acs_nclib_factory::dereference(new_hello);
		new_hello = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build hello failed! Exception caught, bad path: %s", e.what());

		//std::cout << "Exception caught, bad path: " << e.what() << std::endl;
	}
    
    
    return new_hello;
}

acs_nclib_message* acs_nclib_builder::build_message(const char* dump)
{
	ACS_NCLIB_TRACE_FUNCTION;
    //Stream dump data to a stringstream
	std::stringstream ss;
	ss << dump;
    
    
	//Define a property tree to contain rpc-reply
	ptree tree_rpc;
        
    try
	{
		//Try to read xml structure from data
		read_xml(ss, tree_rpc, boost::property_tree::xml_parser::trim_whitespace );
    }
    catch(xml_parser_error& e)
	{
    	ACS_NCLIB_TRACE_MESSAGE("ERROR: Build message failed! Exception caught, parser error: %s", e.what());

		//std::cout << "Exception caught, parser error: " << e.what() << std::endl;
        return 0;
	}
    
    if (tree_rpc.count("hello") > 0)
        return build_hello(tree_rpc);
    else if (tree_rpc.count("rpc") > 0)
        return build_rpc(tree_rpc);
    else if (tree_rpc.count("rpc-reply") > 0)
        return build_rpc_reply(tree_rpc);
    else
        return 0;
}



acs_nclib_rpc_reply* acs_nclib_builder::build_rpc_reply(acs_nclib_tree& tree_reply)
{
	ACS_NCLIB_TRACE_FUNCTION;
    //create a new blank rpc-reply
	acs_nclib_rpc_reply* new_reply = acs_nclib_factory::create_rpc_reply();
    
	try
	{

		std::string str_msgId = tree_reply.get("rpc-reply.<xmlattr>.message-id","");

		unsigned int msgId = atoi(str_msgId.c_str());
		new_reply->set_message_id(msgId);

		//Look for child nodes of rpc-reply
		ptree empty;
		ptree childs = tree_reply.get_child("rpc-reply", empty);
        
		BOOST_FOREACH( ptree::value_type const& v,  childs) {
            
			if (v.first.compare("data") == 0)
			{
				if (new_reply->get_reply_type() == acs_nclib::REPLY_UNKNOWN)
					new_reply->set_reply_type(acs_nclib::REPLY_DATA);
				else if (new_reply->get_reply_type() == acs_nclib::REPLY_ERROR)
					new_reply->set_reply_type(acs_nclib::REPLY_DATA_WITH_ERRORS);
                
				std::string value;
                
				//Dump data content to a stringstream
				std::stringstream data_dump;
				boost::property_tree::xml_writer_settings<char> settings('\t', 1);
				write_xml(data_dump, v.second, settings);
                
				//Remove first line (xml header)
				size_t index = data_dump.str().find_first_of("\n");
				if (index != std::string::npos)
					value = data_dump.str().substr(index+1, std::string::npos);
                
				new_reply->set_data(value.c_str());
                
			}
			else if (v.first.compare("ok") == 0)
			{
				new_reply->set_reply_type(acs_nclib::REPLY_OK);
			}
			else if( v.first.compare("rpc-error" ) == 0)
            {
				acs_nclib_rpc_error* temp = acs_nclib_factory::create_rpc_error();

				if (new_reply->get_reply_type() == acs_nclib::REPLY_UNKNOWN)
					new_reply->set_reply_type(acs_nclib::REPLY_ERROR);
				else if (new_reply->get_reply_type() == acs_nclib::REPLY_DATA)
					new_reply->set_reply_type(acs_nclib::REPLY_DATA_WITH_ERRORS);

				ptree error_tags = v.second.get_child("");
                
				BOOST_FOREACH( ptree::value_type const& e,  error_tags) {
					//std::cout << "Error tag vlaue: " << e.first << "\n";
                    
					if (e.first.compare("error-tag") == 0)
					{
						std::string value = e.second.get_value("");
						temp->set_error_tag(value.c_str());
					}
					else if (e.first.compare("error-app-tag") == 0)
					{
						std::string value = e.second.get_value("");
						temp->set_error_app_tag(value.c_str());
					}
					else if (e.first.compare("error-path") == 0)
					{
						std::string value = e.second.get_value("");
						temp->set_error_path(value.c_str());
					}
					else if (e.first.compare("error-message") == 0)
					{
						std::string value = e.second.get_value("");
						temp->set_error_message(value.c_str());
					}
					else if (e.first.compare("error-info") == 0)
					{
						std::string value;

						//Dump data content to a stringstream
						std::stringstream info_dump;
						boost::property_tree::xml_writer_settings<char> settings('\t', 1);
						write_xml(info_dump, e.second, settings);

						//Remove first line (xml header)
						size_t index = info_dump.str().find_first_of("\n");
						if (index != std::string::npos)
							value = info_dump.str().substr(index+1, std::string::npos);

						//std::string value = e.second.get_value("");
						temp->set_error_info(value.c_str());
					}
					else if (e.first.compare("error-type") == 0)
					{
						std::string value = e.second.get_value("");
                        
						if (value.compare("rpc") == 0)
							temp->set_error_type(acs_nclib::NC_ERR_RPC);
                        
						else if (value.compare("transport") == 0)
							temp->set_error_type(acs_nclib::NC_ERR_TRANSPORT);
                        
						else if (value.compare("protocol") == 0)
							temp->set_error_type(acs_nclib::NC_ERR_PROTOCOL);
                        
						else if (value.compare("application") == 0)
							temp->set_error_type(acs_nclib::NC_ERR_APPLICATION);
                        
						else
							temp->set_error_type(acs_nclib::NC_ERR_UNKNOWN);
                        
					}
					else if (e.first.compare("error-severity") == 0)
					{
						std::string value = e.second.get_value("");
                        
						if (value.compare("error") == 0)
							temp->set_error_severity(acs_nclib::SEVERITY_ERROR);
                        
						else if (value.compare("warning") == 0)
							temp->set_error_severity(acs_nclib::SEVERITY_WARNING);
                        
					}
				}
                
				new_reply->add_rpc_error(temp);
				acs_nclib_factory::dereference(temp);
			}
            
		}
	}
	catch(xml_parser_error& e)
	{
		acs_nclib_factory::dereference(new_reply);
		new_reply = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build rpc-reply failed! Exception caught, parser error: %s", e.what());

		//std::cout << "Exception caught parser error" << std::endl;
	}
	catch(ptree_bad_path& e)
	{
		acs_nclib_factory::dereference(new_reply);
		new_reply = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build rpc-reply failed! Exception caught, bad path: %s", e.what());

		//std::cout << "Exception caught bad path" << std::endl;
	}
    
	return new_reply;
    
}



acs_nclib_rpc* acs_nclib_builder::build_rpc(acs_nclib_tree& tree_rpc)
{
	ACS_NCLIB_TRACE_FUNCTION;
    //create a new blank rpc
	acs_nclib_rpc* new_rpc = 0;
    
	try
	{
		      
		//Look for child nodes of rpc-reply
		ptree empty;
		ptree childs = tree_rpc.get_child("rpc", empty);
        
		BOOST_FOREACH( ptree::value_type const& v,  childs) {
            
			if (v.first.compare("get") == 0)
			{
                
				if (new_rpc == 0)
				{
					new_rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_GET);
				}
                
				ptree get_childs = v.second.get_child("", empty);
                
				BOOST_FOREACH( ptree::value_type const& c,  get_childs) {
                    
					if (c.first.compare("source") == 0)
					{
                        
						if (c.second.count("running") > 0)
						{
							new_rpc->set_source(acs_nclib::DATASTORE_RUNNING);
						}

						else if (c.second.count("candidate") > 0)
						{
							new_rpc->set_source(acs_nclib::DATASTORE_CANDIDATE);
						}
					}
					else if (c.first.compare("filter") == 0)
					{
                        
						std::string type = c.second.get("<xmlattr>.type","");
                        
						if (type.compare("subtree") == 0) {
                            
							//Dump filter content to a stringstream
							std::stringstream filter_dump;
							boost::property_tree::xml_writer_settings<char> settings('\t', 1);
							write_xml(filter_dump, c.second, settings);
                            
							//Remove first line (xml header)
							size_t index = filter_dump.str().find_first_of("\n");
							std::string value;
							if (index != std::string::npos)
                                value = filter_dump.str().substr(index+1, std::string::npos);
                            
                            
							acs_nclib_filter* new_filter = acs_nclib_factory::create_filter(acs_nclib::FILTER_SUBTREE);
							new_filter->set_filter_content(value.c_str());
                            
							new_rpc->add_filter(new_filter);
                            
							acs_nclib_factory::dereference(new_filter);
                            
						}
					}
                    
				}
                
			}
			else if (v.first.compare("get-config") == 0)
			{
				if (new_rpc == 0)
				{
					new_rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_GETCONFIG);
				}
                
				ptree get_childs = v.second.get_child("", empty);
                
				BOOST_FOREACH( ptree::value_type const& c,  get_childs) {
                    
					if (c.first.compare("source") == 0)
					{
						if (c.second.count("running") > 0)
						{
							new_rpc->set_source(acs_nclib::DATASTORE_RUNNING);
						}
						else if (c.second.count("candidate") > 0)
						{
							new_rpc->set_source(acs_nclib::DATASTORE_CANDIDATE);
						}
					}
					else if (c.first.compare("filter") == 0)
					{
                        
						std::string type = c.second.get("<xmlattr>.type","");
                        
						if (type.compare("subtree") == 0) {
                            
							//Dump filter content to a stringstream
							std::stringstream filter_dump;
							boost::property_tree::xml_writer_settings<char> settings('\t', 1);
							write_xml(filter_dump, c.second, settings);
                            
							//Remove first line (xml header)
							size_t index = filter_dump.str().find_first_of("\n");
							std::string value;
							if (index != std::string::npos)
								value = filter_dump.str().substr(index+1, std::string::npos);
                            
                            
							acs_nclib_filter* new_filter = acs_nclib_factory::create_filter(acs_nclib::FILTER_SUBTREE);
							new_filter->set_filter_content(value.c_str());
                            
							new_rpc->add_filter(new_filter);
                            
							acs_nclib_factory::dereference(new_filter);
						}
					}
                    
				}
			}
			else if (v.first.compare("edit-config") == 0)
			{
				if (new_rpc == 0)
				{
					new_rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_EDITCONFIG);
				}
                
				ptree edit_childs = v.second.get_child("", empty);
                
				BOOST_FOREACH( ptree::value_type const& c,  edit_childs) {
                    
					if (c.first.compare("target") == 0)
					{
						if (c.second.count("running") > 0)
						{
							new_rpc->set_target(acs_nclib::DATASTORE_RUNNING);
						}
						else if (c.second.count("candidate") > 0)
						{
							new_rpc->set_target(acs_nclib::DATASTORE_CANDIDATE);
						}
					}
					else if (c.first.compare("config") == 0)
					{
                        
						//Dump config content to a stringstream
						std::stringstream config_dump;
						boost::property_tree::xml_writer_settings<char> settings('\t', 1);
						write_xml(config_dump, c.second, settings);
                        
						//Remove first line (xml header)
						size_t index = config_dump.str().find_first_of("\n");
						std::string value;
						if (index != std::string::npos)
							value = config_dump.str().substr(index+1, std::string::npos);
                        
						new_rpc->set_config(value.c_str());
                        
					}
				}
			}
		}

		if (new_rpc)
		{
			std::string str_msgId = tree_rpc.get("rpc.<xmlattr>.message-id","");

			unsigned int msgId = atoi(str_msgId.c_str());

			new_rpc->set_message_id(msgId);
		}
	}
    
	catch(xml_parser_error& e)
	{
		if (new_rpc)
			acs_nclib_factory::dereference(new_rpc);
		new_rpc = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build rpc failed! Exception caught, parser error: %s", e.what());

		//std::cout << "Exception caught, parser error: " << e.what() << std::endl;
	}
	catch(ptree_bad_path& e)
	{
		if (new_rpc)
			acs_nclib_factory::dereference(new_rpc);
		new_rpc = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build rpc failed! Exception caught, bad path: %s", e.what());

		//std::cout << "Exception caught, bad path: " << e.what() << std::endl;
	}
    
    
	return new_rpc;

}




acs_nclib_hello* acs_nclib_builder::build_hello(acs_nclib_tree& tree_rpc)
{
	ACS_NCLIB_TRACE_FUNCTION;
    //create a new blank rpc
	acs_nclib_hello* new_hello = acs_nclib_factory::create_hello();
    
    try
    {
        //Try to read xml structure from data
        //read_xml(ss, tree_rpc, boost::property_tree::xml_parser::trim_whitespace );
        
        //Look for child nodes of rpc-reply
        ptree empty;
        ptree childs = tree_rpc.get_child("hello", empty);
        
        BOOST_FOREACH( ptree::value_type const& v,  childs) {
            
            if (v.first.compare("capabilities") == 0)
            {
                
                ptree cap_childs = v.second.get_child("", empty);
                
                BOOST_FOREACH( ptree::value_type const& c,  cap_childs) {
                    
                    if (c.first.compare("capability") == 0)
                    {
                        std::string value = c.second.get_value("");
                        new_hello->add_capability(value.c_str());
                        
                    }
                    
                }
            }
            else if (v.first.compare("session-id") == 0)
            {
                std::string str_sessId = v.second.get_value("");

                unsigned int sessId = atoi(str_sessId.c_str());

                new_hello->set_session_id(sessId);
            }
        }
        
    }
    catch(xml_parser_error& e)
    {
        if (new_hello)
            acs_nclib_factory::dereference(new_hello);
        new_hello = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build hello failed! Exception caught, parser error: %s", e.what());

        //std::cout << "Exception caught, parser error: " << e.what() << std::endl;
	}
	catch(ptree_bad_path& e)
	{
		if (new_hello)
			acs_nclib_factory::dereference(new_hello);
		new_hello = 0;
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Build hello failed! Exception caught, bad path: %s", e.what());

		//std::cout << "Exception caught, bad path: " << e.what() << std::endl;
	}
    
    
    return new_hello;
}

acs_nclib_udp* acs_nclib_builder::build_udp_message(const char* dump)
{
	ACS_NCLIB_TRACE_FUNCTION;
	//create a new blank rpc
        acs_nclib_udp* udp = acs_nclib_factory::create_udp(acs_nclib::OP_UDP_GET);
	int data_len = strlen(dump) - MSG_ID_SIZE;
        char temp_msgid[MSG_ID_SIZE+1]={0};
        char* temp_data = new char[data_len + 1];
        memset(temp_data,0,(data_len + 1));

        memcpy(temp_msgid,dump,MSG_ID_SIZE);
        memcpy(temp_data, dump + MSG_ID_SIZE+1, data_len);

        std::string str(temp_data);
        int msg_id = atoi(temp_msgid);

        udp->set_message_id(msg_id);
	udp->set_cmd(str);
	return udp;	
}

acs_nclib_udp_reply* acs_nclib_builder::build_udp_reply_message(const char* dump)
{
	ACS_NCLIB_TRACE_FUNCTION;
	//create a new blank rpc
        acs_nclib_udp_reply* udp_reply = acs_nclib_factory::create_udp_reply(acs_nclib::OP_UDP_REPLY);
	int data_len = strlen(dump) - MSG_ID_SIZE;
	char temp_msgid[MSG_ID_SIZE+1]={0};
	char* temp_data = new char[data_len + 1];
	memset(temp_data,0,(data_len + 1));

	memcpy(temp_msgid,dump,MSG_ID_SIZE); 
	memcpy(temp_data, dump + MSG_ID_SIZE+1, data_len);

	std::string str(temp_data);
	int msg_id = atoi(temp_msgid);

	udp_reply->set_message_id(msg_id);

	udp_reply->set_data(str);
	delete[] temp_data;
	return udp_reply;	
}

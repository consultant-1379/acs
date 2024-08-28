/*=================================================================== */
   /**
   @file fcsc_Common.cpp

   Class method implementation for SWUpdate module.

   This module contains the implementation of class declared in
   the fcsc module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#ifndef FCSC_PCF_COMMON_H
#include "fcsc_Common.h"
#endif

#include "fcsc_CompatibilityChecker.h"

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
using namespace std;
const string fcsc_Config::FCSC_ROOT_DIR("/cluster");
string fcsc_Config::FCSC_DOWNLOAD_PATH("");
const string fcsc_Config::FCSC_FTP_PATH("/cluster/data/FTPVOL/");
const string fcsc_Config::FCSC_SFTP_PATH("/cluster/data/SFTPVOL/");
const string fcsc_Config::FCSC_APG_REPOSITORY_PATH("/cluster/data/APGREPO/");
const string fcsc_Config::FCSC_DOWNLOAD_DIR_NAME("new_CSP/");
const string fcsc_Config::FCSC_APG_UPDCSP_DIR("updated_CSP/");
const string fcsc_Config::FCSC_APG_CURCSP_DIR("current_CSP/");
const string fcsc_Config::FCSC_PCF_FILE_NAME("PCF.xml");
const string fcsc_Config::FCSC_CHKSUM_FILE_PATH("./Chksum");
const string fcsc_Config::FCSC_CONFIG_FILE_PATH("./SWUpdate_Config");
const string fcsc_Config::FCSC_PCF_BUNDLE_NAME("PCFBundle.tar");

/*===================================================================
   ROUTINE: exec_cmd
=================================================================== */
int fcsc_Config::exec_cmd(string cmd, const char *mode)
{
    	FILE *fp;
     	int status;
     	char path[10000];

     	fp = popen(cmd.c_str(), mode);
         if (fp == NULL)
         {
     		cout << "Error in executing the command" << endl;
     		return -1;
     	}
     	while (fgets(path, 10000, fp) != NULL)
     		    printf("%s", path);

     	status = pclose(fp);
     	//int ret=WEXITSTATUS(status);
     	//cout << "ret = " << ret <<endl;
     	//return ret;
     	if (WIFEXITED(status) )
     	{
     	      int ret=WEXITSTATUS(status);
     	      return ret;
     	}
     	return 0;
}


/* end exec_cmd */

/*===================================================================
   ROUTINE: PCF_Util
=================================================================== */
PCF_Util::PCF_Util(){}
/* end PCF_Util */

/*===================================================================
   ROUTINE: getUpdatedSDPList
=================================================================== */

int PCF_Util::getUpdatedSDPList(vector<SdpRecord> &sdpList)
{
    string file_path = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_CURCSP_DIR + fcsc_Config::FCSC_PCF_FILE_NAME;
    //TO DO: take vector<SdpRecord> as ref parameter to this function & fill that in case parsePCF is successful, else return Error.
    if (parsePCF((char *)file_path.c_str(), true) == -1)
         return -1;
    file_path.clear();
    file_path = file_path = fcsc_Config::FCSC_DOWNLOAD_PATH + fcsc_Config::FCSC_PCF_FILE_NAME;

    if (parsePCF((char *)file_path.c_str()) != 0)
        return -1;
    /*int i;
    for (i=0; i < updatedSdps.size();i++)
      cout << "****: " << updatedSdps[i].sdpName <<": " <<  updatedSdps[i].rState <<": "<< updatedSdps[i].signature << endl;*/
    sdpList = updatedSdps;
    return 0;
}
/* end getUpdatedSDPList*/

/*===================================================================
   ROUTINE: parsePCF
=================================================================== */
int PCF_Util::parsePCF(const char* fileName, bool useMap)
{

        xmlDoc         *doc = NULL;
        xmlNode        *root_element = NULL;
        //const char     *fileName = "./PCF.xml";
        if (useMap)
           Packages.clear();

        doc = xmlReadFile(fileName, NULL, 0);

        if (doc == NULL)
          {
                  printf("error: could not parse file %s\n", fileName);
                  return -1;
          }
        else
          {
                  // Get the root element node
                  root_element = xmlDocGetRootElement(doc);

                  printElementNames(root_element, useMap);

                  /*for( map<string, SdpInfo>::iterator ii=Packages.begin(); ii!=Packages.end(); ++ii)
                  {
                         cout << "parcePCF : " << (*ii).first << ": " << (*ii).second.rState << ": "<< (*ii).second.signature << endl;
                  }*/
                  /*string s = string("CXC456_3.1.1.sdp");
                  cout << "My map entry : " << Packages[s].rState << endl; */

                  /*
                   * free the document
                   */
                  xmlFreeDoc(doc);;
          }
        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();
        //cout << "exiting parsepcf" << endl;
        return 0;

}

/* end parsePCF*/

/* Recursive function that prints the XML structure */
/*===================================================================
   ROUTINE: printElementNames
=================================================================== */

void PCF_Util::printElementNames(xmlNode * a_node, bool useMap)
{
    xmlNode *cur_node = NULL;
    SdpInfo temp;
    string key;
    SdpRecord rec;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
           if (!xmlStrcmp(cur_node->name, (const xmlChar *)SDPNAME_TAG))
                 key = string((const char*)cur_node->xmlChildrenNode->content);
           if (!xmlStrcmp(cur_node->name, (const xmlChar *)RSTATE_TAG))
                 temp.rState = string((const char*)cur_node->xmlChildrenNode->content);
           if (!xmlStrcmp(cur_node->name, (const xmlChar *)SDPSIZE_TAG))
                 temp.size = atoll((const char*)cur_node->xmlChildrenNode->content);
           if (!xmlStrcmp(cur_node->name, (const xmlChar *)SIGNATURE_TAG))
           {
                 temp.signature = string((const char*)cur_node->xmlChildrenNode->content);

                 if (useMap)
                   Packages[key] = temp;
                 else //if(Packages[key].rState != temp.rState)
                 {
                   map<string, SdpInfo>::iterator iter = Packages.begin();
                   iter = Packages.find(key);
                   if (iter != Packages.end())
                   {
                       if (iter->second.rState != temp.rState)
                       {
                           //cout << "Updated Sdp : " << key << ": " << temp.rState << ": " << temp.signature << endl;
                           rec.sdpName = key; rec.rState = temp.rState;
                           rec.size = temp.size; rec.signature = temp.signature;
                           updatedSdps.push_back(rec);
                       }
                   }
                   else
                   {
                       //cout << "New Sdp : " << key << ": " << temp.rState << ": " << temp.signature << endl;
                       rec.sdpName = key; rec.rState = temp.rState;
                       rec.size = temp.size; rec.signature = temp.signature;
                       updatedSdps.push_back(rec);
                   }
                 }
           }
        }
        /*{
            printf("node type: Element, name: %s\n", cur_node->name);
            printf("node type: Element, content: %s\n", cur_node->xmlChildrenNode->content);


        }*/

        printElementNames(cur_node->children, useMap);
    }
}
/* end printElementNames*/

/*===================================================================
   ROUTINE: createSDPChksumFile
=================================================================== */

int SDP_IntegrityCheck:: createSDPChksumFile()
{

     FILE *fp = ACE_OS::fopen (ACE_TEXT(fcsc_Config::FCSC_CHKSUM_FILE_PATH.c_str()),ACE_TEXT("w"));

     if (fp == NULL)
        return -1;

     PCF_Util upd;
     vector<SdpRecord> updatedSdps;
     upd.getUpdatedSDPList(updatedSdps);

     for(unsigned int i=0; i < updatedSdps.size(); i++)
     {
         char *record = new char[updatedSdps[i].signature.length() + 2 + fcsc_Config::FCSC_DOWNLOAD_PATH.length()
                                 + updatedSdps[i].sdpName.length() + 2];
         strcpy(record, updatedSdps[i].signature.c_str());
         strcat(record, "  ");
         strcat(record, fcsc_Config::FCSC_DOWNLOAD_PATH.c_str());
         strcat(record, updatedSdps[i].sdpName.c_str());
         strcat(record, "\n");
         record[updatedSdps[i].signature.length() + 2 + fcsc_Config::FCSC_DOWNLOAD_PATH.length() +
         updatedSdps[i].sdpName.length() + 1] = '\0';

         if (ACE_OS::fprintf(fp, "%s", record) < 0)
         {
        	 delete[] record;
             return -1;
         }
         delete[] record;
     }

     if (ACE_OS::fclose(fp) != 0)
         return -1;

     //fp.close();
     return 0;
}
/* end createSDPChksumFile*/

/*===================================================================
   ROUTINE: checkSDPIntegrity
=================================================================== */
int SDP_IntegrityCheck::checkSDPIntegrity()
{
	if (createSDPChksumFile() == -1)
		return -1;
	fcsc_Config cfg;
	string cmd = string("sha1sum -c ") + fcsc_Config::FCSC_CHKSUM_FILE_PATH + string(" 2>&1");
		if (cfg.exec_cmd(cmd) != 0)
		{
			cout << "The downloaded packages are corrupt" << endl;
			return -1;
		}
		return 0;
}
/* end checkSDPIntegrity*/

/*===================================================================
   ROUTINE: writeToCfgFile
=================================================================== */
int fcsc_Config::writeToCfgFile(string parameter,string value)
{

        FILE *fp = ACE_OS::fopen (ACE_TEXT(fcsc_Config::FCSC_CONFIG_FILE_PATH.c_str()),ACE_TEXT("w"));

        if (fp == NULL)
                return -1;

        char *record = new char[parameter.length() + 2 +
                                 + value.length() + 2 ];
        memset(record,'\0',parameter.length() + 2 +
                                 + value.length() + 2 );
        strcpy(record, parameter.c_str());
        strcat(record, "  ");
        strcat(record, value.c_str());
        strcat(record, "\n");

        if (ACE_OS::fprintf(fp, "%s", record) < 0)
        {
                perror("fwrite");
        }
        delete[] record;

        if(ACE_OS::fclose(fp) < 0)
                perror("fclose");

        return 0;
}
/* end checkSDPIntegrity*/

/*===================================================================
   ROUTINE: readFromCfgFile
=================================================================== */
int fcsc_Config::readFromCfgFile(const string& parameter,string& value)
{
        FILE *fp = ACE_OS::fopen (ACE_TEXT(fcsc_Config::FCSC_CONFIG_FILE_PATH.c_str()),ACE_TEXT("r"));

        if (fp == NULL)
        {
                perror(" filename");
                return -1;
        }
        char line[128];
        while ( ACE_OS::fgets ( line, sizeof(line), fp ) != NULL ) /* read a line */
        {
                char* new_begin = 0;
                char* new_end = 0;

                new_begin = line;
                new_end = strchr(line,' ');


                int len = 0;
                len = new_end - new_begin + 1;
                char* word = new char[len];
                memset(word,'\0',len);
                strncpy(word,line,len - 1);

                if((strcmp(word,parameter.c_str()) == 0))
                {

						new_begin = new_end + 2;
                        new_end = strchr(new_begin,'\n');
                        int length = new_end - new_begin + 1;
                        char* temp = new char[length];
                        memset(temp,'\0',length);
                        strncpy(temp,new_begin,length - 1 );

                        value = string(temp);
                        ACE_OS::fclose ( fp );

			// clean up
                        delete[] word;
                        delete[] temp;
                        return 0;
                }

                memset(line,'\0',128);
		delete word;
        }
        ACE_OS::fclose ( fp );
	return -1;
}
/* end readFromCfgFile*/

//******************************************************************************

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2001-2004
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------


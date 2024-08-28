
/*=================================================================== */
   /**
   @file fcsc_CompatibilityChecker.cpp

   Class method implementation for SWUpdate module.

   This module contains the implementation of class declared in
   the SWUpdate module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
===================================================================== */
#include "fcsc_CompatibilityChecker.h"

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: Sw_Compatibility_Chk
=================================================================== */
Sw_Compatibility_Chk::Sw_Compatibility_Chk()
{
    cur_version.clear();
    //new_version = parse_pcf("new_CSP/PCF.xml","revision");//TODO: replace "new_CSP"/"current" with a constant common class member
    string file_path = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_CURCSP_DIR + fcsc_Config::FCSC_PCF_FILE_NAME;

    ACE_TCHAR* arr = new ACE_TCHAR[strlen(file_path.c_str()) + 1];
    strcpy(arr, file_path.c_str()); 
    //cur_version = parse_pcf((ACE_TCHAR *)file_path.c_str(),"revision");//TODO: replace "new_CSP"/"current" with a constant common class member
    ACE_TCHAR rev[] = "revision";
    cur_version = parse_pcf(arr, rev);//TODO: replace "new_CSP"/"current" with a constant common class member
    delete[] arr;
    supported_versions.clear();
    file_path.clear();
    file_path = fcsc_Config::FCSC_DOWNLOAD_PATH + fcsc_Config::FCSC_PCF_FILE_NAME;
    arr = new ACE_TCHAR[strlen(file_path.c_str()) + 1];
    strcpy(arr, file_path.c_str());
    ACE_TCHAR ver[] = "version"; 
    supported_versions = parse_pcf(arr,ver);
    delete[] arr;
    file_path.clear();
}

/* end Sw_Compatibility_Chk */

/*===================================================================
   ROUTINE: Sw_Compatibility_Chk
=================================================================== */
Sw_Compatibility_Chk::~Sw_Compatibility_Chk()
{
    cur_version.clear();
    supported_versions.clear();
}
/* end ~Sw_Compatibility_Chk */

/*===================================================================
   ROUTINE: get_elements
=================================================================== */
void Sw_Compatibility_Chk::get_elements(xmlNode * a_node, ACE_TCHAR* tag, vector<string> &version_data, ACE_TCHAR* desired_content )
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
                if (!xmlStrcmp(cur_node->name, (const xmlChar *)tag))
                {
                    if(cur_node->xmlChildrenNode != NULL)
                    {
                        if (desired_content != NULL)
                        {
                           if ( !strncasecmp(desired_content, (ACE_TCHAR*)cur_node->xmlChildrenNode->content, strlen(desired_content)) )
                           {
                               //printf(" the %s tag  = %s\n",tag,((const ACE_TCHAR*)cur_node->xmlChildrenNode->content));
                               version_data.push_back((ACE_TCHAR* )cur_node->xmlChildrenNode->content);
                           }
                        }
                        else
                        {
                               ///printf(" the %s tag  = %s\n",tag,((const ACE_TCHAR*)cur_node->xmlChildrenNode->content));
                               version_data.push_back((ACE_TCHAR* )cur_node->xmlChildrenNode->content);
                        }
                    }
                    else
                    {
                       version_data.push_back("");
                    }
                }
        }
        get_elements(cur_node->children,tag,version_data, desired_content);
    }
}
/* end ~Sw_Compatibility_Chk */
/*===================================================================
   ROUTINE: parse_pcf
=================================================================== */
vector<string> Sw_Compatibility_Chk::parse_pcf(ACE_TCHAR* file_name , ACE_TCHAR* tag, ACE_TCHAR * desired_content )
{
        xmlDoc *doc = NULL;
        xmlNode *root_element = NULL;
        vector<string> version_data;
        if (file_name == NULL || !strcmp(file_name,"") )
        {
            version_data.clear();
            return version_data;
        }    
        doc = xmlReadFile(file_name, NULL, 0);
        if (doc == NULL)
        {
            printf("error: could not parse file %s\n", file_name);
            version_data.clear();
            return version_data;
        }
        /*Get the root element node */
        root_element = xmlDocGetRootElement(doc);

        get_elements(root_element,tag,version_data, desired_content);

         /*free the document */
        xmlFreeDoc(doc);
        /*
        *Free the global variables that may
        *have been allocated by the parser.
        */
        xmlCleanupParser();

        return version_data;
}
/* end parse_pcf */
/*===================================================================
   ROUTINE: parseVersion
=================================================================== */
ACE_INT32 Sw_Compatibility_Chk::parseVersion(const ACE_TCHAR* version, vector<string> &parsed_version)
{
    //TODO : to check for error scenarion
    if (version == NULL || !strcmp(version, ""))
        return -1;
    ACE_TCHAR *new_beg, *new_end;
    new_beg = new_end = 0;
    ACE_TCHAR *new_temp = NULL;
    new_beg = (ACE_TCHAR *)version;
    parsed_version.clear();

    while(true)
    {
        new_end = strchr(new_beg, '.');
        if (new_end == NULL)
        {
            new_temp = new ACE_TCHAR[strlen(new_beg) + 1];
            strcpy(new_temp, new_beg);
            new_temp[strlen(new_beg)] = '\0';
            parsed_version.push_back(string(new_temp));
            //cout << "new_temp : " << new_temp << endl;
            delete[] new_temp;
            return 0;
        }
        else
        {
            ACE_INT32 len = new_end - new_beg + 1;
            new_temp = new ACE_TCHAR[len];
            strncpy(new_temp, new_beg, len -1);
            new_temp[len-1] = '\0';
            new_beg = new_end + 1;
            parsed_version.push_back(string(new_temp));
            //cout << "new_temp : " << new_temp << endl;
            delete[] new_temp;
        }
    }

}
/* end parseVersion */

/*===================================================================
   ROUTINE: wildCardCompare
=================================================================== */
ACE_INT32 Sw_Compatibility_Chk::wildCardCompare()
{
    if (cur_version.empty() ||  supported_versions.empty())
        return -1;

    vector<string> parsed_cur_version, parsed_supp_version;

    if (parseVersion(cur_version[0].c_str(), parsed_cur_version) == -1)
    {
        //cout << "error" << endl;
        return -1;
    }

    vector<string>::const_iterator iter;
//    cout << "new V : " << cur_version[0].c_str() << endl;

    for(iter=supported_versions.begin(); iter!=supported_versions.end(); iter++)
    {

       if (parseVersion(iter->c_str(), parsed_supp_version) == 0)//success
       {
           //compare the no of elements in both
           if (parsed_cur_version.size() != parsed_supp_version.size())
           {
               return -1;
           }
           else //if anyone matches we should return
           {
               ACE_UINT32 iter;
               //loop for checking each supported version
               for(iter = 0; iter < parsed_cur_version.size(); iter++)
               {
                  if(parsed_supp_version[iter] != string("*") && parsed_supp_version[iter] != parsed_cur_version[iter])//mismatch
                     break;
               }
               if (iter >= parsed_cur_version.size())//match found
                   return 0;
           }
       }
    }
    return -1;
}
/* end wildCardCompare */
/*===================================================================
   ROUTINE: verifyCampaign
=================================================================== */
ACE_INT32 Sw_Compatibility_Chk::verifyCampaign()
{
    vector<string> campaign_entry;
    campaign_entry.clear();
    string file_path = fcsc_Config::FCSC_DOWNLOAD_PATH + fcsc_Config::FCSC_PCF_FILE_NAME;
    ACE_TCHAR* arr = new ACE_TCHAR[strlen(file_path.c_str()) + 1];
    strcpy(arr, file_path.c_str());
    ACE_TCHAR typ[] = "type";
    ACE_TCHAR cam[] = "campaign"; 
    campaign_entry = parse_pcf(arr,typ, cam);//TODO: replace "new_CSP"/"current" with a constant common class member
    delete[] arr;
    //campaign_entry = parse_pcf(string(file_path),"type", "campaign");//TODO: replace "new_CSP"/"current" with a constant common class member
    file_path.clear();
    if (campaign_entry.empty())
        return -1;
    return 0;      
}
/* end verifyCampaign */
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

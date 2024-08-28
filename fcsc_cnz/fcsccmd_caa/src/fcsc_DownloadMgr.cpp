/*=================================================================== */
   /**
   @file fcsc_DownloadMgr.cpp

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
#include "fcsc_DownloadMgr.h"
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: fcsc_DownloadMgr
=================================================================== */
fcsc_DownloadMgr::fcsc_DownloadMgr()
{
	fcsc_Config::FCSC_DOWNLOAD_PATH = "";
}
/* end Sw_Compatibility_Chk */
/*===================================================================
   ROUTINE: fcsc_DownloadMgr
=================================================================== */
fcsc_DownloadMgr::fcsc_DownloadMgr(ACE_TCHAR* protocol)
{
    
    if (!strcasecmp(protocol, "FTP") )/*&& stat(fcsc_Config::FCSC_FTP_PATH.c_str(), &sb) == 0 for demo*/
    {
    	fcsc_Config::FCSC_DOWNLOAD_PATH = fcsc_Config::FCSC_FTP_PATH;
    }
    else
    {
    	fcsc_Config::FCSC_DOWNLOAD_PATH = fcsc_Config::FCSC_SFTP_PATH;
    }
    //fcsc_Config::FCSC_DOWNLOAD_PATH += '/';
    fcsc_Config::FCSC_DOWNLOAD_PATH += fcsc_Config::FCSC_DOWNLOAD_DIR_NAME;
}
/* end fcsc_DownloadMgr */

/*===================================================================
   ROUTINE: printDownloadPath
=================================================================== */
void fcsc_DownloadMgr::printDownloadPath()
{
   cout << "download_path: " << fcsc_Config::FCSC_DOWNLOAD_PATH << endl;
}
/* end printDownloadPath */

/*===================================================================
   ROUTINE: cleanDownloadFolder
=================================================================== */
ACE_INT32 fcsc_DownloadMgr::cleanDownloadFolder()
{

	string cmd = string("rm -rf ") + fcsc_Config::FCSC_DOWNLOAD_PATH + string(" 2>&1");
	fcsc_Config cfg;
	cfg.exec_cmd(cmd);
	cmd.clear();
	cmd = string("mkdir ") + fcsc_Config::FCSC_DOWNLOAD_PATH + string(" 2>&1");
	if (cfg.exec_cmd(cmd) != 0)
	{
		return -1;
	}
	cmd.clear();
	cmd = string("chmod 755 ") + fcsc_Config::FCSC_DOWNLOAD_PATH + string(" 2>&1");
	if (cfg.exec_cmd(cmd) != 0)
	{
		return -1;
	}

    return 0;
}
/* end cleanDownloadFolder */

/*===================================================================
   ROUTINE: my_fwrite
=================================================================== */
size_t fcsc_DownloadMgr::my_fwrite(void * buffer,size_t size,size_t nmeb, void *stream)
{
    struct FtpFile *out=(struct FtpFile *)stream;
    if(out && !out->stream)
    {
        out->stream=fopen(out->filename,"wb");
        if(!out->stream)
           return -1;
    }
    return fwrite(buffer,size,nmeb,out->stream);
}
/* end my_fwrite */

/*===================================================================
   ROUTINE: download
=================================================================== */
int fcsc_DownloadMgr::download(string url,string uid,string pwd,vector<SdpRecord>files)
{
   CURL *curl;
   CURLcode res;
   int count=1;
   string usrpwd=uid+":"+pwd;

   for(ACE_UINT32 j=0;j<files.size();j++)
   {

        string temp=files[j].sdpName;
        cout<<"**************************Downloading file: ";
        cout<<temp<<"******************"<<endl<<endl;

        struct FtpFile ftpfile={
        temp.c_str(),NULL };/* name to store the file as if succesful */


        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if(curl)
        {
            string complete_path=url+"/"+temp;
            curl_easy_setopt(curl, CURLOPT_URL,complete_path.c_str());
//          curl_easy_setopt(curl,CURLOPT_URL,"sftp://172.16.67.222/dharm/sprint3");
            curl_easy_setopt(curl,CURLOPT_USERPWD,usrpwd.c_str());
//          curl_easy_setopt(curl,CURLOPT_NETRC,1L);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fcsc_DownloadMgr::my_fwrite);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

            curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1L);
            curl_easy_setopt(curl, CURLOPT_VERBOSE,1L);

            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if(CURLE_OK != res)
            {
               /* we failed */
               if(count==3)
               {
                  cout<<"Error not resolved.........Quiting............";
                  return -1;
               }
               else
               {
                  sleep(5);
                  count++;
                  cout<<count << ":  some error occured while downloading......"<<endl;
                  cout<<"Trying to resume the transfer............";
                  fprintf(stderr, "curl told us %d\n", res);
                  j--;
                  continue;
               }
            }

            if(ftpfile.stream)
            fclose(ftpfile.stream); /* close the local file */

            curl_global_cleanup();

        }//end outer if
   }//end for
   return 0;
}
/* end download */

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

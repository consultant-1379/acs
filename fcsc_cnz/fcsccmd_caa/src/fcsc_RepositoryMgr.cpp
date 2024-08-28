/*=================================================================== */
   /**
   @file fcsc_RepositoryMgr.cpp

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
==================================================================== */
#include "fcsc_RepositoryMgr.h"
#include <sys/sendfile.h>

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
using namespace std;

/*===================================================================
   ROUTINE: APG_RepositoryMgr
=================================================================== */
APG_RepositoryMgr::APG_RepositoryMgr(const ACE_TCHAR* pcf_filepath)
{
  ACE_INT32 retValue = parsePCF(pcf_filepath,true);
  src_sdp_list.clear();
  //TODO: Check free space for Updated_CSP creation
  if (retValue == -1 )
  {
      //src_sdp_list[0];
      cout << "Parsing of PCF file is failed\n";
  }

}
/* end APG_RepositoryMgr */

/*===================================================================
   ROUTINE: APG_RepositoryMgr
=================================================================== */
APG_RepositoryMgr::APG_RepositoryMgr(vector<SdpRecord> sdp_list, bool bcreate)
{
	//Delete & create updated_CSP directory
	if (bcreate)
	{
	  string upd_path = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_UPDCSP_DIR;
	  string cmd = string("rm -rf ") + upd_path + string(" 2>&1");
	  fcsc_Config cfg;
	  cfg.exec_cmd(cmd);
	  mode_t pmask = umask(0000);
	  mkdir(upd_path.c_str(), 00755);
	  umask(pmask);
	}

	if (!sdp_list.empty())
	{
		src_sdp_list.clear();
	}
	//else

	   for( ACE_UINT32 i = 0; i < sdp_list.size(); i++ )
	   {
	      /*if (sdp_list[i].sdpName == "" ||  sdp_list[i].sdpName.empty())
	      {
	    	  src_sdp_list.clear();
	    	  break;
	      }
	      else*/
	      {
	    	  //cout << "updated SDP added : " << sdp_list[i].sdpName << endl;
	    	  src_sdp_list.push_back(sdp_list[i].sdpName);
	      }
	   }

}
/* end APG_RepositoryMgr */
/*===================================================================
   ROUTINE: ~APG_RepositoryMgr
=================================================================== */
APG_RepositoryMgr::~APG_RepositoryMgr()
{
   src_sdp_list.clear();
}
/* end ~APG_RepositoryMgr */
/*===================================================================
   ROUTINE: APG_RepositoryMgr
=================================================================== */
ACE_INT32 APG_RepositoryMgr::get_SrcSdpList(/* vector<SdpRecord> updatedList */)
{
   vector<SdpRecord> updatedList;
   getUpdatedSDPList(updatedList);
   ///cout<< "Packages to be copied ... \n";
   //cout << "Pacakages.size" << Packages.size();
   bool found;
   for( map<string, SdpInfo>::iterator ii=Packages.begin(); ii!=Packages.end(); ++ii)
   {
        string sdpName = (*ii).first;
        //cout << "4m Map: " << sdpName << endl;
        found = false;
        if (!updatedList.empty())
        {
           for( ACE_UINT32 i = 0; i < updatedList.size(); i++ )
           {
        	  //cout << "!!!!--comparing " << sdpName <<"with " << updatedList[i].sdpName << endl;
              if (sdpName.compare(updatedList[i].sdpName) == 0)//ignore case while comparing
              {
            	 //cout << "*****FOUND*****" << endl;
            	 found = true;
                 break;
              }
           }
        }
        if (found != true)
        {
          src_sdp_list.push_back(sdpName);
          //cout <<"--@@---Adding SDP : "<< sdpName << endl;
        }
   }
   return 0;
}
/* end get_SrcSdpList */

/*===================================================================
   ROUTINE: cpTo_Repository
=================================================================== */
ACE_INT32 APG_RepositoryMgr::cpTo_Repository(string srcPath, string destPath)
{
   if ( src_sdp_list.empty())
   {
      //cout<< "Package List is NULL\n";
      return 0;
    }
   if (srcPath.empty() || destPath.empty())
   {
      cout<< "source or destination path is NULL\n";
      return -1;
   }
   for( ACE_UINT32 i = 0; i < src_sdp_list.size(); i++)
   {
     string srcfile,destfile;

     srcfile.append(srcPath);
     //srcfile.append("/");
     srcfile.append(src_sdp_list[i]);
     //cout << "src File with full path " << srcfile;

     destfile.append(destPath);
     //destfile.append("/");
     destfile.append(src_sdp_list[i]);
     copyFile(srcfile, destfile);
     //cout << " dest File with full path " << destfile;

     //cout<< endl <<"Copy Status " << copyFile(srcfile, destfile)<<endl;//error check need to be done

    }
  return 0;
}
/* end cpTo_Repository */

/*===================================================================
   ROUTINE: cpTo_APGRepository
=================================================================== */
ACE_INT32 APG_RepositoryMgr::cpTo_APGRepository(string src_path, string dest_path)
{
	/*ACE_UINT32 numSDPs = src_sdp_list.size();
	struct statvfs64 buf;
	if (statvfs64(fcsc_Config::FCSC_ROOT_DIR.c_str(), &buf) != 0)
	{
		cout << "stat err" << endl;
		return -1;
	}
	//check free space before copy
	if ( (numSDPs * 30 * 1024 * 1024) <= (buf.f_bsize * buf.f_bfree) )
	{
		cout << "No size" << endl;
		return -1;
	}*/

    return cpTo_Repository(src_path, dest_path);
}
/* end cpTo_APGRepository */

/*===================================================================
   ROUTINE: copyFile
=================================================================== */

bool APG_RepositoryMgr::copyFile(string src_file, string dest_file)
{
    //cout << "Ntering copyFile" << endl;
	if (src_file.c_str() == NULL || dest_file.c_str() == NULL)
    {
        cout << "src dest wrong"<< endl;
    	return false;
    }


    FILE* src = ACE_OS::fopen(ACE_TEXT(src_file.c_str()),ACE_TEXT("r"));
    if (src == NULL)
    {
    	perror("src open ");
	return false;
    }


    /* open destination file */
    FILE* dest = ACE_OS::fopen(ACE_TEXT(dest_file.c_str()),ACE_TEXT("w"));
    if (dest == NULL)
    {
       	perror("dest open ");
       	fclose(src);
       	return false;
    }
    /* copy file using sendfile
    cout << stat_buf.st_size << endl;
    cout << "sendfile : " << sendfile (dest, src, &offset, 1024) << endl;
    perror("send file fail : ");*/



    ACE_TCHAR buf[1024];
    clearerr(src);
    clearerr(dest);
    ACE_INT32 filesize = 0;
    while( (filesize = ACE_OS::fread(buf, 1, 1024, src )) > 0 && !ferror(src) )
    {
    	ACE_OS::fwrite((void *)buf, 1, filesize, dest);
    	memset((void *)buf, 0, 1024);

       /* if ((res = ACE_OS::fread( buf, 1, 1024, src )) == 0 && ferror(src))
        {
    	    cout << "fread failed" << endl;
            return false;
        }
        if ( ACE_OS::fwrite(buf, 1, 1024, dest) > 0 )
        {
    	    cout << "fwrite failed" << endl;
    	    return false;
        }*/
    }

    //ACE_INT32 val  = EOF;
    //fwrite(&val, 1 , 4 ,dest);

    /*if ( ACE_OS::fclose(dest) != 0 && ACE_OS::fclose(src) != 0)
    {
        ACE_OS::fclose(src);
    	//cout << "file close failed" << endl;
    	return false;
    }*/
    int retd = ACE_OS::fclose(dest);
    int rets = ACE_OS::fclose(src);
    if(rets != 0 && retd != 0)
        return false;
    return true; // file copied successfully
}
/* end copyFile */

/*===================================================================
   ROUTINE: checkFreeSpace
=================================================================== */
bool APG_RepositoryMgr::checkFreeSpace(vector<SdpRecord> &updatedSdps)
{
	//total space needed = size of updated SDPs + size of updated + current SDPs(size of current_CSP)
    ACE_UINT32 iter;
    unsigned long long totalSdpSize = 0, totalFreeSpace = 0;
    for(iter = 0; iter < updatedSdps.size(); iter++)
    {
    	//cout << "Sdp name: " << updatedSdps[iter].sdpName << "Sdp size : " << updatedSdps[iter].size << endl;
    	totalSdpSize += updatedSdps[iter].size;
    }

    for( map<string, SdpInfo>::iterator ii=Packages.begin(); ii!=Packages.end(); ++ii)
    {
    	totalSdpSize += (*ii).second.size;
    }
    //cout << "sdp size = " << totalSdpSize << endl;
    totalSdpSize *= 1024;//converting kb to bytes
    struct statvfs64 buf;
    if (statvfs64(fcsc_Config::FCSC_ROOT_DIR.c_str(), &buf) != 0)
    {
    	cout << "stat err" << endl;
    	return false;
    }
    //cout << buf.f_bfree << "  " << buf.f_bsize <<endl;
    totalFreeSpace = buf.f_bfree *  buf.f_bsize;

    if (totalFreeSpace > totalSdpSize)
    	return true;
    return false;
}
/* end checkFreeSpace */

/*===================================================================
   ROUTINE: CoreMW_RepositoryMgr
=================================================================== */

CoreMW_RepositoryMgr::CoreMW_RepositoryMgr()
{
	string file_path = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_CURCSP_DIR + fcsc_Config::FCSC_PCF_FILE_NAME;
	fileName = new ACE_TCHAR[strlen(file_path.c_str()) + 1];
	strcpy(fileName, file_path.c_str());
	fileName[strlen(file_path.c_str())] = '\0';
	//cout <<"fileName : " << fileName << endl;
}
/* end CoreMW_RepositoryMgr */

/*===================================================================
   ROUTINE: ~CoreMW_RepositoryMgr(
=================================================================== */
CoreMW_RepositoryMgr::~CoreMW_RepositoryMgr()
{
     delete[] fileName;
     fileName = NULL;
}
/* end ~CoreMW_RepositoryMgr */

/*===================================================================
   ROUTINE: CoreMW_Cleanup
=================================================================== */
ACE_INT32 CoreMW_RepositoryMgr::CoreMW_Cleanup()
{
   ACE_INT32 ret;

   ret = parsePCF(fileName, true);
   if (ret == -1)
   {
       cout << "Error occured while fetching list of SDPs" << endl;
       return -1;
   }
   FILE *fp = ACE_OS::fopen (ACE_TEXT("CoreMW_Cleanup"),ACE_TEXT("w"));
   if (fp == NULL)
            return -1;
   //cout << "Map size : " << Packages.size() << endl;
   for( map<string, SdpInfo>::iterator ii=Packages.begin(); ii!=Packages.end(); ++ii)
   {
       string removeCmd = string("cmw-sdp-remove ERIC-") + (*ii).first;
       removeCmd.erase(removeCmd.length() - 4 , 4);
       removeCmd += ';';
       //cout << "removeCmd : " << removeCmd << endl;

        //check if takes \n automatically
       //cout << "Writing " << removeCmd.c_str() << endl;
       if (ACE_OS::fprintf(fp, "%s", removeCmd.c_str()) < 0)
       {
           return -1;
       }
       /* if (ACE_OS::fwrite(removeCmd.c_str(), ACE_OS::strlen(removeCmd.c_str()), 1, fp) == 0)
           return -1;*/
   }
   if (ACE_OS::fclose(fp) != 0)
       return -1;
   chmod("./CoreMW_Cleanup", S_IXUSR | S_IXGRP | S_IXOTH); 
   return 0;
}
/* end CoreMW_Cleanup */

/*===================================================================
   ROUTINE: CoreMW_Import
=================================================================== */

ACE_INT32 CoreMW_RepositoryMgr::CoreMW_Import()
{
   ACE_INT32 ret;
   if (fileName != NULL)
         delete[] fileName;

      string file_path = fcsc_Config::FCSC_DOWNLOAD_PATH + fcsc_Config::FCSC_PCF_FILE_NAME;
      fileName = new ACE_TCHAR[strlen(file_path.c_str()) + 1];
      strcpy(fileName, file_path.c_str());
      fileName[strlen(file_path.c_str())] = '\0';
      //cout <<"fileName : " << fileName << endl;

   ret = parsePCF(fileName, true);
   if (ret == -1)
   {
       cout << "Error occured while fetching list of SDPs" << endl;
       return -1;
   }
   FILE *fp = ACE_OS::fopen (ACE_TEXT("CoreMW_Import"),ACE_TEXT("w"));
   for( map<string, SdpInfo>::iterator ii=Packages.begin(); ii!=Packages.end(); ++ii)
   {
	   string all_sdp_path = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_UPDCSP_DIR;
       string importCmd = string("cmw-sdp-import ") + all_sdp_path + (*ii).first + string(";\n");
       //importCmd += ';';
       //cout << importCmd << endl;


       //ofstream fp("ChksumFile", ios::app);

        if (fp == NULL)
          return -1;

        //cout << "Writing " << importCmd.c_str() << endl;
        if (ACE_OS::fprintf(fp, "%s", importCmd.c_str()) < 0)
        {
            return -1;
        }
        //check if takes \n automatically
        /*if ( ACE_OS::fwrite(removeCmd.c_str(), ACE_OS::strlen(removeCmd.c_str()), 1, fp)== 0 )
           return -1;*/
   }
   if (ACE_OS::fclose(fp) != 0)
       return -1;
   chmod("./CoreMW_Import", S_IXUSR | S_IXGRP | S_IXOTH);
   return 0;
}
/* end CoreMW_Import */

/*===================================================================
   ROUTINE: CoreMW_Import
=================================================================== */
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

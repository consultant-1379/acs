
#include <ace/ACE.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_SSU_Common.h"
#include <ace/OS_NS_strings.h>

Cute_SSU_Common::Cute_SSU_Common()
{
}

bool Cute_SSU_Common::ExecuteCommand(string cmd)
{
        FILE *fp1,*fp2;
        char * data = new char[512];
        int status = -1;
        int ret = -1;
        bool bstatus = false;

        fp1 = popen(cmd.c_str(),"r");

        if (fp1 == NULL)
        {
                bstatus = false;
        }

        status = pclose(fp1);

        if (WIFEXITED(status))
        {
                ret=WEXITSTATUS(status);
        }

        if (ret == 0)
        {
                bstatus = true;
        }
        else
        {
                bstatus = false;
        }
        return bstatus;
}

void Cute_SSU_Common::CheckFileExistsBasicTest()
{
        //ExecuteCommand("touch /cluster/Umesh/testFileExists.txt");
        std::string myPath("/cluster/Umesh/testFileExists.txt");
        bool myResult = ACS_SSU_Common::CheckFileExists(myPath.c_str());
	std::cout<<"Result = "<<myResult<<std::endl;
        ASSERTM("##CheckFileExistsBasicTest Test case Failed##",myResult);
}



cute::suite Cute_SSU_Common::make_suite_Cute_SSU_Common()
{

	cute::suite s;
	s.push_back(CUTE(CheckFileExistsBasicTest));
	return s;
}



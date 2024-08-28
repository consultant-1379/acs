#include "CUTE_ACS_JTP_Service_R3A.h"
#include "CUTE_ACS_JTP_Service_R2A.h"
#include "CUTE_ACS_JTP_Job_R2A.h"
#include "CUTE_ACS_JTP_Job_R3A.h"
#include "CUTE_ACS_JTP_Conversation_R2A.h"
#include "CUTE_ACS_JTP_Conversation_R3A.h"

void runSuiteServerJobR2A()
{
	cute::ide_listener lis;

	//Create a ACS_JTP_Service_R2A object.
	CUTE_ACS_JTP_Service_R2A::init();

	cute::makeRunner(lis)(CUTE_ACS_JTP_Service_R2A::makeACSJTPServiceR2AJobSuite(), "Running Suite for ACS_JTP_Service_R2A\n\n");

	CUTE_ACS_JTP_Service_R2A::destroy();

}

void runSuiteServerConvR2A()
{
	cute::ide_listener lis;

	//Create a ACS_JTP_Service_R2A object.
	CUTE_ACS_JTP_Service_R2A::init();

	cute::makeRunner(lis)(CUTE_ACS_JTP_Service_R2A::makeACSJTPServiceR2AConvSuite(), "Running Suite for ACS_JTP_Service_R2A\n\n");

	CUTE_ACS_JTP_Service_R2A::destroy();

}

void runSuiteServerJobR3A()
{
	cute::ide_listener lis;

	//Create a ACS_JTP_Service_R3A object.
	CUTE_ACS_JTP_Service_R3A::init();

	cute::makeRunner(lis)(CUTE_ACS_JTP_Service_R3A::makeACSJTPServiceR3AJobSuite(), "Running Suite for ACS_JTP_Service_R3A\n\n");

	CUTE_ACS_JTP_Service_R3A::destroy();
}

void runSuiteServerConvR3A()
{
	cute::ide_listener lis;

	//Create a ACS_JTP_Service_R3A object.
	CUTE_ACS_JTP_Service_R3A::init();

	cute::makeRunner(lis)(CUTE_ACS_JTP_Service_R3A::makeACSJTPServiceR3AConvSuite(), "Running Suite for ACS_JTP_Service_R3A\n\n");

	CUTE_ACS_JTP_Service_R3A::destroy();
}

void runSuiteJobR2A()
{

	cute::ide_listener lis;
	//Create the job object.
	CUTE_ACS_JTP_Job_R2A::init();

	cute::makeRunner(lis)(CUTE_ACS_JTP_Job_R2A::makeACSJTPJobR2ASuite(), "Running suite for ACS_JTP_Job_R2A\n\n");

	//Destroy the job object.
	CUTE_ACS_JTP_Job_R2A::destroy();
}

void runSuiteJobR3A()
{
	cute::ide_listener lis;
	//Create the job object.
	CUTE_ACS_JTP_Job_R3A::init();

	cute::makeRunner(lis)(CUTE_ACS_JTP_Job_R3A::makeACSJTPJobR3ASuite(), "Running suite for ACS_JTP_Job_R3A\n\n");

	//Destroy the job object.
	CUTE_ACS_JTP_Job_R3A::destroy();

}
void runSuiteConvR2A()
{
	cute::ide_listener lis;
	//Create the conversation object.
	CUTE_ACS_JTP_Conversation_R2A::init();

	cute::makeRunner(lis)(CUTE_ACS_JTP_Conversation_R2A::makeACSJTPConversationR2ASuite(), "Running suite for ACS_JTP_Conversation_R2A\n\n");

	//Destroy the conversation object.
	CUTE_ACS_JTP_Conversation_R2A::destroy();

}
void runSuiteConvR3A()
{
	cute::ide_listener lis;
	//Create the conversation object.
	CUTE_ACS_JTP_Conversation_R3A::init();

	cute::makeRunner(lis)(CUTE_ACS_JTP_Conversation_R3A::makeACSJTPConversationR3ASuite(), "Running suite for ACS_JTP_Conversation_R3A\n\n");

	//Destroy the conversation object.
	CUTE_ACS_JTP_Conversation_R3A::destroy();

}

int main(int argc, char* argv[])
{
    if( argc == 3)
    {
    	if( ACE_OS::strcmp( argv[2], "R2A" ) == 0 )
    	{
    		if( ACE_OS::strcmp("sj", argv[1]) == 0 )
    		{
    			runSuiteServerJobR2A();
    		}
    		else if(ACE_OS::strcmp("sc", argv[1]) == 0  )
    		{
    			runSuiteServerConvR2A();
    		}
    		else if( argv[1][0] == 'j' )
    		{
    			runSuiteJobR2A();
    		}
    		else if( argv[1][0] == 'c' )
    		{
				runSuiteConvR2A();
    		}
    	}
    	else if( ACE_OS::strcmp( argv[2], "R3A" ) == 0 )
    	{
    		if( ACE_OS::strcmp("sj", argv[1]) == 0 )
    		{
    			runSuiteServerJobR3A();
    		}
    		else if(ACE_OS::strcmp("sc", argv[1]) == 0  )
    		{
    			runSuiteServerConvR3A();
    		}
    		else if( argv[1][0] == 'j' )
    		{
    			runSuiteJobR3A();
    		}
    		else if( argv[1][0] == 'c' )
    		{
				runSuiteConvR3A();
    		}
    	}
    }
}




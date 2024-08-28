
#include <iostream>
#include<ThreadPool.h>
#include "ace/Log_Msg.h"
#include "ace/ACE.h"
#include "ace/OS_NS_sys_select.h"
#include <unistd.h>
#include"ace/OS_NS_time.h"
//#include "ACS_CHB_Thread_JTPcomm.h"
using namespace std;

int globalVar=0;

class ACS_CHB_TestClass:public ThreadProcedure
{

public:
	int hbtimeout;
	int hbinterval;
	int MasterCPId;
	int myConversation;

	ACS_CHB_TestClass():ThreadProcedure ()
	{

	}
	/*
	inline ACS_CHB_TestClass(int hbtimeout,int hbinterval,int MasterCPId,int myConversation)
	{
		hbtimeout=100;
		hbinterval=5;
		MasterCPId=21;
		myConversation =0;
	}
*/
	inline int ACS_CHB_TestClassFunc1()
	{
		for(int i =0;i<10;++i)
		{
			cout<<"This is ACS_CHB_TestClassFunc1"<<endl;
		}
		return 0;
	}

	inline void ACS_CHB_TestClassFunc2()
	{
		for(int i =0;i<10;++i)
		{
			cout<<"This is ACS_CHB_TestClassFunc2"<<endl;
		}
	}

	inline void ACS_CHB_TestClassFunc3()
	{
		for(int i =0;i<10;++i)
		{
			cout<<"This is ACS_CHB_TestClassFunc3"<<endl;
		}
	}
  int operator()(void * g)
  {
	  ACE_DEBUG ((LM_TRACE, ACE_TEXT ("\n.................................This is start of thread function.......................\n")));
	  //cout<<".................................This is start of thread function......................."<<endl;
	  globalVar = *((int*)g);
	  for( globalVar =0; globalVar<10; ++globalVar)
	  {
		  cout<<"This is ACS_CHB_TestClassFunc1:\t"<<globalVar<<endl;
	  }
	  cout<<".................................This is end of thread function......................."<<endl;
	  return 0;

  }

  void stop() const
  {
	  cout <<"\n stopping."<<endl;
  }

private:

};

int main()
{
	ThreadPool pool(1, 70);


	for(int i=0;i<4;++i)
	{
		cout<<endl<<endl<<".................................QUEUING USER WORK ITEM............................."<<endl<<endl<<endl;
		pool.queueUserWorkItem(new ACS_CHB_TestClass(), &globalVar);
		cout<<endl<<endl<<".................................END OF USER WORK ITEM............................."<<endl<<endl<<endl;

	}

	ACE_DEBUG ((LM_TRACE, ACE_TEXT ("\n.....End of Test driver......\n")));
	sleep(50);

	return 0;

}

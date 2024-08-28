
#include<Cute_acs_logm_time.h>

cute_acs_logm_time::cute_acs_logm_time()
{


}

cute_acs_logm_time::~cute_acs_logm_time()
{

}

void cute_acs_logm_time::isTimeElapsed_unittest()
{
	acs_logm_time myCurrentTime1;
	sleep(3);
	bool myResult = myCurrentTime1.isTimeElapsed();
	ASSERTM("isTimeElapsed() Failed",(myResult == true));
	myCurrentTime1.addHours(1);
	myResult = myCurrentTime1.isTimeInFuture();
	ASSERTM("isTimeInFuture() Failed",(myResult == true));

}

void cute_acs_logm_time::isTimeInFuture_unittest()
{
	bool myResult= false;
	acs_logm_time myCurrentTime1;
	myCurrentTime1.addHours(1);
	myResult = myCurrentTime1.isTimeInFuture();
	ASSERTM("isTimeInFuture() Failed",(myResult == true));

	acs_logm_time myCurrentTime2;
	myCurrentTime2.addMinutes(3);
	myResult = myCurrentTime2.isTimeInFuture();
	ASSERTM("isTimeInFuture() Failed",(myResult == true));

	acs_logm_time myCurrentTime3;
	myCurrentTime3.addSeconds(10);
	myResult = myCurrentTime3.isTimeInFuture();
	ASSERTM("isTimeInFuture() Failed",(myResult == true));

}

void cute_acs_logm_time::elapsedTimeWithCurrentTime_unittest()
{
	double elapTime = 0;
	acs_logm_time myCurrentTime1;
	myCurrentTime1.addHours(1);

	elapTime = myCurrentTime1.elapsedTimeWithCurrentTime();
	elapTime *= -1;
	cout<<endl<<"elap time = "<<elapTime;
	ASSERTM("elapsedTimeWithCurrentTime() Failed",(elapTime >= 3600));

	acs_logm_time myCurrentTime2;
	myCurrentTime2.addMinutes(1);

	elapTime = myCurrentTime2.elapsedTimeWithCurrentTime();
	elapTime *= -1;
	cout<<endl<<"elap time = "<<elapTime;
	ASSERTM("elapsedTimeWithCurrentTime() Failed",(elapTime >= 60));

	acs_logm_time myCurrentTime3;
	myCurrentTime3.addDays(1);

	elapTime = myCurrentTime3.elapsedTimeWithCurrentTime();
	elapTime *= -1;
	cout<<endl<<"elap time = "<<elapTime;
	ASSERTM("elapsedTimeWithCurrentTime() Failed",(elapTime >= 86400));

	acs_logm_time myCurrentTime4;
	myCurrentTime4.addSeconds(5);

	elapTime = myCurrentTime4.elapsedTimeWithCurrentTime();
	elapTime *= -1;
	cout<<endl<<"elap time = "<<elapTime;
	ASSERTM("elapsedTimeWithCurrentTime() Failed",(elapTime >= 5));

}

void cute_acs_logm_time::elapsedTime_unittest()
{
	double elapTime = 0;
	acs_logm_time myCurrentTime0;

	acs_logm_time myCurrentTime1;
	myCurrentTime1.addHours(1);

	elapTime = myCurrentTime1.elapsedTime(myCurrentTime0);
	elapTime *= -1;
	cout<<endl<<"elap time = "<<elapTime;
	ASSERTM("elapsedTime() Failed",(elapTime >= 3600));

	acs_logm_time myCurrentTime2;
	myCurrentTime2.addMinutes(1);

	elapTime = myCurrentTime2.elapsedTime(myCurrentTime0);
	elapTime *= -1;
	cout<<endl<<"elap time = "<<elapTime;
	ASSERTM("elapsedTime() Failed",(elapTime >= 60));

	acs_logm_time myCurrentTime3;
	myCurrentTime3.addDays(1);

	elapTime = myCurrentTime3.elapsedTime(myCurrentTime0);
	elapTime *= -1;
	cout<<endl<<"elap time = "<<elapTime;
	ASSERTM("elapsedTime() Failed",(elapTime >= 86400));

	acs_logm_time myCurrentTime4;
	myCurrentTime4.addSeconds(5);

	elapTime = myCurrentTime4.elapsedTime(myCurrentTime0);
	elapTime *= -1;
	cout<<endl<<"elap time = "<<elapTime;
	ASSERTM("elapsedTime() Failed",(elapTime >= 5));

}

void cute_acs_logm_time::operator_less_than_unit_test()
{
	acs_logm_time myCurrentTime1;
	myCurrentTime1.addHours(1);
	acs_logm_time myCurrentTime2;
	bool myResult = false;
	if(myCurrentTime2 < myCurrentTime1)
	{
		myResult = true;
	}
	ASSERTM("operator_less_than_unit_test Failed",myResult);

}
void cute_acs_logm_time::operator_greater_than_unit_test()
{
	acs_logm_time myCurrentTime1;
	myCurrentTime1.addHours(1);
	acs_logm_time myCurrentTime2;
	bool myResult = false;
	if(myCurrentTime1 > myCurrentTime2)
	{
		myResult = true;
	}
	ASSERTM("operator_less_than_unit_test Failed",myResult);

}

void cute_acs_logm_time::difftime_unit_test()
{
	acs_logm_time myCurrentTime1;
	myCurrentTime1.addHours(1);
	acs_logm_time myCurrentTime2;
	double elaptime = myCurrentTime1.diffTime(myCurrentTime2);
	cout<<endl<<"elap time "<<elaptime;
	ASSERTM("difftime_unit_test Failed",(elaptime >= 3600));

}

cute::suite cute_acs_logm_time::make_suite()
{
	cute::suite s;

	s.push_back(CUTE(cute_acs_logm_time::isTimeElapsed_unittest));
	s.push_back(CUTE(cute_acs_logm_time::isTimeInFuture_unittest));
	s.push_back(CUTE(cute_acs_logm_time::elapsedTimeWithCurrentTime_unittest));
	s.push_back(CUTE(cute_acs_logm_time::elapsedTime_unittest));
	s.push_back(CUTE(cute_acs_logm_time::operator_less_than_unit_test));
	s.push_back(CUTE(cute_acs_logm_time::operator_greater_than_unit_test));
	s.push_back(CUTE(cute_acs_logm_time::difftime_unit_test));

	return s;
}


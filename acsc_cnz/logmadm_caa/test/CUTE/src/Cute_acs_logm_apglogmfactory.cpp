
#include<Cute_acs_logm_apglogmfactory.h>

cute_acs_logm_apglogmfactory::cute_acs_logm_apglogmfactory()
{
}

cute_acs_logm_apglogmfactory::~cute_acs_logm_apglogmfactory()
{
}

void cute_acs_logm_apglogmfactory::createComponents_unittest()
{

	acs_logm_apglogmFactory myFactory;
	std::list<acs_logm_housekeeping*> aHouseKeepingComponentsList;

	myFactory.createComponents(aHouseKeepingComponentsList);

	int mySize = 0;
	mySize = aHouseKeepingComponentsList.size();
	ASSERTM("createComponents_unittest createComponents during first time checking Failed",(mySize > 0));

	std::string myCommand_del = "";
	myCommand_del =  "immcfg -d timeBasedHouseKeepingId=TRALOG,logMaintenanceMId=1";
	ExecuteCommand(myCommand_del);
	myCommand_del =  "immcfg -d timeBasedHouseKeepingId=OUTPUTSTSIn,logMaintenanceMId=1";
	ExecuteCommand(myCommand_del);
	myCommand_del =  "immcfg -d timeBasedHouseKeepingId=OUTPUTSTSag,logMaintenanceMId=1";
	ExecuteCommand(myCommand_del);
	myCommand_del =  "immcfg -d periodicBasedHouseKeepingId=SCXLOG,logMaintenanceMId=1";
	ExecuteCommand(myCommand_del);

	aHouseKeepingComponentsList.clear();
	mySize = aHouseKeepingComponentsList.size();
	ASSERTM("createComponents_unittest createComponents after deleting all objects checking Failed",(mySize == 0));

	ExecuteCommand("immcfg -f /opt/ap/acs/conf/APZIM_LogMaintenanceM_imm_objects.xml");

	aHouseKeepingComponentsList.clear();
	myFactory.createComponents(aHouseKeepingComponentsList);
	mySize = aHouseKeepingComponentsList.size();
	ASSERTM("createComponents_unittest createComponents  after reloading the XML file checking Failed",(mySize > 0));


}
bool cute_acs_logm_apglogmfactory::ExecuteCommand(string cmd)
{
	FILE *fp1;
		//char * data = new char[512];
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

cute::suite cute_acs_logm_apglogmfactory::make_suite()
{
	cute::suite s;

	s.push_back(CUTE(cute_acs_logm_apglogmfactory::createComponents_unittest));


	return s;

}






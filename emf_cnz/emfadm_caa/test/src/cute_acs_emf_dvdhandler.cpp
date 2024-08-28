#include "cute_acs_emf_dvdhandler.h"



ACS_EMF_MEDIAHandler* TestSuite::acs_emf_dvdhandler = NULL;

void TestSuite::init()
{
	ACE_Event * p_hHandles = new ACE_Event(TRUE,FALSE,USYNC_THREAD,"EV_ACS_EMF_TERMINATE_EVENT");
	//acs_emf_dvdhandler = new ACS_EMF_DVDHandler(p_hHandles, NULL);
	acs_emf_dvdhandler = new ACS_EMF_DVDHandler();
}



void TestSuite::destroy()
{
	delete acs_emf_dvdhandler;
}

void TestSuite::DVDOperationCheckForFromDVD()
{

	ACS_EMF_DATA srctData;
	ACS_EMF_ParamList Params;

	// FROM DVD
	Params.CmdCode=EMF_CMD_TYPE_COPYFROMMEDIA;
	cout << "Recieved Code EMF_CMD_TYPE_COPYFROMDVD" << Params.CmdCode << endl;
	const ACE_TCHAR* lpszArgs = "/cluster/EMF/Test";
	Params.Data[0]=lpszArgs;
	srctData.Code = Params.CmdCode;
	Params.Encode();
	(void)Params.getEncodedBuffer(srctData.Data);
	//srctData.Data[0] = *lpszArgs;
	if(acs_emf_dvdhandler->mediaOperation(&srctData)== 0)
		cout<<"DVDOperation success"<<endl;
	else
		cout<<"DVDOperation unsuccess"<<endl;

}

void TestSuite::DVDOperationCheckForFromDVDToCpAllData()
{

	ACS_EMF_DATA srctData;
	ACS_EMF_ParamList Params;

	// FROM DVD
	Params.CmdCode=EMF_CMD_TYPE_COPYFROMMEDIA;
	cout << "Recieved Code EMF_CMD_TYPE_COPYFROMDVD" << Params.CmdCode << endl;
	const ACE_TCHAR* lpszArgs = "/cluster/EMF/Test";
	Params.Data[0]=lpszArgs;
	srctData.Code = Params.CmdCode;
	Params.Data[0] = "SC-2-1";
	Params.Data[1] = "yes";
	Params.Data[2]=lpszArgs;
	Params.Encode();
	(void)Params.getEncodedBuffer(srctData.Data);
	//srctData.Data[0] = *lpszArgs;
	if(acs_emf_dvdhandler->mediaOperation(&srctData)== 0)
		cout<<"DVDOperation success"<<endl;
	else
		cout<<"DVDOperation unsuccess"<<endl;

}

void TestSuite::DVDOperationCheckForFromDVDToCpSpecData()
{

	ACS_EMF_DATA srctData;
	ACS_EMF_ParamList Params;

	// FROM DVD
	Params.CmdCode=EMF_CMD_TYPE_COPYFROMMEDIA;
	cout << "Recieved Code EMF_CMD_TYPE_COPYFROMDVD" << Params.CmdCode << endl;
	const ACE_TCHAR* lpszArgs1 = "emfinfo";
	const ACE_TCHAR* lpszArgs2 = "/cluster/EMF/Test";
	srctData.Code = Params.CmdCode;
	Params.Data[0] = "SC-2-1";
	Params.Data[1] = "yes";
	Params.Data[2]=lpszArgs1;
	Params.Data[3]=lpszArgs2;
	Params.Encode();
	(void)Params.getEncodedBuffer(srctData.Data);
	//srctData.Data[0] = *lpszArgs;
	if(acs_emf_dvdhandler->mediaOperation(&srctData)== 0)
		cout<<"DVDOperation success"<<endl;
	else
		cout<<"DVDOperation unsuccess"<<endl;

}

void TestSuite::DVDOperationCheckForToDVD()
{
	ACS_EMF_DATA srctData;
	ACS_EMF_ParamList Params;
	// TODVD
	Params.CmdCode=EMF_CMD_TYPE_COPYTOMEDIA;
	Params.Data[0] = "SC-2-1";
	Params.Data[1] = "";
	Params.Data[2] = "yes";
	Params.Data[3] = "yes";
	Params.Data[4] = "/root/emfinfo";

	Params.Encode();
	(void)Params.getEncodedBuffer(srctData.Data);

	if(acs_emf_dvdhandler->mediaOperation(&srctData)== 0)
		cout<<"DVDOperation success"<<endl;
	else
		cout<<"DVDOperation unsuccess"<<endl;

}

void TestSuite::DVDOperationCheckForMediaInfo()
{
	ACS_EMF_DATA srctData;
	ACS_EMF_ParamList Params;
	// TODVD
	Params.CmdCode = EMF_CMD_TYPE_GETMEDIAINFO;
	Params.Encode();
	(void)Params.getEncodedBuffer(srctData.Data);

	if(acs_emf_dvdhandler->mediaOperation(&srctData)== 0)
		cout<<"DVDOperation success"<<endl;
	else
		cout<<"DVDOperation unsuccess"<<endl;

}

void TestSuite::DVDOperationCheckForOwnerInfo()
{
	ACS_EMF_DATA srctData;
	ACS_EMF_ParamList Params;
	// TODVD
	Params.CmdCode = EMF_CMD_TYPE_GETDVDOWNER;
	Params.Encode();
	(void)Params.getEncodedBuffer(srctData.Data);

	if(acs_emf_dvdhandler->mediaOperation(&srctData)== 0)
		cout<<"DVDOperation success"<<endl;
	else
		cout<<"DVDOperation unsuccess"<<endl;

}

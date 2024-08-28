/*=================================================================== */
/**
  @file swr_Swrprint.cpp

  Implementation for swrprint 

  @version 1.0.0

  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module

  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       02/05/2012   XSAMECH   Initial Release
  N/A       29/05/2012   XSAMECH   TR HP94040 fixed,APOS details updated
  N/A       16/07/2012   XSAMECH   TR HQ15308 fixed.
  N/A       31/07/2012   XSAMECH   CBA components display added.
  N/A	    17/10/2012   XSAMECH   TR HQ48327 fixed.
  N/A		20/05/2014	 XSAMECH    NON APG software handling.
  N/A	    04/07/2014	 XSAMECH   TR  HS 75015 FIXED.
 */

/*===================================================================
  INCLUDE DECLARATION SECTION
  =================================================================== */
#include "acs_swr_global.h"
#include <boost/filesystem.hpp>
/*
 * Global Variables
 */
string g_errorText;
OmHandler g_theOmHandler;


/*
 * Main Function
 */
const char* const SWMVERSION="/cluster/storage/system/config/apos/swm_version";
bool is_swm_2_0 = true;
bool isSWM20();
int main(int argc, char *argv[])
{

	vector<string> dnListSwInventory;
	string SwInventoryId,systemName;
	string activeVersion;
	string aposPackage,aposSecurity;
	map<string,string>AmfSC1bundle,AmfSC2bundle;
	map<string,string> cxcBlockNameMap;
	swData data;
	parseDetails parData;
	parData.l_opt=false;
	parData.c_opt=false;

	list<string> activeVersions;
	is_swm_2_0 = isSWM20();
	if (!parse(argc,argv,parData))
	{
		cout<<g_errorText<<endl;
		return 2;
	}

	initBlockCxcName(cxcBlockNameMap);

	int initStatus=0;
	initStatus=getInitialParams(dnListSwInventory, activeVersion, systemName, aposPackage,
			aposSecurity,activeVersions);
	if (initStatus)
	{
		cout<<g_errorText<<endl;
		return initStatus;
	}

	if(ACS_CC_SUCCESS != getInstalledBundle(AmfSC1bundle,AmfSC2bundle))
	{
		cout<<g_errorText<<endl;
		return 11;
	}
	SwInventoryId=dnListSwInventory[0];
	if(argc == 1) 
	{
		string var;
		bool category=false;
		int dataStat=0;
		if ((dataStat=printData(var, category, activeVersions, AmfSC1bundle, AmfSC2bundle, systemName, aposPackage,
				aposSecurity, SwInventoryId, data, activeVersion)))
		{
			cout<<g_errorText<<endl;
			return dataStat;
		}
		cout << endl;

		printThirdPart(data);

		return 0;
	}

	if (argc>=2)
	{
		//Now check for printing
		if(strcmp(argv[1], "-l") == 0 && argc == 2)
		{
			string var;

			bool category = false;
			int printPartStatus=0;

			list<string>subSys;
			if ((printPartStatus=printPart(var, category, activeVersions, AmfSC1bundle, AmfSC2bundle, systemName,
					aposPackage, aposSecurity, subSys, SwInventoryId, cxcBlockNameMap, data, activeVersion)))
			{
				cout<<g_errorText<<endl;
				return printPartStatus;
			}

			printThirdPart(data);
			return 0;


		}
		else if(parData.l_opt && parData.c_opt && argc == 4)
		{
			// List all active packages with -l and -c options-->fewer clones implementation.
			string var;
			var = argv[parData.parPosition[c_category]+1];
			if (strcmp(argv[parData.parPosition[c_category]+1], "3pp") == 0)
			{
				int productStat=0;
				list<string>subSys;
				productStat=getProductData(AmfSC1bundle, AmfSC2bundle, subSys, activeVersion, SwInventoryId, data);
				if (productStat)
					return productStat;
				if(!header(systemName, aposPackage, aposSecurity))
				{
					cout<<swrErrorStrings::nodeNameNotFound<<endl;
					return 1;
				}
				getProductStats(activeVersion);
				printThirdPart(data);
				return 0;
			}

			bool category =true;
			int printPartStatus=0;

			list<string>subSys;
			if ((printPartStatus=printPart(var, category, activeVersions, AmfSC1bundle, AmfSC2bundle,
					systemName, aposPackage, aposSecurity, subSys, SwInventoryId, cxcBlockNameMap, data, activeVersion)))
			{
				cout<<g_errorText<<endl;
				return printPartStatus;
			}	
			return 0;	
		}
		else if(parData.c_opt  && argc == 3)
		{

			// List all, or third party, active packages with -c option-->fewer clones implementation.
			string var;
			var = argv[parData.parPosition[c_category]+1];
			if (strcmp(argv[parData.parPosition[c_category]+1], "3pp") == 0)

			{
				int productStat=0;
				list<string>subSys;
				productStat=getProductData(AmfSC1bundle, AmfSC2bundle, subSys, activeVersion, SwInventoryId, data);
				if (productStat)
					return productStat;
				if(!header(systemName, aposPackage, aposSecurity))
				{
					cout<<swrErrorStrings::nodeNameNotFound<<endl;
					return 1;
				}
				getProductStats(activeVersion);
				printThirdPart(data);
				return 0;
			}

			bool category = true;
			int dataStat=0;
			if ((dataStat=printData(var, category, activeVersions, AmfSC1bundle, AmfSC2bundle, systemName, aposPackage,
					aposSecurity, SwInventoryId, data, activeVersion)))
			{
				cout<<g_errorText<<endl;
				return dataStat;
			}
			return 0;
		}
		// printing chosen packages 
		else if (parData.l_opt && argc > 2)
		{
			// List chosen active packages with -l (and possibly -c) option-->fewer clones implementation.
			unsigned int checkCat=0;
			bool category =false;
			string var;
			if (parData.c_opt)
			{
				if (strcmp(argv[parData.parPosition[c_category]+1], "3pp") == 0)
				{
					cout <<swrErrorStrings::illegalIn3pp<< endl;
					cout <<endl;
					exit(1);
				}
				var = argv[parData.parPosition[c_category]+1];
				category = true;

			}
			int productStat=0;
			bool head=true;
			int trackPackage=0;
			bool foundSubsys=false;
			list<string>subSys;
			for (list<string>::const_iterator activeIt=activeVersions.begin();activeIt!=activeVersions.end();++activeIt)
			{
				data.packageName.clear();
				data.nameToNumber.clear();
				data.nameToRevision.clear();
				data.packageToProduct.clear();
				subSys.clear();
				productStat=getProductData(AmfSC1bundle, AmfSC2bundle, subSys, (*activeIt), SwInventoryId, data);
				if (productStat)
					return productStat;

				data.packageName.sort();
				int packPos=1;


				bool catFound= false;
				map<string,string>::const_iterator name_Num;
				map<string,string>::const_iterator name_Rev;
				map<string,string>::const_iterator subsysNameIt;

				if( category && data.subsysMap.count(toUpperString(var)))
				{
					catFound=true;
				}

				if( catFound || !category)
				{
					string SubsystemA;
					for (;packPos<argc;++packPos)
					{

						if ( packPos == parData.parPosition[l_longform] ||
								packPos == parData.parPosition[c_category] ||
								(parData.c_opt && packPos ==(parData.parPosition[c_category]+1)))
						{
							continue;
						}
						string ProductName,ProductRevision,ProductNumber;
						map<string,string>::iterator packIter;
						string tempPackage(argv[packPos]);
						packIter = data.packageMap.find(toUpperString(tempPackage));
						if(data.packageMap.end() == packIter)
							continue;
						ProductName = (*packIter).second;

						if (data.nameToNumber.count(ProductName)> 0 )
						{
							name_Num=data.nameToNumber.find(ProductName);
							ProductNumber=(*name_Num).second;
							name_Rev=data.nameToRevision.find(ProductName);
							ProductRevision=(*name_Rev).second;
							subsysNameIt=data.subsysName.find(ProductName);
							SubsystemA=(*subsysNameIt).second;
							string tempSubsytem;
							map<string,string>::iterator subsysIter;
							subsysIter=data.subsysMap.find(toUpperString(var));
							if (data.subsysMap.end() != subsysIter)
							{
								if (SubsystemA == (*subsysIter).second)
									foundSubsys=true;
							}
							else
							{
								foundSubsys=false;
							}

							if (foundSubsys || !category )
							{
								if(head)
								{
									if(!header(systemName, aposPackage, aposSecurity))
									{
										cout<<swrErrorStrings::nodeNameNotFound<<endl;
										return 1;
									}
									getProductStats((activeVersion));
									if(category)
									{

										cout << "Category:   " << (*subsysIter).second<< endl << endl;
									}
									head=false;

								}
								cout<<"SUBSYSTEM:\t\t"    <<SubsystemA<<endl;
								cout<<"MODULE NAME:\t\t"   <<ProductName<<endl;
								cout<<"CXC NAME:\t\t";getCxcName(ProductName,cxcBlockNameMap);
								cout << "CXC NUMBER:\t\t"<<ProductNumber<<endl;
								cout << "CXC VERSION:\t\t" <<ProductRevision<<endl;
								cout << "SUPPLIER:\t\t" <<"ERICSSON AB"<<endl;
								cout<<endl;
								cout<<endl;
								trackPackage++;

							}
						}

					}
				}
				else
				{
					checkCat++;
				}
			}
			if (checkCat == activeVersions.size())
			{
				cout <<swrErrorStrings::catNotFound<< endl;
				return 9;
			}
			else if (trackPackage!=parData.packageCount)
			{
				cout <<endl<<swrErrorStrings::packageNotFound<<  endl;
				return 10;
			}
		}
		else 
		{
			// List data for chosen active packages, no (or possibly -c) options--> fewer clones implementation.
			string var;
			bool category =false;
			if (parData.c_opt)
			{
				if (strcmp(argv[parData.parPosition[c_category]+1], "3pp") == 0)
				{
					cout <<swrErrorStrings::illegalIn3pp<< endl;
					cout <<endl;
					return 1;
				}

				var = argv[parData.parPosition[c_category]+1];
				category = true;
			}

			int productStat=0;
			list<string>subSys;
			int trackPackage=0;
			bool head=true;
			bool foundSubsys=false;
			unsigned int checkCat=0;
			for (list<string>::const_iterator activeIt=activeVersions.begin();activeIt!=activeVersions.end();++activeIt)
			{
				data.packageName.clear();
				data.nameToNumber.clear();
				data.nameToRevision.clear();
				data.packageToProduct.clear();
				subSys.clear();
				productStat=getProductData(AmfSC1bundle, AmfSC2bundle, subSys, (*activeIt), SwInventoryId, data);
				if (productStat)
					return productStat;

				data.packageName.sort();
				int packPos=1;
				bool catFound= false;
				map<string,string>::const_iterator name_Num;
				map<string,string>::const_iterator name_Rev;
				map<string,string>::const_iterator subsysNameIt;

				if(category&& data.subsysMap.count(toUpperString(var)))
				{
					catFound=true;
				}

				if( catFound || !category)
				{
					string SubsystemA;
					for (;packPos<argc;++packPos)
					{

						if ( packPos == parData.parPosition[l_longform] ||
								packPos == parData.parPosition[c_category] ||
								(parData.c_opt && packPos ==(parData.parPosition[c_category]+1)))
						{
							continue;
						}

						string ProductName,ProductRevision,ProductNumber;
						map<string,string>::iterator packIter;
						string tempPackage(argv[packPos]);
						packIter = data.packageMap.find(toUpperString(tempPackage));
						if(data.packageMap.end() == packIter)
							continue;
						ProductName= (*packIter).second;

						if (data.nameToNumber.count(ProductName)> 0 )
						{
							name_Num=data.nameToNumber.find(ProductName);
							ProductNumber=(*name_Num).second;
							name_Rev=data.nameToRevision.find(ProductName);
							ProductRevision=(*name_Rev).second;
							subsysNameIt=data.subsysName.find(ProductName);
							SubsystemA=(*subsysNameIt).second;
							string tempSubsytem;
							map<string,string>::iterator subsysIter;
							subsysIter=data.subsysMap.find(toUpperString(var));
							if (data.subsysMap.end() != subsysIter)
							{
								if (SubsystemA == (*subsysIter).second)
									foundSubsys=true;
							}
							else
							{
								foundSubsys=false;
							}

							if (foundSubsys || !category )
							{
								if(head)
								{
									if(!header(systemName, aposPackage, aposSecurity))
									{
										cout<<swrErrorStrings::nodeNameNotFound<<endl;
										return 1;
									}
									getProductStats(activeVersion);

									if(category)
									{
										cout << "Category:   " << SubsystemA<< endl << endl;
									}
									head=false;

									cout << setw(25) << "PACKAGE" << setw(16) << "IDENTITY" << setw(10) << "VERSION"
											<< setw(16) << "CNI" << "STATE" << endl;
								}
								cout<< setw(25) << ProductName << setw(16)<< ProductNumber << setw(10)<<ProductRevision <<setw(16) << " " <<"Active"<<endl;
								trackPackage++;
							}

						}

					}
				}
				else
				{
					checkCat++;
				}
			}
			if (checkCat == activeVersions.size())
			{
				cout <<swrErrorStrings::catNotFound<< endl;
				return 9;
			}
			else if (trackPackage!= parData.packageCount)
			{
				cout <<endl<<swrErrorStrings::packageNotFound<<  endl;
				return 10 ;
			}

		}
	}

	return 0;
}

bool isSWM20()
{
    if((boost::filesystem::exists(boost::filesystem::path(SWMVERSION)))== true)
    {
        return true;
    }
    else {
        return false;
    }
}


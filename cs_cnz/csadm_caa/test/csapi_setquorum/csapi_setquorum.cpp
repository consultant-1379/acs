
#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_Util.h"

#include <vector>
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <set>
#include <string>
#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace std;
using namespace ACS_CS_API_SET_NS;

//
// Definitions
//

//------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------
void testSetString();
void printSetString(std::set<std::string> group);
void printQuorumData();
bool testSetQuorumData(int profile, int aqr, int begin, int end);

int main(int argc, char *argv[])
{

	if ( argc < 2 || argc > 6)
	{
		cout << endl;
		cout << "Usage: csapi_setquorum <list | testSet | set profile aqr startBcId endBcId> " << endl;
		cout << "csapi_setquorum <list|set> <profile> <aqr> <from id blade> <to id blade>" << endl;
		cout << endl;
	}
	else
	{
		cout << endl << "Option = "<< argv[1] << endl;
		if (strcmp(argv[1],"set") == 0)
		{
			if (!testSetQuorumData(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5])))
			{
				cout << endl << "FAILURE" << endl;
				return 1;
			}
		}
		else if (strcmp(argv[1],"list") == 0)
		{
			printQuorumData();
		}
		else if (strcmp(argv[1],"testSet") == 0)
		{
			testSetString();
		}
	}

    return 0;
}


bool testSetQuorumData(int profile, int aqr, int begin, int end)
{
    ACS_CS_API_QuorumData data;

    data.trafficIsolatedCpId = 0;
    data.trafficLeaderCpId = 0;
    data.automaticQuorumRecovery = aqr;
    data.apzProfile = profile;
    data.aptProfile = profile;

    for (int i=begin; i<=end; i++)
    {
    	ACS_CS_API_CpData cpData;
    	cpData.id = static_cast<CPID> (i);
    	cpData.state = static_cast<ACS_CS_API_NS::CpState> (3);
    	cpData.applicationId = 0;
    	cpData.apzSubstate = 255;
    	cpData.stateTransition = 4;
    	cpData.aptSubstate = 1;
    	cpData.blockingInfo = 6;
    	cpData.cpCapacity = 7;

    	data.cpData.push_back(cpData);
    }

    if (ACS_CS_API_Set::setQuorumData(data) != Result_Success) {
        cout << "setQuorumData() failed" << endl;
        return false;
    }

    return true;
}

void printQuorumData()
{
	ACS_CS_ImModel * model = new ACS_CS_ImModel();

	try
	{
		std::string QuorumDataFile = ACS_CS_NS::PATH_QUORUM_DATA_FILE;
		std::ifstream file(QuorumDataFile.c_str());
		boost::archive::binary_iarchive ia(file);
		ia >> *model;
	}
	catch (boost::archive::archive_exception& e)
	{
		cout << "Serialization exception occurred: "<< e.what()<< endl;
	}
	catch (boost::exception& )
	{
		cout << "Serialization exception occurred."<< endl;
	}

	if (model)
	{
		ACS_CS_ImModel *currentModel = ACS_CS_ImRepository::instance()->getModelCopy();

		ACS_CS_ImBase * currentCpCluster = currentModel->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());
		ACS_CS_ImBase * CpCluster = model->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());

		if (currentCpCluster && CpCluster)
		{
			ACS_CS_ImCpCluster *cluster = dynamic_cast<ACS_CS_ImCpCluster *> (CpCluster);
			ACS_CS_ImCpCluster *currentCluster = dynamic_cast<ACS_CS_ImCpCluster *> (currentCpCluster);

			cout << ""<< endl << endl;

			cout << "  IMM ALLBC: ";
			printSetString(currentCluster->allBcGroup);

			cout << "IMM OPGROUP: ";
			printSetString(currentCluster->operativeGroup);

			cout << ""<< endl << endl;

			cout << "  DATA ALLBC: ";
			printSetString(cluster->allBcGroup);

			cout << "DATA OPGROUP: ";
			printSetString(cluster->operativeGroup);

			cout << ""<< endl << endl;

			if (currentCluster->allBcGroup == cluster->allBcGroup)
			{
				cout << "  NO DIFFERENCE FOUND !!! "<< endl;
			}
			else
			{
				cout << "  DIFFERENCE FOUND !!! "<< endl;

				if (currentCluster->allBcGroup.size() > cluster->allBcGroup.size())
				{
					for (std::set<std::string>::iterator it_cur1 = currentCluster->allBcGroup.begin(); it_cur1 != currentCluster->allBcGroup.end(); it_cur1++)
					{
						std::set<std::string>::iterator it_f;
						it_f = cluster->allBcGroup.find((*it_cur1));
						if (it_f == cluster->allBcGroup.end())
						{
							cout << (*it_cur1) << " " << endl;
						}
					}
				}
				else
				{
					for (std::set<std::string>::iterator it_cur1 = cluster->allBcGroup.begin(); it_cur1 != cluster->allBcGroup.end(); it_cur1++)
					{
						std::set<std::string>::iterator it_f;
						it_f = currentCluster->allBcGroup.find((*it_cur1));
						if (it_f == currentCluster->allBcGroup.end())
						{
							cout << (*it_cur1) << " " << endl;
						}
					}
				}
			}

			delete currentModel;
		}
		delete model;
	}
}

void printSetString(std::set<std::string> group)
{
	for (std::set<std::string>::iterator it = group.begin(); it != group.end(); it++)
	{
		cout << (*it) << " ";
	}
	cout << ";" << endl;
}

void testSetString()
{
	int sizeSet1 = 0;
	int sizeSet2 = 0;
	std::set<std::string> set1;
	std::set<std::string> set2;

	cout << " Set-1- size: "<< endl;
	cin >> sizeSet1;

	for (int i1=0;i1<sizeSet1;i1++)
	{
		std::string value("");
		cout << " Set-1- elem "<< i1 << ":";
		cin >> value;
		set1.insert(value);
	}

	cout << " Set-2- size: "<< endl;
	cin >> sizeSet2;

	for (int i2=0;i2<sizeSet2;i2++)
	{
		std::string value("");
		cout << " Set-2- elem "<< i2 << ":";
		cin >> value;
		set2.insert(value);
	}

	cout << "> Set-1-"<< endl;
	printSetString(set1);
	cout << "> Set-2-"<< endl;
	printSetString(set2);

	if (set1 == set2)
	{
		cout << "  NO DIFFERENCE FOUND !!! "<< endl;
	}
	else
	{
		cout << "  DIFFERENCE FOUND !!! "<< endl;

		if (set1.size() > set2.size())
		{
			for (std::set<std::string>::iterator it_cur1 = set1.begin(); it_cur1 != set1.end(); it_cur1++)
			{
				std::set<std::string>::iterator it_f;
				it_f = set2.find((*it_cur1));
				if (it_f == set2.end())
				{
					cout << (*it_cur1) << " " << endl;
				}
			}
		}
		else
		{
			for (std::set<std::string>::iterator it_cur1 = set2.begin(); it_cur1 != set2.end(); it_cur1++)
			{
				std::set<std::string>::iterator it_f;
				it_f = set1.find((*it_cur1));
				if (it_f == set1.end())
				{
					cout << (*it_cur1) << " " << endl;
				}
			}
		}

	}
}

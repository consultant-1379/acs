#include "ACS_CS_API_NWT_BoardSearch_Implementation.h"
#include "ACS_CS_API_NWT_Implementation.h"



#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImComputeResource.h"
#include "ACS_CS_ImComputeResourceNetwork.h"
#include "ACS_CS_ImInternalNetwork.h"
#include "ACS_CS_ImExternalNetwork.h"

ACS_CS_API_NWT_Implementation:: ACS_CS_API_NWT_Implementation()
 {
	model = new ACS_CS_ImModel();
	immReader = new ACS_CS_ImIMMReader();
	int nwload = immReader->loadModel(model);

		if(nwload < 0){
			if(model)
				delete model;
			model = NULL;
		}
 }

 ACS_CS_API_NWT_Implementation::~ACS_CS_API_NWT_Implementation()
 {
	 if(immReader)
	 		delete immReader;
		if(model)
			delete model;
}

ACS_CS_API_NS::CS_API_Result  ACS_CS_API_NWT_Implementation::getIntNetwID (ACS_CS_API_NameList_R1 &listObj, BoardID boardId)
{
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;
	ACS_CS_API_NWT_BoardSearch_R1 *searchObj = ACS_CS_API_NWT_R1::createNetworkSearchInstance();
	searchObj->setBoardID(boardId);
	list<ACS_CS_API_NetworkTable_R1> rows;
	ACS_CS_API_NS::CS_API_Result resultVal = getNtwTableRows(rows,searchObj);
	if(resultVal == ACS_CS_API_NS::Result_Success)
	{
		listObj.setSize(rows.size());
		int i=0;
		for (std::list<ACS_CS_API_NetworkTable_R1>::iterator row=rows.begin(); row!=rows.end(); ++row)
		{
		    string intNtwId = row->intNetwId;
		    ACS_CS_API_Name intNtwIdName(intNtwId.c_str());
		    listObj.setValue(intNtwIdName,i++);
		}
		if(listObj.size()>0)
			resultValue = ACS_CS_API_NS::Result_Success;
		else if(listObj.size()==0)
			resultValue = ACS_CS_API_NS::Result_NoValue;
	}
	ACS_CS_API_NWT_R1::deleteNetworkSearchInstance(searchObj);
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result  ACS_CS_API_NWT_Implementation::getExtNetwID (ACS_CS_API_NameList_R1 &listObj, BoardID boardId)
{
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;
	ACS_CS_API_NWT_BoardSearch_R1 *searchObj = ACS_CS_API_NWT_R1::createNetworkSearchInstance();
	searchObj->setBoardID(boardId);
	list<ACS_CS_API_NetworkTable_R1> rows;
	ACS_CS_API_NS::CS_API_Result resultVal = getNtwTableRows(rows,searchObj);
	if(resultVal == ACS_CS_API_NS::Result_Success)
	{
		listObj.setSize(rows.size());
		int i=0;
		for (std::list<ACS_CS_API_NetworkTable_R1>::iterator row=rows.begin(); row!=rows.end(); ++row)
		{
		    string extNtwId = row->extNetwId;
		    ACS_CS_API_Name extNtwIdName(extNtwId.c_str());
		    listObj.setValue(extNtwIdName,i++);
		}
		if(listObj.size()>0)
			resultValue = ACS_CS_API_NS::Result_Success;
		else if(listObj.size()==0)
			resultValue = ACS_CS_API_NS::Result_NoValue;
	}
	ACS_CS_API_NWT_R1::deleteNetworkSearchInstance(searchObj);
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NWT_Implementation::getInterfaceID (ACS_CS_API_NameList_R1 &listInterfaces, BoardID boardId)
{
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;
	ACS_CS_API_NWT_BoardSearch_R1 * searchObj = ACS_CS_API_NWT_R1::createNetworkSearchInstance();
	searchObj->setBoardID(boardId);
	list<ACS_CS_API_NetworkTable_R1> rows;
	ACS_CS_API_NS::CS_API_Result resultVal = getNtwTableRows(rows,searchObj);
	if(resultVal == ACS_CS_API_NS::Result_Success)
	{
		listInterfaces.setSize(rows.size());
		int i=0;
		for (std::list<ACS_CS_API_NetworkTable_R1>::iterator row=rows.begin(); row!=rows.end(); ++row)
		{
		    string interfaceId = row->interfaceId;
		    ACS_CS_API_Name interfaceIdName(interfaceId.c_str());
			listInterfaces.setValue(interfaceIdName,i++);
		}
		if(listInterfaces.size()>0)
			resultValue = ACS_CS_API_NS::Result_Success;
		else if(listInterfaces.size()==0)
			resultValue = ACS_CS_API_NS::Result_NoValue;
	}
	ACS_CS_API_NWT_R1::deleteNetworkSearchInstance(searchObj);
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NWT_Implementation::getNtwName (ACS_CS_API_NameList_R1 &listNtwNames, BoardID boardId)
{
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;
	ACS_CS_API_NWT_BoardSearch_R1 *searchObj = ACS_CS_API_NWT_R1::createNetworkSearchInstance();
	searchObj->setBoardID(boardId);
	list<ACS_CS_API_NetworkTable_R1> rows;
	ACS_CS_API_NS::CS_API_Result resultVal = getNtwTableRows(rows,searchObj);
	if(resultVal == ACS_CS_API_NS::Result_Success)
	{
		listNtwNames.setSize(rows.size());
		int i=0;
		for (std::list<ACS_CS_API_NetworkTable_R1>::iterator row=rows.begin(); row!=rows.end(); ++row)
		{
		    string netwkNam = row->netwName;
		    ACS_CS_API_Name networkName(netwkNam.c_str());
		    listNtwNames.setValue(networkName,i++);
		}
		if(listNtwNames.size()>0)
			resultValue = ACS_CS_API_NS::Result_Success;
		else if(listNtwNames.size()==0)
			resultValue = ACS_CS_API_NS::Result_NoValue;
	}
	ACS_CS_API_NWT_R1::deleteNetworkSearchInstance(searchObj);
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NWT_Implementation::getAdminState (ACS_CS_API_NameList_R1 &listAdminStates, BoardID boardId)
{
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;
	ACS_CS_API_NWT_BoardSearch_R1 *searchObj = ACS_CS_API_NWT_R1::createNetworkSearchInstance();
	searchObj->setBoardID(boardId);
	list<ACS_CS_API_NetworkTable_R1> rows;
	ACS_CS_API_NS::CS_API_Result resultVal = getNtwTableRows(rows,searchObj);
	if(resultVal == ACS_CS_API_NS::Result_Success)
	{
		listAdminStates.setSize(rows.size());
		int i=0;
		for (std::list<ACS_CS_API_NetworkTable_R1>::iterator row=rows.begin(); row!=rows.end(); ++row)
		{
		    string admStat = row->adminState;
		    ACS_CS_API_Name adminStateName(admStat.c_str());
		    listAdminStates.setValue(adminStateName,i++);
		}
		if(listAdminStates.size()>0)
			resultValue = ACS_CS_API_NS::Result_Success;
		else if(listAdminStates.size()==0)
			resultValue = ACS_CS_API_NS::Result_NoValue;
	}
	ACS_CS_API_NWT_R1::deleteNetworkSearchInstance(searchObj);
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NWT_Implementation::getNetworkTable (std::list<ACS_CS_API_NetworkTable_R1> &tableRows, BoardID boardId)
{
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;
	ACS_CS_API_NWT_BoardSearch_R1 *searchObj = ACS_CS_API_NWT_R1::createNetworkSearchInstance();
	searchObj->setBoardID(boardId);
	list<ACS_CS_API_NetworkTable_R1> rows;
	resultValue = getNtwTableRows(tableRows,searchObj);
	ACS_CS_API_NWT_R1::deleteNetworkSearchInstance(searchObj);
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NWT_Implementation::getMAC (ACS_CS_API_MacList_R1 &macList, ACS_CS_API_NWT_BoardSearch_R1 *searchObj)
{
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;
	list<ACS_CS_API_NetworkTable_R1> rows;
	ACS_CS_API_NS::CS_API_Result resultVal = getNtwTableRows(rows,searchObj);
	if(resultVal == ACS_CS_API_NS::Result_Success)
	{
		macList.setSize(rows.size());
		int i=0;
		for (std::list<ACS_CS_API_NetworkTable_R1>::iterator row=rows.begin(); row!=rows.end(); ++row)
			macList.setValue(row->mac,i++);
		if(macList.size()>0)
			return ACS_CS_API_NS::Result_Success;
		else if(macList.size()==0)
			resultValue = ACS_CS_API_NS::Result_NoValue;
	}
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_NWT_Implementation::getBoardIds (ACS_CS_API_IdList_R1 &listObj, ACS_CS_API_NWT_BoardSearch_R1 *searchObj)
{
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;
	list<ACS_CS_API_NetworkTable_R1> rows;
	ACS_CS_API_NS::CS_API_Result resultVal = getNtwTableRows(rows,searchObj);
	if(resultVal == ACS_CS_API_NS::Result_Success)
	{
		listObj.setSize(rows.size());
		int i=0;
		for (std::list<ACS_CS_API_NetworkTable_R1>::iterator row=rows.begin(); row!=rows.end(); ++row)
			listObj.setValue(row->boardId,i++);
		if(listObj.size()>0)
			return ACS_CS_API_NS::Result_Success;
		else if(listObj.size()==0)
			resultValue = ACS_CS_API_NS::Result_NoValue;
	}
	return resultValue;
}



ACS_CS_API_NS::CS_API_Result ACS_CS_API_NWT_Implementation::getNtwTableRows (std::list<ACS_CS_API_NetworkTable_R1> &tableRows,
		ACS_CS_API_NWT_BoardSearch_R1 *searchObj)
{
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

    if (!model || searchObj == 0)	//check for null pointer
				return resultValue;

    ACS_CS_API_NWT_BoardSearch_Implementation* boardSearchImp =
        				reinterpret_cast<ACS_CS_API_NWT_BoardSearch_Implementation *> (searchObj);

    std::set<const ACS_CS_ImBase *> computResourcObjs;
    model->getObjects(computResourcObjs, COMPUTERESOURCE_T);

    std::set<const ACS_CS_ImBase *> crNetworkObjs;
    model->getObjects(crNetworkObjs, CR_NETWORKSTRUCT_T);

    std::set<const ACS_CS_ImBase *> extNetworkObjs;
    model->getObjects(extNetworkObjs, TRM_EXTERNALNETWORK_T);

    std::set<const ACS_CS_ImBase *> intNetworkObjs;
    model->getObjects(intNetworkObjs, TRM_INTERNALNETWORK_T);

    for (std::set<const ACS_CS_ImBase *>::const_iterator it = computResourcObjs.begin(); it != computResourcObjs.end(); ++it)
    {
    	const ACS_CS_ImComputeResource* computResource = dynamic_cast<const ACS_CS_ImComputeResource*> (*it);
    	if (computResource)
    	{
    		for (std::set<string>::iterator network=computResource->networks.begin(); network!=computResource->networks.end(); ++network)
    		{
    			// Now checking each network with CR_NETWORKSTRUCT_T and getting mac,net,nic
    			for (std::set<const ACS_CS_ImBase *>::const_iterator it2 = crNetworkObjs.begin();it2 != crNetworkObjs.end(); it2++)
    			{
    				const ACS_CS_ImComputeResourceNetwork* crNetwork = dynamic_cast<const ACS_CS_ImComputeResourceNetwork*> (*it2);
    				if (crNetwork && crNetwork->rdn.compare(*network) == 0)
    				{
    				    ACS_CS_API_NetworkTable_R1 row;
    				    row.boardId = getBoardIdFromComputeResource(computResource);
    				    row.mac = crNetwork->macAddress.c_str();
    				    row.netwName = crNetwork->netName.c_str();
    				    row.interfaceId = crNetwork->nicName.c_str();

    				    // checking for id & admin state for external network
    				    bool externalNtwIdFound = false;
    				    row.adminState=row.extNetwId=row.intNetwId="";
    				    for (std::set<const ACS_CS_ImBase *>::const_iterator extNetw = extNetworkObjs.begin();extNetw != extNetworkObjs.end(); extNetw++)
    				    {
    				         const ACS_CS_ImExternalNetwork* extntw = dynamic_cast<const ACS_CS_ImExternalNetwork*>(*extNetw);
    				         if (extntw && extntw->name.compare(crNetwork->netName) == 0)
    				         {
    				        	 externalNtwIdFound = true;
    				        	 row.extNetwId = ACS_CS_ImUtils::getIdValueFromRdn(extntw->networkId);
    				             row.adminState = getAdminStateName(extntw->adminState);
    				             break;
    				         }
    				    }

    				    // checking for id & admin state for internal network
    				    if (!externalNtwIdFound)
    				    {
    				        for (std::set<const ACS_CS_ImBase *>::const_iterator intNetwork = intNetworkObjs.begin();intNetwork != intNetworkObjs.end(); intNetwork++)
    				        {
    				            const ACS_CS_ImInternalNetwork* intNtw = dynamic_cast<const ACS_CS_ImInternalNetwork*> (*intNetwork);
    				            if (intNtw && intNtw->name.compare(crNetwork->netName) == 0)
    				            {
    				             row.intNetwId = ACS_CS_ImUtils::getIdValueFromRdn(intNtw->networkId);
       				             row.adminState = getAdminStateName(intNtw->adminState);
       				             break;
       				            }
    				        }
    				    }
    				    if (boardSearchImp->match(row))
    				    	tableRows.push_back(row);
    				    break; //network name found and collected details so can break
    				}
    			}
    		}
    	}
    }
    if(tableRows.size()>0)
    	resultValue = ACS_CS_API_NS::Result_Success;
    else if(tableRows.size()==0)
    	resultValue = ACS_CS_API_NS::Result_NoAccess;  // Table is empty only when there is some internal problem and CS service is not accessible

    return resultValue;
}

string ACS_CS_API_NWT_Implementation::getAdminStateName(AdminState adminState)
{
	if(adminState==LOCKED)
		return "LOCKED";
	else if(adminState==UNLOCKED)
		return "UNLOCKED";
	else if(adminState==EMPTY_ADMIN_STATE)
		return "EMPTY";
	else
		return "";
}

BoardID ACS_CS_API_NWT_Implementation::getBoardIdFromComputeResource(const ACS_CS_ImComputeResource* computResource)
{
	BoardID boardId = (BoardID)-1;
	std::set<const ACS_CS_ImBase *> objects;
	model->getObjects(objects, APBLADE_T);
	model->getObjects(objects, CPBLADE_T);
	model->getObjects(objects, OTHERBLADE_T);
	std::set<const ACS_CS_ImBase *>::const_iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImBlade *blade = dynamic_cast<const ACS_CS_ImBlade *>(*it);
		if (blade && blade->uuid == computResource->uuid)
			boardId = blade->entryId;
	}
	return boardId;
}


#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_SNMPTrapReceiver.h"
#include "ACS_CS_TrapHandler.h"


// Valorize attribute (TRAP)
const oid ACS_CS_SNMPTrapReceiver::m_oidSnmpTrap[] = 			{1,3,6,1,6,3,1,1,4,1,0};

//SCX Board Presence TRAP
const oid ACS_CS_SNMPTrapReceiver::m_oidBoardPresence[]  = 	{1,3,6,1,4,1,193,177,2,2,1,0,2};

//BSP Board Presence TRAP
const oid ACS_CS_SNMPTrapReceiver::m_BSPoidBoardPresence[] =    {1,3,6,1,4,1,193,177,2,2,8,0,0,1};

//BSP TRAP Values
const oid ACS_CS_SNMPTrapReceiver::m_BSPoidShelfNum[]      =    {1,3,6,1,4,1,193,177,2,2,8,1,2,0};
const oid ACS_CS_SNMPTrapReceiver::m_BSPoidSlotPos[]       =    {1,3,6,1,4,1,193,177,2,2,8,1,3,0};
const oid ACS_CS_SNMPTrapReceiver::m_BSPoidBoardStatus[]   =    {1,3,6,1,4,1,193,177,2,2,8,2,2,1,1,30};

//SCX TRAP Values
const oid ACS_CS_SNMPTrapReceiver::m_oidShelfNum[] =            {1,3,6,1,4,1,193,177,2,2,1,1,1,0};
const oid ACS_CS_SNMPTrapReceiver::m_oidSlotPos[] =        		{1,3,6,1,4,1,193,177,2,2,1,1,3,0};
const oid ACS_CS_SNMPTrapReceiver::m_oidBoardStatus[] = 		{1,3,6,1,4,1,193,177,2,2,1,1,7,0};

//SCB-RP Board Presence TRAP
const oid ACS_CS_SNMPTrapReceiver::m_SCBoidBoardPresence[]  = 		{1,3,6,1,4,1,193,154,2,1,0,2};

//SCB-RP TRAP Values
const oid ACS_CS_SNMPTrapReceiver::m_SCBoidMagPlugNum[] = 			{1,3,6,1,4,1,193,154,2,1,3,1};
const oid ACS_CS_SNMPTrapReceiver::m_SCBoidSlotPos[] = 				{1,3,6,1,4,1,193,154,2,1,3,3};
const oid ACS_CS_SNMPTrapReceiver::m_SCBoidBoardStatus[] = 			{1,3,6,1,4,1,193,154,2,1,3,4};

ACS_CS_SNMPTrapReceiver::ACS_CS_SNMPTrapReceiver(ACS_CS_TrapHandler *traphand): ACS_TRAPDS_API()  {

	serviceHandler = traphand->getServiceHandler();
}
ACS_CS_SNMPTrapReceiver::ACS_CS_SNMPTrapReceiver(ACS_CS_TrapHandler *traphand, int l): ACS_TRAPDS_API(l)  {

	serviceHandler =  traphand->getServiceHandler();
}


ACS_CS_SNMPTrapReceiver::~ACS_CS_SNMPTrapReceiver()
{

}



void ACS_CS_SNMPTrapReceiver::handleTrap(ACS_TRAPDS_StructVariable v2)
{

	int iRet;
	long lShelfNum;
	long lSlotPos;
	long lBoardStatus;

//	struct variable_list *vars;
//	char *ip_remote_agent;
	typTrapEnm nTrap;

	std::string ip_remote_agent("");
//	typTrapEnm nTrap;

	// Init
	iRet = TRAP_HANDLER_ERROR;
	//Child Variables
	lShelfNum = INT_TRAPVALUE_NONE;
	lSlotPos = INT_TRAPVALUE_NONE;
	lBoardStatus = INT_TRAPVALUE_NONE;

	nTrap = enmTrapNull;

	ACS_TRAPDS_varlist td;

	// Loop on all pdu variable list
	for (std::list<ACS_TRAPDS_varlist>::iterator it = v2.nvls.begin(); it != v2.nvls.end(); it++)
	{
		td = *it;
		//Check if is trap
		if(netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_oidSnmpTrap,sizeof(ACS_CS_SNMPTrapReceiver::m_oidSnmpTrap)/sizeof(oid),td.name.oids, td.name.length) == 0)
		{
			//			std::ostringstream rec_oid("");
			//			for(int i=0; i < td.name.length; i++)
			//				rec_oid << td.name.oids[i] << ".";
			//
			//			std::cout << "*********** TRAP *********************** "<< rec_oid.str() << std::endl;

			// Check value
			if (netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_oidBoardPresence,sizeof(ACS_CS_SNMPTrapReceiver::m_oidBoardPresence)/sizeof(oid),td.val.objid.oids, td.val.objid.length) == 0) {
				// Trap type: "BoardPresence".
				nTrap = enmBoardPresence;
			}

			// Check value
			else if (netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_SCBoidBoardPresence,sizeof(ACS_CS_SNMPTrapReceiver::m_SCBoidBoardPresence)/sizeof(oid),td.val.objid.oids, td.val.objid.length) == 0) {
				// Trap type: "BoardPresence".
				nTrap = enmSCBBoardPresence;
			}
			//Check value
			else if (netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_BSPoidBoardPresence,sizeof(ACS_CS_SNMPTrapReceiver::m_BSPoidBoardPresence)/sizeof(oid),td.val.objid.oids, td.val.objid.length) == 0) {
				// Trap type: "BoardPresence".
				nTrap = enmBSPBoardPresence;
			}


		} //end if trap

		switch(nTrap){

		case enmBoardPresence:
		{
			// Check for child variables

			if(netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_oidShelfNum,sizeof(ACS_CS_SNMPTrapReceiver::m_oidShelfNum)/sizeof(oid),	td.name.oids, td.name.length) == 0){
				//Set "ShelfNum value"
				lShelfNum = td.val.integer;
			}else if(netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_oidSlotPos,sizeof(ACS_CS_SNMPTrapReceiver::m_oidSlotPos)/sizeof(oid),td.name.oids, td.name.length) == 0){
				// Set "slotPos value"
				lSlotPos = td.val.integer;
			} else if(netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_oidBoardStatus,sizeof(ACS_CS_SNMPTrapReceiver::m_oidBoardStatus)/sizeof(oid),td.name.oids, td.name.length) == 0){
				// Set "boardStatus value"
				lBoardStatus = td.val.integer;
			}
			break;
		}
		case enmSCBBoardPresence:
		{       // Check for child variables

			if(netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_SCBoidSlotPos,sizeof(ACS_CS_SNMPTrapReceiver::m_SCBoidSlotPos)/sizeof(oid),td.name.oids, td.name.length - 1) == 0){
				// Set "slotPos value"
				lSlotPos = td.val.integer;
			} else if(netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_SCBoidBoardStatus,sizeof(ACS_CS_SNMPTrapReceiver::m_SCBoidBoardStatus)/sizeof(oid),	td.name.oids, td.name.length - 1) == 0){
				// Set "boardStatus value"
				lBoardStatus = td.val.integer;
			}
			break;
		}
		case enmBSPBoardPresence:
		{  //Check for child variables

			if(netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_BSPoidShelfNum,sizeof(ACS_CS_SNMPTrapReceiver::m_BSPoidShelfNum)/sizeof(oid),  td.name.oids, td.name.length) == 0){
				//Set "ShelfNum value"
				lShelfNum = td.val.integer;
			}else if(netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_BSPoidSlotPos,sizeof(ACS_CS_SNMPTrapReceiver::m_BSPoidSlotPos)/sizeof(oid),td.name.oids, td.name.length) == 0){
				// Set "slotPos value"
				lSlotPos = td.val.integer;
			} else if(netsnmp_oid_equals(ACS_CS_SNMPTrapReceiver::m_BSPoidBoardStatus,sizeof(ACS_CS_SNMPTrapReceiver::m_BSPoidBoardStatus)/sizeof(oid),td.name.oids, td.name.length) == 0){
				// Set "boardStatus value"
				lBoardStatus = td.val.integer;
			}

			break;
		}
		default:
			break;
		} //end Switch

	} // end for

	// ShelfManager Ip that has sent the trap
	ip_remote_agent = td.ipTrap;

	// Switch trap type
	switch(nTrap){
	        case enmBSPBoardPresence:
                {
                        // Print common part
                        std::cout << "*********** TRAP: BoardPresence lShelfNum = [";
                        // Check value
                        if(lShelfNum != INT_TRAPVALUE_NONE){
                                std::cout << lShelfNum << "], lSlotPos = [";

                                if(lSlotPos != INT_TRAPVALUE_NONE){
                                        // Value OK
                                        std::cout << lSlotPos << "]";
                                        // Send Ok
                                        iRet = TRAP_HANDLER_OK;
                                        if (lBoardStatus != INT_TRAPVALUE_NONE) {
                                                if (lBoardStatus == 1){
                                                        std::cout << " ...Board Inserted." << std::endl;
                                                        serviceHandler->handleBSPBoardPresenceTrap(lShelfNum, lSlotPos);
                                                   }
                                                cout << "\n";

                                        } else {
                                                // Error: Value is not evaluated!
                                                std::cout << " ... ERROR:Unknown Status" << std::endl;
                                        }

                                }else{
                                        // Error: Value is not evaluated!
                                        std::cout << "ERROR]" << std::endl;
                                }
                        }else{
                                // Error: Value is not evaluated!
                                std::cout << "ERROR]" << std::endl;
                        }
                        // Print closed part
                        //std::cout << "]" << std::endl;
                        break;
 
                }  
		case enmBoardPresence: //Board Presence from SCX
		{
			// Print common part
			std::cout << "*********** TRAP: BoardPresence lShelfNum = [";
			// Check value
			if(lShelfNum != INT_TRAPVALUE_NONE){
				std::cout << lShelfNum << "], lSlotPos = [";

				if(lSlotPos != INT_TRAPVALUE_NONE){
					// Value OK
					std::cout << lSlotPos << "]";
					// Send Ok
					iRet = TRAP_HANDLER_OK;
					if (lBoardStatus != INT_TRAPVALUE_NONE) {
						if (lBoardStatus == 1)
							std::cout << " ...Board Inserted." << std::endl;
						if (lBoardStatus == 3) {
							std::cout << " ...Board Accessible!! Trying Automatic Synchronization." << std::endl;
							serviceHandler->handleBoardPresenceTrap(lShelfNum, lSlotPos);
						}
						cout << "\n";

					} else {
						// Error: Value is not evaluated!
						std::cout << " ... ERROR:Unknown Status" << std::endl;
					}

				}else{
					// Error: Value is not evaluated!
					std::cout << "ERROR]" << std::endl;
				}
			}else{
				// Error: Value is not evaluated!
				std::cout << "ERROR]" << std::endl;
			}
			// Print closed part
			//std::cout << "]" << std::endl;
			break;
		}

		case enmSCBBoardPresence: //Board Presence from SCB-RP
		{
			// Print common part
			std::cout << "*********** TRAP: BoardPresence lShelfNum = [";
			// Check value

			std::set<const ACS_CS_ImBase *> blades;

			//Get a copy of all model
			ACS_CS_ImModel* model = ACS_CS_ImRepository::instance()->getModelCopy();

			model->getObjects(blades, OTHERBLADE_T);

			std::set<const ACS_CS_ImBase *>::const_iterator it;

			const ACS_CS_ImShelf * shelf = 0;

			for(it = blades.begin(); it != blades.end(); it++)
			{

				ACS_CS_ImBase *imBaseObj =const_cast<ACS_CS_ImBase *> (*it);

				ACS_CS_ImOtherBlade *ob = dynamic_cast<ACS_CS_ImOtherBlade *> (imBaseObj);

				if(NULL == ob)
					continue;

				if(ob->ipAddressEthA==ip_remote_agent || ob->ipAddressEthB==ip_remote_agent)
				{
					const ACS_CS_ImBase * parent = ACS_CS_ImRepository::instance()->getParentOf(ob->rdn);
					if (parent)
					{
						shelf = dynamic_cast<const ACS_CS_ImShelf *>(parent);

						if (shelf){
							unsigned found = shelf->address.find(".");
							std::string magazine = shelf->address.substr(0,found);
							lShelfNum =  atoi (magazine.c_str());

							break;
						}
					}

				}

			}

			if(lShelfNum != INT_TRAPVALUE_NONE){

				std::cout << lShelfNum << "], lSlotPos = [";

				if(lSlotPos != INT_TRAPVALUE_NONE){
					// Value OK
					std::cout << lSlotPos << "] - from IP address: "<< ip_remote_agent << " ";
					// Send Ok
					iRet = TRAP_HANDLER_OK;
					if (lBoardStatus != INT_TRAPVALUE_NONE) {
						if (lBoardStatus == 0)
							std::cout << " ...Board Inserted." << std::endl;
						if (lBoardStatus == 1) {
							std::cout << " ...Board Accessible!! Trying Automatic Synchronization." << std::endl;
							serviceHandler->handleBoardPresenceTrap(shelf->address, lSlotPos);
						}
						cout << "\n";

					} else {
						// Error: Value is not evaluated!
						std::cout << " ... ERROR:Unknown Status" << std::endl;
					}

				}else{
					// Error: Value is not evaluated!
					std::cout << "ERROR]" << std::endl;
				}
			}else{
				// Error: Value is not evaluated!
				std::cout << "ERROR]" << std::endl;
			}

			if (model)
				delete model;

			// Print closed part
			//std::cout << "]" << std::endl;
			break;
		}

		default:
			// Trap unknown!
			//std::cout << "*********** UNKNOWN TRAP: " << std::endl;
		break;

	}
	// Print out method
	//std::cout << "trapHandler return [" << iRet << "]" << std::endl;
	// Out of method

}



void ACS_CS_SNMPTrapReceiver::OidToString(oid *iodIn, unsigned int len, std::string *strOut)
{
	std::stringstream sStr;
	unsigned int x;
	// Init
	x = 0;
	sStr.str("");
	(*strOut) = "";
	// for all element in oid,
	for (x = 0; x < len ; ++x){
	   sStr << '.';
	   sStr << (int)iodIn[x];
	}
	// Set output string
	(*strOut) = sStr.str();
}


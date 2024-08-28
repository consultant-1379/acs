/*
 * @file ACS_CS_DHCP_Configurator_Test.h
 * @author mann
 * @date Sep 12, 2010
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 */

#ifndef ACS_CS_DHCP_CONFIGURATOR_TEST_H_
#define ACS_CS_DHCP_CONFIGURATOR_TEST_H_

#include "ACS_CS_Protocol.h"


void setup(void);
void connectTest(void);
void disConnectTest(void);
void addNormalDhcpBoardTest(void);
void removeNormalDhcpBoardTest(void);
void addClientDhcpBoardTest(void);
void removeClientDhcpBoardTest(void);
void tearDown(void);

void getDhcpInfoObject(ACS_CS_DHCP_Info &info,
						ACS_CS_Protocol::CS_FBN_Identifier fbn,
						ACS_CS_Protocol::CS_DHCP_Method_Identifier method,
						ACS_CS_Protocol::CS_Side_Identifier side,
						ACS_CS_Protocol::CS_Network_Identifier network,
						std::string name,
						uint32_t mag,
						unsigned short slot,
						unsigned char *mac, int size,
						uint32_t ip,
						std::string bootServer,
						std::string bootPath
						);


#endif /* ACS_CS_DHCP_CONFIGURATOR_TEST_H_ */

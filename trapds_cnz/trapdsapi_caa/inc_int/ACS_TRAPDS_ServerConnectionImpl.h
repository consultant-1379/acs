/*
 * ACS_TRAPDS_ServerConnectionImpl.h
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_SERVERCONNENTIONIMPL_H_
#define ACS_TRAPDS_SERVERCONNENTIONIMPL_H_

#include "ACS_TRAPDS_SnmpObserver.h"
#include "ACS_TRAPDS_ServerConnection.h"

#define MAX_SIZE_DATA 30000

class ACS_TRAPDS_ServerConnectionImpl : public ACS_TRAPDS_ServerConnection{
public:

	ACS_TRAPDS_ServerConnectionImpl(std::string name, int port);
	virtual ~ACS_TRAPDS_ServerConnectionImpl();
	virtual int sendPackage();
	inline std::string getNameSubscriber(){ return nameSubscriber;}

private:
	std::string nameSubscriber;
	int portSubscriber;
};


#endif /* ACS_TRAPDS_SERVERCONNENTIONIMPL_H_ */

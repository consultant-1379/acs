#include "ACS_DSD_CommonTypes.h"
#include "ACS_DSD_MacrosConstants.h"
#include "string.h"

ServiceInfo::ServiceInfo()
{
	memset(serv_name,'\0',acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX);
	memset(serv_domain,'\0',acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX);
	connection_type = acs_dsd::CONNECTION_TYPE_UNKNOWN;
	NrOfConn = 0;
	ip1 = 0;
	port1 = 0;
	ip2 = 0;
	port2 = 0;
}


ServiceInfo::ServiceInfo(const ServiceInfo & rhs)
{
	memcpy(this->serv_name, rhs.serv_name, acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX);
	memcpy(this->serv_domain, rhs.serv_domain, acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX);
	this->connection_type = rhs.connection_type;
	this->NrOfConn = rhs.NrOfConn;
	this->ip1 = rhs.ip1;
	this->port1 = rhs.port1;
	this->ip2 = rhs.ip2;
	this->port2 = rhs.port2;
}


ServiceInfo::ServiceInfo(char serv_name[acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX],
						char serv_domain[acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX],
						uint16_t connection_type,
						uint16_t NrOfConn,
						uint32_t ip1,
						uint16_t port1,
						uint32_t ip2,
						uint16_t port2)
{
	memcpy(this->serv_name, serv_name, acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX);
	memcpy(this->serv_domain, serv_domain, acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX);
	this->connection_type = connection_type;
	this->NrOfConn =  NrOfConn;
	this->ip1 = ip1;
	this->port1 = port1;
	this->ip2 = ip2;
	this->port2 = port2;
}


ServiceInfo & ServiceInfo::operator=(const ServiceInfo & rhs)
{
	memcpy(this->serv_name, rhs.serv_name, acs_dsd::CONFIG_APP_SERVICE_NAME_SIZE_MAX);
	memcpy(this->serv_domain, rhs.serv_domain, acs_dsd::CONFIG_APP_DOMAIN_NAME_SIZE_MAX);
	this->connection_type = rhs.connection_type;
	this->NrOfConn = rhs.NrOfConn;
	this->ip1 = rhs.ip1;
	this->port1 = rhs.port1;
	this->ip2 = rhs.ip2;
	this->port2 = rhs.port2;

	return *this;
}

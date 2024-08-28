/*
 * ACS_TRAPDS_ACS_TRAPDS_ServerConnection.cpp
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#include "ACS_TRAPDS_ServerConnection.h"


ACS_TRAPDS_ServerConnection::ACS_TRAPDS_ServerConnection()
{
	data_buf=0;
	data_buf_len=0;
}

ACS_TRAPDS_ServerConnection::~ACS_TRAPDS_ServerConnection() {
	// TODO Auto-generated destructor stub
	if(!dataStringStream)
		delete dataStringStream;
}


//Expect data to arrive from the remote machine. Accept it and display
//it. After receiving data, immediately send some data back to the
//remote.

//int ACS_TRAPDS_ServerConnection::accept_data()
//{
//	int byte_count=0;
//
//	while( (byte_count=ACS_TRAPDS_Sender::getInstance()->getInetAddrSender()->recv(data_buf,SIZE_DATA,*remote_addr_))!=-1)
//	{
//		data_buf[byte_count]=0;
//
//		ACE_OS::sleep(1);
//
//		if(send_data()==-1)
//			break;
//	}
//	return -1;
//
//}

//Method used to send data to the remote using the datagram component
//local_

int ACS_TRAPDS_ServerConnection::send_data()
{
	ACS_TRAPDS_StructVariable v=ACS_TRAPDS_StructVariable(pdu,ipTrap);

	v.insertData();

	save_trap_date(v);


	if(ACS_TRAPDS_Sender::getInstance()->getInetAddrSender()->send(data_buf, data_buf_len,*remote_addr_)==-1){

		printf("ERROR: Failed to send Serialized Data!\n");

		return -1;
	}
	else
	{

		printf("Serialized Data successfully sent!\n");

		return 0;
	}

	if(!data_buf)
	{
		delete(data_buf);
		data_buf=0;
		data_buf_len=0;
	}

	return 0;

}

int ACS_TRAPDS_ServerConnection::setPort(int port)
{
	this->port=port;

	return 0;
}


int ACS_TRAPDS_ServerConnection::setPdu(snmp_pdu *pdu){

	this->pdu=pdu;

	return 0;
}

int ACS_TRAPDS_ServerConnection::setIpTrap(std::string ipTrap){

	this->ipTrap=ipTrap;

	return 0;
}


void ACS_TRAPDS_ServerConnection::save_trap_date(const ACS_TRAPDS_StructVariable &v){
    // make an archive

	std::stringstream dataStringStream;

	boost::archive::text_oarchive oa(dataStringStream);
	oa<<v;

	std::string strTrap = dataStringStream.str();

	if(data_buf)
	{
		delete data_buf;
		data_buf=0;
		data_buf_len=0;
	}

	data_buf_len = strTrap.size();
	data_buf = new char[data_buf_len + 1];
	memset(data_buf,0,data_buf_len+1);
	strTrap.copy(data_buf,data_buf_len,0);
}

void ACS_TRAPDS_ServerConnection::restore_trap_date(ACS_TRAPDS_StructVariable &v)
{
    std::ifstream ifs(filename.c_str());
    boost::archive::text_iarchive ia(ifs);
    ia >> v;
}

int ACS_TRAPDS_ServerConnection::setFilename(std::string fn)
{
	filename=fn;

	return 0;
}

void ACS_TRAPDS_ServerConnection::setRemoteAddress(std::string ra)
{
	remote_addr_->set(port,ra.c_str());

}

void ACS_TRAPDS_ServerConnection::setRemotePort(int rp)
{
	remote_port=rp;
}

void ACS_TRAPDS_ServerConnection::setLocalPort(int lp)
{
	local_port=lp;
}


int ACS_TRAPDS_ServerConnection::setConnection()
{
	if (!remote_port)
	{
		printf("setConnection %d %d error\n",remote_port,local_port);

		return -1;
	}

	printf("setConnection %d %d\n",remote_port,local_port);

	dataStringStream=new std::stringstream(std::ios::in | std::ios::out | std::ios::binary);

	remote_addr_=new ACE_INET_Addr(remote_port,"127.0.0.1");

	return 0;

}





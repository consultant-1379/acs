#ifndef ACS_TRAPDS_SENDER_H_
#define ACS_TRAPDS_SENDER_H_

#include "ace/OS.h"
#include "ace/SOCK_Dgram.h"
#include "ace/INET_Addr.h"


#define MIN_PORT 49154
#define MAX_PORT 49169


class ACS_TRAPDS_Sender {

public:

        static  ACS_TRAPDS_Sender * getInstance ();

        ACE_SOCK_Dgram *getInetAddrSender();

private:

        ACS_TRAPDS_Sender();
        virtual ~ACS_TRAPDS_Sender();
        static ACS_TRAPDS_Sender* s_instance;
        ACE_INET_Addr *local_addr_;
        ACE_SOCK_Dgram *local_;
};




#endif /* ACS_CS_TRAPDS_SENDER_H_ */

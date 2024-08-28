#ifndef _LINUX_ERI_IPMI_H
#define _LINUX_ERI_IPMI_H

#include <linux/ioctl.h>

#define ERI_IPMI_DATA_MAX_SIZE   105

struct eri_ipmi_message {
	unsigned char groupcode;
	unsigned char commandcode;
	unsigned char result;
	unsigned char size;
	unsigned char data[ERI_IPMI_DATA_MAX_SIZE];
};

struct eri_ipmb_header {
	unsigned char dest_addr;
	unsigned char netfn_lun;
	unsigned char head_chs;
};

struct eri_ipmb_req {
	unsigned char res_addr;
	unsigned char netfn_res_lun;
	unsigned char head_chs;
	unsigned char req_addr;
	unsigned char req_seq_req_lun;
	unsigned char cmd;
	unsigned char data_chs[26];
};

struct eri_ipmb_res {
	unsigned char req_addr;
	unsigned char netfn_req_lun;
	unsigned char head_chs;
	unsigned char res_slave_addr;
	unsigned char req_seq_res_lun;
	unsigned char cmd;
	unsigned char completion;
	unsigned char data_chs[25];
};

/* IOCTLS */

/* IOCTL data structures */
struct eri_ipmi_local_cmd_handler {
	unsigned char groupcode;
	unsigned char commandcode;
};

struct eri_ipmi_ipmb_cmd_handler {
	unsigned char req_addr;
	unsigned char netfn_res_lun;
	unsigned char cmd;
};

/*
 * Register to receive all commands, though commands subscribed to
 * with subsequent ioctls are not received by the file descriptor.
 */
#define REGISTER_GLOBAL_CMD_HANDLER  _IO(0xA1, 0)
/*
 * Register to receive a spefic local cmd, returns EBUSY
 * if a handler is already registered
 */
#define REGISTER_LOCAL_CMD _IOW(0xA1, 1, struct eri_ipmi_local_cmd_handler)
/*
 * Unregister the handler for a spefic local cmd, usefull
 * if a handler is already registered
 */
#define UNREGISTER_LOCAL_CMD _IOW(0xA1, 2, struct eri_ipmi_local_cmd_handler)
/*
 * Register to receive a spefic ipmb cmd, returns EBUSY
 * if a handler is already registered
 */
#define REGISTER_IPMB_CMD  _IOW(0xA1, 3, struct eri_ipmi_ipmb_cmd_handler)
/*
 * Unregister the handler for a spefic ipmb cmd, usefull
 * if a handler is already registered
 */
#define UNREGISTER_IPMB_CMD  _IOW(0xA1, 4, struct eri_ipmi_ipmb_cmd_handler)
/*
 * Sniffing mode, listen to all traffic to the IPMI.
 */
#define PROMISCUOUS_MODE _IO(0xA1, 5)
/*
 * Change the timeout, for upgrading GEP1 boards
 */
#define SET_TRANSMISSION_TIMEOUT _IOW(0xA1, 6, unsigned long)

static inline int ipmi_message_is_ipmb(struct eri_ipmi_message *message)
{
	return (message->groupcode & 0x7F) == 0x08 ? 1 : 0;
}

static inline int ipmb_message_is_response(struct eri_ipmb_header *header)
{
	if (((header->netfn_lun >> 2) % 2) == 1) {
		return 1;
	} else {
		return 0;
	}
}

static inline int ipmi_message_is_response(struct eri_ipmi_message *message)
{
	if (ipmi_message_is_ipmb(message)) {
		return ipmb_message_is_response(
			(struct eri_ipmb_header *)message->data);
	} else if ((message->groupcode & 0x80) == 0x80) {
		return 1;
	} else {
		return 0;
	}
}

/* The if statement around address 20 is due to a bug in shelf manager */
static inline int match_ipmb_response(struct eri_ipmb_res *received,
				      struct eri_ipmb_req *sent)
{
	if (sent->res_addr == received->res_slave_addr &&
	    (sent->netfn_res_lun >> 2) + 1 == received->netfn_req_lun >> 2 &&
	    sent->cmd == received->cmd) {
		return 1;
	} else if ((sent->res_addr == 0x20 &&
		    (received->res_slave_addr == 0x80 ||
		     received->res_slave_addr == 0xb2)) &&
		   (sent->netfn_res_lun >> 2) + 1 == received->netfn_req_lun >> 2 &&
		   sent->cmd == received->cmd) {
		return 1;
	} else {
		return 0;
	}
}

static inline int match_ipmi_response(struct eri_ipmi_message *received,
				      struct eri_ipmi_message *sent)
{
	if (ipmi_message_is_ipmb(received)) {
		return match_ipmb_response(
			(struct eri_ipmb_res *)received->data,
			(struct eri_ipmb_req *)sent->data);
	} else 	if (sent->groupcode + 0x80 == received->groupcode &&
		    sent->commandcode == received->commandcode) {
		return 1;
	} else {
		return 0;
	}
}

static inline int ipmb_message_is_broadcast(struct eri_ipmb_header *header)
{
	return header->dest_addr == 0x00 ? 1 : 0;
}

static inline int ipmi_message_requires_response(struct eri_ipmi_message *msg)
{
	if (ipmi_message_is_response(msg)) {
		return 0;
	}

	if (ipmi_message_is_ipmb(msg) &&
	    ipmb_message_is_broadcast(
		    (struct eri_ipmb_header *)msg->data)) {

		return 0;
	}

	return 1;
}

static inline unsigned char ipmb_checksum(unsigned char* uc, int size)
{
        unsigned int sum = 0;
        unsigned char res;
        int i;

        for (i = 0; i < size; i++) {
                sum += (unsigned int)uc[i];
	}
        sum &= 0xFF;
        sum = 0x0100 - sum;
        res = (unsigned char)(sum & 0xFF);

        return res;
}


#ifdef __KERNEL__

#define ERI_IPMI_MINOR          241     /* Ericsson IPMI */

/* POST CODES */
#define LINUX_START                     0x020000
#define OS_CRASH                        0x02ffff
#define KDUMP_START                     0x030000
#define KDUMP_KERNEL_UP                 0x030280
#define KDUMP_KERNEL_END                0x03ffff
#define UNKNOWN_POSTCODE                0xffffff

struct ipmi_post_codes {
    unsigned initrd_unzip_err;
    unsigned initrd_bad_sz;
    unsigned initrd_pop_done;
    unsigned boot_failed;
    unsigned module_init;
    unsigned module_init_done;
    unsigned run_init_failed;
};

extern struct ipmi_post_codes ipmi_codes;
extern int ipmiuser_debug_enable;

int write_eri_ipmi_postcode(const unsigned int postcode);

void eri_ipmi_select_kdump_kernel(void);

void eri_ipmi_early_init(void);

void eri_ipmi_kernelcrash(char short_error_code, u32 crash_data);

int eri_ipmi_initiate_nmi(unsigned int seconds);

void eri_ipmi_print_err(void);

#endif /* __KERNEL__ */

#endif /* _LINUX_ERI_IPMI_H */

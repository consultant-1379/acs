#ifndef ACS_DSD_IMM_FUNCTIONS_H_
#define ACS_DSD_IMM_FUNCTIONS_H_

#include <string.h>
#include "ACS_DSD_Macros.h"

class ACS_DSD_ImmConnectionHandler;
class ACE_Recursive_Thread_Mutex;

// Initializes DSD Server's IMM components
int init_IMM(const ACS_DSD_Node & dsd_local_node, bool clean_imm_entry_for_local_node = false);

// Unregister  the services not running from IMM
int unregisterOldServicesfromIMM(const ACS_DSD_Node & dsd_local_node);

// checks if the process <process_name> is still running
bool  isServiceProcessRunning(const char *process_name,int32_t process_pid);

// finalize DSD Server's IMM components
int finalize_IMM(const ACS_DSD_Node & dsd_local_node,  bool clean_imm_entry_for_local_node = false);

/*
 * 	Try to become the controller of the IMM shared objects, making <nRetry>,
 *  Return 0 on success, a value < 0 on failure.
 *  In case of success, the <nRety argument is set with the number of retry performed
*/
int take_control_of_IMM_shared_objects(int & nRetry);

/*
*  Release the control of the IMM shared objects
*  Return 0 on success, a value < 0 on failure.
 *  In case of success, the nRetry argument is set with the number of retry performed
*/
int release_control_of_IMM_shared_objects(int & nRetry);


/*
 *  Reset flags used to handle pending operations on IMM connections.
*/
int reset_IMM_op_pending_flags(bool lock_imm_manteninance_mutex = true);

/*
 *  Check the two connections of DSD Server towards IMM ( used for PRIVATE and SHARED objects handling) and, if down, tries to restore them.
*/
int check_and_restore_IMM_connections();

int update_local_node_state_in_IMM();

/*
 * Utility function: verify the integrity of a global IMM connection object, and eventually try to restore it.
 * Takes as arguments:
 * - imm_conn_handler_p : reference to the global pointer containing the address of the global IMM connection object to be verified and eventually restored
 * - imm_conn_mutex_p : address of the the recursive thread mutex object used to synchronize access to the global IMM connection object
 * - do_init : indicates if registration ('init') has to be performed after restore
 */
int check_and_restore_imm_conn_object(ACS_DSD_ImmConnectionHandler *& imm_conn_handler_p, ACE_Recursive_Thread_Mutex * imm_conn_mutex_p, bool do_init = true);

#endif /* ACS_DSD_IMM_FUNCTIONS_H_ */

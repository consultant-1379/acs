#include "acs_apbm_api.h"

int main (int /*argc*/, char * /*argv*/ []) {
	acs_apbm_api apbm_api;

	int call_result = apbm_api.set_board_info(5, "oid_number", "oid_value");
	printf("call_result == %d\n", call_result);

	call_result = apbm_api.safe_reset_board(5, "192.168.169.55", "192.168.169.57");
	printf("call_result == %d\n", call_result);

	return 0;
}

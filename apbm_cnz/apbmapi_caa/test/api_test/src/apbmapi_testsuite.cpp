#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include "/vobs/cm4ap/ntacs/apbm_cnz/apbmapi_caa/inc_ext/acs_apbm_api.h"


#define TRUE 1

acs_apbm_api test_api;



int Test_setUp(const char * devname);
void Test_get_board_location();					/* method 0*/
void Test_get_board_status();				/* method 1*/
void Test_set_board_status();					/* method 2*/
void Test_check_board_presence();		/* method 3*/


int main(int argc, char *argv[])
{
   int c; 
   int cont_flag =1;

   printf("\nStart test for APBMPAPI \n\n");


   // main Test application loop
   while (cont_flag)
   {
		if(argc == 1)
		{
		   printf("TEST MENU ...:\n");
		   printf("\n0. Test_get_board_location()\n" );
		   printf("1. Test_get_board_status()\n" );
		   printf("2. Test_set_board_status() \n");
		   printf("3. Test_check_board_presence() \n");
		   printf("Q. exit\n");
		   printf("\nEnter a choice --->");
		   do
		   {
			   c = getchar();
		   }while (c == '\n');
		}
		else {
		   c = *argv[1];
		   cont_flag = 0;
		}

		char dummy = getchar();

		switch(c)
		{
		case '0':
			Test_get_board_location();
			break;
		case '1':
			Test_get_board_status();
			break;
		case '2':
			Test_set_board_status();
			break;
		case '3':
			Test_check_board_presence();
			break;

		case 'q':
		case 'Q':
			cont_flag = 0;
			break;
		default:
			printf("Invalid choice !\n");
			break;
		}
		exit (0);

		if(cont_flag)
		{
			printf("\npress any key to continue ... ");
			dummy = getchar();
		}
	}


	return 0;
}


void Test_get_board_location()
{
	int call_result;
	int tmp_idx = -1;
	acs_apbm::board_name_t board_name;

	while(1)
	{
		printf("type board name ( 0 --> BOARD_NAME_DVD,  1 --> BOARD_NAME_GEA ): ");
		int call_result = scanf("%d%*c", & tmp_idx);
		if((call_result == 1) && ((tmp_idx == 0) || (tmp_idx == 1)))   break;
		printf("invalid input ! Call 'scanf' returned '%d; board name== %d'\n", call_result, tmp_idx);
	}

	switch(tmp_idx)
	{
	case 0:
		board_name = acs_apbm::BOARD_NAME_DVD;
		break;
	case 1:
		board_name = acs_apbm::BOARD_NAME_GEA;
		break;
	default:
		break;
	}

	std::list<int> ap_sys_no_list;
	std::list<int>::iterator it;
	call_result = test_api.get_board_location(board_name, ap_sys_no_list);
	if (call_result <0){
		printf("Call 'get_board_location failed  <call_result  == %d>\n", call_result);
	}
	else {
		printf("Call 'get_board_location successful executed!\n");
		int i=0;
		for (it = ap_sys_no_list.begin(); it != ap_sys_no_list.end(); it++,i++ )
			printf("ap_sys_no_list[%d] == %d\n",i, *it);
	}
}


void Test_set_board_status()
{
	int call_result;
	int tmp_idx = -1;
	int par2 = -1;
	acs_apbm::board_name_t board_name;
	acs_apbm::reported_board_status_t board_status;

	while(1)
	{
		printf("type board name ( 0 --> BOARD_NAME_DVD,  1 --> BOARD_NAME_GEA ): ");
		int call_result = scanf("%d%*c", & tmp_idx);
		if((call_result == 1) && ((tmp_idx == 0) || (tmp_idx == 1))) {
			printf("type board state ( -1 --> FAULTY,  0 --> WORKING ): ");
			 call_result = scanf("%d%*c", & par2);
		}
		else continue;
		if((call_result == 1) && ((par2 == -1) || (par2 == 0))) break;
		printf("invalid input ! Call 'scanf' returned '%d; board name== %d board_status == %d'\n", call_result, tmp_idx, par2);
	}

	switch(tmp_idx)
	{
	case 0:
		board_name = acs_apbm::BOARD_NAME_DVD;
		break;
	case 1:
		board_name = acs_apbm::BOARD_NAME_GEA;
		break;
	default:
		break;
	}

	switch(par2)
		{
		case -1:
			board_status = acs_apbm::REPORTED_BOARD_STATUS_FAULTY;
			break;
		case 0:
			board_status = acs_apbm::REPORTED_BOARD_STATUS_WORKING;
			break;
		default:
			break;
		}

	call_result = test_api.set_board_status(board_name, board_status);

	if (call_result == 0)
		printf("set_board_status() successful executed!\n");
	else
		printf("set_board_status() failed! <error_code == %d>\n", call_result);
}

void Test_get_board_status()
{
	int call_result;
	int tmp_idx = -1;
	acs_apbm::board_name_t board_name;

	while(1)
	{
		printf("type board name ( 0 --> BOARD_NAME_DVD,  1 --> BOARD_NAME_GEA ): ");
		int call_result = scanf("%d%*c", & tmp_idx);
		if((call_result == 1) && ((tmp_idx == 0) || (tmp_idx == 1)))   break;
		printf("invalid input ! Call 'scanf' returned '%d; board name== %d'\n", call_result, tmp_idx);
	}

	switch(tmp_idx)
	{
	case 0:
		board_name = acs_apbm::BOARD_NAME_DVD;
		break;
	case 1:
		board_name = acs_apbm::BOARD_NAME_GEA;
		break;
	default:
		break;
	}

	call_result = test_api.get_board_status(board_name);

	switch(call_result){

	case acs_apbm::BOARD_STATUS_BOARD_WORKING:
		printf("Call 'get_board_status executed <status  == BOARD_STATUS_BOARD_WORKING>\n");
		break;
	case acs_apbm::BOARD_STATUS_BOARD_FAULTY:
		printf("Call 'get_board_status executed <status  == BOARD_STATUS_BOARD_FAULTY>\n");
		break;
	case acs_apbm::BOARD_STATUS_BOARD_BLOCKED :
		printf("Call 'get_board_status executed <status  == BOARD_STATUS_BOARD_BLOCKED>\n");
		break;
	case acs_apbm::BOARD_STATUS_BOARD_MISSING:
		printf("Call 'get_board_status executed <status  == BOARD_STATUS_BOARD_MISSING>\n");
		break;
	default:
		printf("Call 'get_board_status executed <status  == BOARD_STATUS_BOARD_ERROR>\n");
		break;
	}

}

void Test_check_board_presence()
{
	int call_result;
	int tmp_idx = -1;
	acs_apbm::board_name_t board_name ;

	while(1)
	{
		printf("type board name ( 0 --> BOARD_NAME_DVD,  1 --> BOARD_NAME_GEA ): ");
		int call_result = scanf("%d%*c", & tmp_idx);
		if((call_result == 1) && ((tmp_idx == 0) || (tmp_idx == 1)))   break;
		printf("invalid input ! Call 'scanf' returned '%d; board name== %d'\n", call_result, tmp_idx);
	}

	switch(tmp_idx)
	{
	case 0:
		board_name = acs_apbm::BOARD_NAME_DVD;
		break;
	case 1:
		board_name = acs_apbm::BOARD_NAME_GEA;
		break;
	default:
		break;
	}

	call_result = test_api.check_board_presence(board_name);

	switch(call_result){

	case acs_apbm::BOARD_PRESENCE_BOARD_PRESENT:
		printf("Call 'get_board_presence executed <status  == BOARD_PRESENCE_BOARD_PRESENT>\n");
		break;
	case acs_apbm::BOARD_PRESENCE_BOARD_NOT_CONFIGURED:
		printf("Call 'get_board_presence executed <status  == BOARD_PRESENCE_BOARD_NOT_CONFIGURED>\n");
		break;
	case acs_apbm::BOARD_PRESENCE_BOARD_NOT_AVAILABLE :
		printf("Call 'get_board_presence executed <status  == BOARD_PRESENCE_BOARD_NOT_AVAILABLE>\n");
		break;
	default:
		printf("Call 'get_board_presence executed <status  == BOARD_PRESENCE_ERROR\n");
		break;
	}

}


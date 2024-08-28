#include <stdio.h>
#include <termios.h>
#include "acs_apbm_ipmiapi.h"
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

#define TRUE 1

acs_apbm_ipmiapi test_api;

int Test_setUp(const char * devname);
void Test_enter_level();					/* method 0*/
void Test_get_mac_address();				/* method 1*/
void Test_get_product_id();					/* method 2*/
void Test_get_parameter_tableblock();		/* method 3*/
void Test_get_ipmi_fwinfo();				/* method 4*/
void Test_get_reset_line_state ();			/* method 5*/
void Test_flash_mode_activate();			/* method 6*/
void Test_flash_srec();						/* method 7*/
void Test_get_led();						/* method 8*/
void Test_set_redled();						/* method 9*/
void Test_set_yellowled();					/* method A*/
void Test_get_ext_alarms();					/* method B*/
void Test_set_alarm_panel();				/* method C*/
void Test_presence_ack();					/* method D*/
void Test_ipmi_restart();					/* method E*/
void Test_get_board_address();				/* method F*/
void Test_close();

int main(int argc, char *argv[])
{
   int c; 
   int cont_flag =1;
   char devname[64] = {0};

   printf("\nStart test for PIPAPI \n\n");

   printf("Enter the device name (the default is '/dev/eri_ipmi') : ");
   scanf("%s",devname);

   // initialize Test Application
   Test_setUp(devname);

   // main Test application loop
   while (cont_flag)
   {
		if(argc == 1)
		{
		   printf("TEST MENU ...:\n");
		   printf("\n0. Test_enter_level()\n" );
		   printf("1. Test_get_mac_address()\n" );
		   printf("2. Test_get_product_id() \n");
		   printf("3. Test_get_parameter_tableblock() \n");
		   printf("4. Test_get_ipmi_fwinfo() \n");
		   printf("5. Test_get_reset_line_state() \n");
		   printf("6. Test_flash_mode_activate() \n");
		   printf("7. Test_flash_srec()) \n");
		   printf("8. Test_get_led() \n");
		   printf("9. Test_set_redled() \n");
		   printf("A. Test_set_yellowled() \n");
		   printf("B. Test get_ext_alarms() \n");
		   printf("C  Test_set_alarm_panel() \n");
		   printf("D  Test_presence_ack() \n");
		   printf("E  Test_ipmi_restart() \n");
		   printf("F  Test_get_board_address() \n");
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
			Test_enter_level();
			break;
		case '1':
			Test_get_mac_address();
			break;
		case '2':
			Test_get_product_id();
			break;
		case '3':
			Test_get_parameter_tableblock();
			break;
		case '4':
			Test_get_ipmi_fwinfo();
			break;
		case '5':
			Test_get_reset_line_state();
			break;
		case '6':
			Test_flash_mode_activate();
			break;
		case '7':
			Test_flash_srec();
			break;
		case '8':
			Test_get_led();
			break;
		case '9':
			Test_set_redled();
			break;
		case 'A':
		case 'a':
			Test_set_yellowled();
			break;
		case 'b':
		case 'B':
			Test_get_ext_alarms();
			break;
		case 'c':
		case 'C':
			Test_set_alarm_panel();
			break;
		case 'd':
		case 'D':
			Test_presence_ack();
			break;
		case 'e':
		case 'E':
			Test_ipmi_restart();
			break;
		case 'f':
		case 'F':
			Test_get_board_address();
			break;
		case 'q':
		case 'Q':
			cont_flag = 0;
			break;
		default:
			printf("Invalid choice !\n");
			break;
		}

		printf("\nLast PIPAPI implementation error == %d\n", test_api.last_impl_error());
		exit (0);

		if(cont_flag)
		{
			printf("\npress any key to continue ... ");
			dummy = getchar();
		}
	}

	Test_close();

	return 0;
}

int Test_setUp(const char * devname)
{
	int retcode = test_api.ipmiapi_init(devname);
	if (retcode != acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
	{
		printf("Call 'ipmiapi_init()' failed ! <retcode == %d>\n", retcode);
		return retcode;
	}
	else
		printf("PIPAPI successfully initialized. Call 'ipmiapi_init()' returned with <retcode == %d>\n\n", retcode);

	 return 0;
}


void Test_enter_level()
{
	unsigned int level;
	while(1)
	{
		printf("type the level of operation ( 0 == bootloader, 1 == OS, 2 == application): ");
		int call_result = scanf("%u", &level);
		if(call_result == 1) break;
		printf("invalid input ! Call 'scanf' returned '%d'\n", call_result);
	}

	int retcode = test_api.enter_level(level);
	if (retcode != acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
		printf("Call 'enter_level()' failed ! <retcode == %d>\n", retcode);
	else
		printf("Call 'enter_level()' successfully executed !\n");
}


void Test_get_mac_address()
{
	int call_result;
	acs_apbm_ipmiapi_ns::mac_adddress_info mac_info;

	call_result = test_api.get_mac_address(&mac_info);
	if (call_result == acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
	{
		printf("boot mac:                  %02x:%02x:%02x:%02x:%02x:%02x\n",
			   mac_info.boot_mac[0],
			   mac_info.boot_mac[1],
			   mac_info.boot_mac[2],
			   mac_info.boot_mac[3],
			   mac_info.boot_mac[4],
			   mac_info.boot_mac[5]);
		printf("ericsson mac:              %02x:%02x:%02x:%02x:%02x:%02x\n",
			   mac_info.eri_mac[0],
			   mac_info.eri_mac[1],
			   mac_info.eri_mac[2],
			   mac_info.eri_mac[3],
			   mac_info.eri_mac[4],
			   mac_info.eri_mac[5]);
		printf("consecutive:               %d\n", mac_info.num);
	} else {
	  printf("Call 'get_mac_address() failed ! <retcode == %d>\n", call_result);
	}
}

void Test_get_product_id()
{
	int call_result;
	acs_apbm_ipmiapi_ns::product_id product_id;

	call_result = test_api.get_product_id(& product_id);
	if (call_result == acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
	{
		printf( "Product Number: %s\tRevision: %s\n"
				"Product Name: %s\tManifacturing Date: %s\tSerial Number: %s\n"
				"Vendor Name: %s\n",
				product_id.product_number, product_id.product_revision,
				product_id.product_name, product_id.product_date, product_id.serial_number,
				product_id.vendor_name);

/*
		printf("product number == '%s', product name == '%s', product_revision == '%s', product date == '%s', product serial_number == '%s', product_vendor_name == '%s'\n:",
				product_id.product_number, product_id.product_name, product_id.product_revision,
				product_id.product_date, product_id.serial_number, product_id.vendor_name);
*/
	}
	else
	  printf("Call 'get_product_id() failed ! <retcode == %d>\n", call_result);
}

void Test_get_parameter_tableblock()
{
	int call_result;
	unsigned int flash_type;
	unsigned int block_index;
	uint8_t buffer[1024]={0};

	printf("type the 'flash type ( 0 == board table type, 1 == board running parameter table, 2 == sensor table, 3 == IPMI FW) : " );
	call_result = scanf("%u%*c", & flash_type);

	printf("type the 'block index ( zero-based block offset (one block is 64 bytes) : " );
	call_result = scanf("%u%*c", & block_index);

	call_result = test_api.get_parameter_tableblock(flash_type, block_index, buffer);
	if (call_result == acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
	{
		for(int i = 0; i < 8; ++i)
		{
			printf("\nBUFFER LINE [%d]: ", i);
			for(int j = 0; j < 8; j++)
				printf("'%02x' ", buffer[i * 8 + j]);
		}
	}
	else
	  printf("Call 'get_parameter_tableblock() failed ! <retcode == %d>\n", call_result);
}

void Test_get_ipmi_fwinfo()
{
	int call_result;
	acs_apbm_ipmiapi_ns::ipmi_fwinfo ipmi_fwinfo;

	call_result = test_api.get_ipmi_fwinfo(& ipmi_fwinfo);
	if (call_result == acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
		printf("IPMI FW INFO: rev_major == '%u', rev_minor == '%u'\n", ipmi_fwinfo.rev_major, ipmi_fwinfo.rev_minor);
	else
	  printf("Call 'get_ipmi_fwinfo() failed ! <retcode == %d>\n", call_result);
}

void Test_get_reset_line_state ()
{
	unsigned int line_index;
	uint8_t line_state;

	while(1)
	{
		printf("type the reset line index: ");
		int call_result = scanf("%u%*c", & line_index);
		if(call_result == 1)  break;
		printf("invalid input ! Call 'scanf' returned '%d'\n", call_result);
	}

	int retcode = test_api.get_reset_line_state(line_index,  & line_state);
	if (retcode != acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
		printf("Call 'reset_line_state()' failed ! <retcode == %d>\n", retcode);
	else
		printf("reset line having index '%u' is in state '%u' !\n", line_index, line_state);
}

void  Test_flash_mode_activate()
{
	printf("test not defined for this API method !\n");
}

void  Test_flash_srec()
{
	printf("test not defined for this API method !\n");
}

void Test_get_led()
{
	int call_result;
	int led_index = -1;
	uint8_t source_bits;

	while(1)
	{
		printf("type the LED type ( 0 == green, 1 == yellow, 2 == red): ");
		int call_result = scanf("%d%*c", & led_index);
		if((call_result == 1) && (led_index >= 0) && (led_index <= 2))  break;
		printf("invalid input ! Call 'scanf' returned '%d; led_index == %d'\n", call_result, led_index);
	}

	call_result = test_api.get_led(led_index, & source_bits);
	if (call_result == acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
	{
		printf("  Status: 1 = ON, 0 = OFF\n");
		printf("  Ordered by Shelf Manager --+\n");
		printf("  Ordered by IPMI FW -------+|\n");
		printf("  Ordered by PCPU ---------+||\n");
		printf("  Ordered by HW Switch ---+|||\n");
		printf("                          ||||\n");

		printf("                      ");
		for(uint8_t i = 8; i > 0; --i)
			printf("%d", (source_bits >> (i-1)) & 1);
	}
	else
	  printf("Call 'get_led() failed ! <retcode == %d>\n", call_result);
}

void Test_set_redled()
{
	int status = -1;
	int call_result = -1;

	while(1)
	{
		printf("type the status to set for the RED led ( 0 == OFF, 1 == ON) : ");
		call_result = scanf("%d%*c", & status);
		if( (call_result == 1) && (status == 0 || status == 1))  break;
		printf("invalid input ! Call 'scanf' returned '%d', status == %u \n", call_result, status);
	}

	call_result = test_api.set_redled(status);
	if (call_result != acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
		printf("Call 'set_redled()' failed ! <retcode == %d>\n", call_result);
	else
		printf("'set_redled()' successfully executed !\n");
}

void Test_set_yellowled()
{
	int status = -1;
	int call_result = -1;

	while(1)
	{
		printf("type the status to set for the YELLOW led ( 0 == OFF, 1 == ON) : ");
		call_result = scanf("%d%*c", & status);
		if( (call_result == 1) && (status == 0 || status == 1))  break;
		printf("invalid input ! Call 'scanf' returned '%d', status == %u \n", call_result, status);
	}

	call_result = test_api.set_yellowled(status);
	if (call_result != acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
		printf("Call 'set_yellowled()' failed ! <retcode == %d>\n", call_result);
	else
		printf("'set_yellowled()' successfully executed !\n");
}

void Test_get_ext_alarms()
{
	int call_result;
	int tmp_idx = -1;
	acs_apbm_ipmiapi_ns::sensor_index sensor_index;
	acs_apbm_ipmiapi_ns::sensor_readings sensor_readings;

	while(1)
	{
		printf("type the sensor index ( 37 --> sensor controlling first 32 connectors,  38 --> sensor controlling the following 32 connectors ): ");
		int call_result = scanf("%d%*c", & tmp_idx);
		if((call_result == 1) && ((tmp_idx == 37) || (tmp_idx == 38)))   break;
		printf("invalid input ! Call 'scanf' returned '%d; sensor_index == %d'\n", call_result, tmp_idx);
	}

	switch(tmp_idx)
	{
	case 37:
		sensor_index = acs_apbm_ipmiapi_ns::SENSOR_HANDLING_FIRST_32_GEA_CONNECTORS;
		break;
	case 38:
		sensor_index = acs_apbm_ipmiapi_ns::SENSOR_HANDLING_FOLLOWING_32_GEA_CONNECTORS;
		break;
	default:
		sensor_index = acs_apbm_ipmiapi_ns::SENSOR_INDEX_UNDEFINED;
	}

	call_result = test_api.get_ext_alarms(sensor_index, & sensor_readings);
	if (call_result == acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
	{
		for(int i = 0; i < acs_apbm_ipmiapi_ns::SENSOR_VALUES_SIZE; ++ i)
			printf("sensor_values[%d] == %u\n", i, sensor_readings.sensor_values[i]);
	}
	else
	  printf("Call 'get_ext_alarms() failed ! <retcode == %d>\n", call_result);
}


void Test_set_alarm_panel()
{
	int call_result;
	int panel_type = -1;
	int panel_values[7] = {0};
	acs_apbm_ipmiapi_ns::alarm_panel_values alarm_panel_values;

	while(1)
	{
		printf("type the 'panel type' ( 01h --> GEA Alarm panel,  02h --> SLE alarm): ");
		int call_result = scanf("%d%*c", & panel_type);
		if((call_result == 1) && (panel_type > 0) && (panel_type < 3))
		{
			alarm_panel_values.panel_type = panel_type;

			for(int i = 0; i < acs_apbm_ipmiapi_ns::PANEL_VALUES_SIZE; ++ i)
			{
				printf("enter 'panel_values[%d]': ", i);
				call_result = scanf("%d%*c", & panel_values[i]);
				if(call_result == 1)
					alarm_panel_values.panel_values[i] = panel_values[i];
				else
				{
					printf("invalid input: an error occurred reading 'panel_values[%d]. We set it to 0'", i);
					alarm_panel_values.panel_values[i] = 0;
				}
				printf("\n");
			}

			break;
		}
		printf("invalid input ! Call 'scanf' returned '%d; panel_type == %u'\n", call_result, panel_type);
	}

	call_result = test_api.set_alarm_panel(& alarm_panel_values);
	if (call_result != acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
		printf("Call 'set_alarm_panel() failed ! <retcode == %d>\n", call_result);
	else
		printf("set_alarm_panel() successfully executed \n");
}


void Test_presence_ack()
{
	int retcode = test_api.presence_ack();
	if (retcode != acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
		printf("Call 'presence_ack()' failed ! <retcode == %d>\n", retcode);
	else
		printf("presence_ack() successfully executed \n");
}


void Test_ipmi_restart()
{
	int retcode = test_api.ipmi_restart();
	if (retcode != acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
		printf("Call 'ipmi_restart()' failed ! <retcode == %d>\n", retcode);
	else
		printf("ipmi_restart() successfully executed \n");
}


void Test_get_board_address()
{
	int call_result;
	unsigned int board_address;

	call_result = test_api.get_board_address(& board_address);
	if (call_result == acs_apbm_ipmiapi_ns::IPMIAPI_NO_ERRORS)
		printf("board_address of this board is: '%02x'\n", board_address);
	else
		printf("Call 'get_board_address() failed ! <retcode == %d>\n", call_result);

}

void Test_close()
{
	int retcode = test_api.ipmiapi_finalize();
	printf("PIPAPI successfully finalized. Call 'ipmiapi_finalize()' returned with <retcode == %d>\n\n", retcode);
}

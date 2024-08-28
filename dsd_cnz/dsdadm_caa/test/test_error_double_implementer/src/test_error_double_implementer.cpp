#include <iostream>
#include <string>
#include "ACS_APGCC_RuntimeOwner.h"
#include "ACS_CC_Types.h"
#include "ACS_DSD_Server.h"
#include "ACS_DSD_Macros.h"

class Test_Error_Double_Implementer : public ACS_APGCC_RuntimeOwner {

public:
	inline Test_Error_Double_Implementer(){}
	inline ~Test_Error_Double_Implementer(){}

	/** Implementation of virtual method **/
	inline ACS_CC_ReturnType updateCallback(const char* /*p_objName*/, const char* /*p_attrName*/){ return (ACS_CC_ReturnType)0;}
};


int main()
{
	Test_Error_Double_Implementer test_obj;
	ACS_DSD_Server serv;
	ACS_DSD_Node my_node;
	int call_result = serv.get_local_node(my_node);
	if ( call_result )
	{
		std::cout << "Call 'get_local_node()' failed! call_result == " << call_result << std::endl;
		return -1;
	}

	char impl_name[100] = {0};
	snprintf(impl_name, 100, "DSD_SERVER_%s", my_node.node_name);

	std::cout << "Registering TEST APPLICATION to IMM with the implementer name " << impl_name << "!" << std::endl;
	ACS_CC_ReturnType res = test_obj.init(std::string(impl_name));

	if (res)
	{
		std::cout << "Error while registering TEST APPLICATION as OI... Error Code = " << res << " Error Descr = " << test_obj.getInternalLastErrorText() << std::endl;
		return -1;
	}

	std::cout << "Registered TEST APPLICATION to IMM with the implementer name" << impl_name << "!" << std::endl;
	std::cout << "Type ENTER to exit!" << std::endl;
	int c = getchar();

	test_obj.finalize();
	return 0;
}

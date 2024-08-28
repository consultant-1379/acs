include ../../cs_cxc/include.mk

GOOGLE_API = gtest
GTEST_DIR = $(GOOGLE_API)/gtest-1.6.0
GMOCK_DIR = $(GOOGLE_API)/gmock-1.6.0
GTEST_LIB = $(GMOCK_DIR)/lib/gmock_main.a

CINCLUDES += -I$(GTEST_DIR)
CINCLUDES += -I$(GTEST_DIR)/include

CINCLUDES += -I$(GMOCK_DIR)
CINCLUDES += -I$(GMOCK_DIR)/include

TEST_NAME = test
TEST_EXE = bin/$(TEST_NAME)

SOURCES_TEST = ACS_CS_ImDefaultObjectCreator.cpp ACS_CS_ImModelSaver_test_2.cpp ACS_CS_ImModelSaver_test.cpp \
	ACS_CS_ImUtils_TextualTest.cpp ACS_CS_ImUtils_test.cpp ACS_CS_ImRepository_test.cpp ACS_CS_ImModel_test.cpp  \
    ACS_CS_ServiceHandler_test.cpp ACS_CS_HWCHandler_test.cpp \
	ACS_CS_ImOtherBlade_test.cpp ACS_CS_ImCpBlade_test.cpp ACS_CS_ImApBlade_test.cpp \
	ACS_CS_ImConfigurationInfo_test.cpp ACS_CS_ImAp_test.cpp ACS_CS_ImAdvancedConfiguration_test.cpp ACS_CS_ImBladeClusterInfo_test.cpp \
	ACS_CS_Registry_test.cpp ACS_CS_ImSearchableBlade_test.cpp  \
	ACS_CS_ImValuesDefinitionType_test.cpp ACS_CS_ImCp_test.cpp ACS_CS_ImApCategory_test.cpp \
	ACS_CS_ImApService_test.cpp ACS_CS_ImApServiceCategory_test.cpp \
	ACS_CS_ImCpCategory_test.cpp ACS_CS_ImCpGroup_test.cpp ACS_CS_ImCpGroupCategory_test.cpp \
	ACS_CS_ImHardwareConfigurationCategory_test.cpp	ACS_CS_ImShelf_test.cpp ACS_CS_ImVlan_test.cpp \
	ACS_CS_ImVlanCategory_test.cpp \
	ACS_CS_CPIdHandler_test.cpp ACS_CS_CPGroupHandler_test.cpp ACS_CS_NEHandler_test.cpp \
	ACS_CS_Master_test.cpp  

OBJ_TEST = $(SOURCES_TEST:.cpp=.obj)
OBJFILES_TEST = $(addprefix $(OBJDIR)/,$(OBJ_TEST))

LIBSDIR += -L$(LOTC_SDK_LIB)64 -L$(LIBAPI_EXT_DIR) -L$(LIBAPI_INT_DIR)
LIBSDIR += -L$(AP_SDK_LIB)/ -L../../cs_cxc/ext_lib
LIBSDIR += -L$(BOOST_SDK_LIB)

CINCLUDES += -I$(IODEV_VOB)/BOOST_SDK/boost_1_44_0/include
CINCLUDES += -I$(IODEV_VOB)/ACE_SDK/ACE_wrappers/ace
CINCLUDES += -I../inc
CINCLUDES += -I../inc/imm_mapper
CINCLUDES += -I../csprot/inc
CINCLUDES += -I../../cscmd_caa/common/inc
CINCLUDES += -I../../csapi_caa/inc_int
CINCLUDES += -I../../csapi_caa/inc_ext

LIBS_TEST += -lrt -lpthread -ldhcpctl -lomapi -ldst -lnetsnmp -lprot -lacs_csapi -lacs_tra -lacs_apgcc 
LIBS_TEST += -lboost_serialization -lacs_csimm -lacs_trapdsapi

TMP_OBJ += ACS_CS_Event.obj ACS_CS_Registry.obj ACS_CS_HostFile.obj ACS_CS_TCPClient.obj \
	ACS_CS_TCPServer.obj ACS_CS_ServiceConnection.obj ACS_CS_EventReporter.obj \
	ACS_CS_NEHandler.obj ACS_CS_API_Util_Internal.obj \
	ACS_CS_Exception.obj ACS_CS_Error.obj ACS_CS_IOException.obj ACS_CS_ProtocolException.obj \
	ACS_CS_SystemException.obj ACS_CS_ParsingException.obj ACS_CS_VersionException.obj \
	ACS_CS_AttributeException.obj ACS_CS_TimeoutException.obj ACS_CS_Throwable.obj \
	ACS_CS_ReaderWriterLock.obj ACS_CS_TableSearch.obj ACS_CS_API_TableLoader.obj \
	ACS_CS_ServiceHandler.obj ACS_CS_HWCHandler.obj ACS_CS_Thread.obj ACS_CS_Attribute.obj \
	ACS_CS_IMMCallbackThread.obj ACS_CS_TableOperation.obj \
	ACS_CS_SubscriptionConnection.obj ACS_CS_TableHandler.obj ACS_CS_Table.obj ACS_CS_TableEntry.obj \
	ACS_CS_EntryCounter.obj ACS_CS_DHCP_Info.obj ACS_CS_Trace.obj ACS_CS_DHCP_Configurator.obj \
	ACS_CS_Internal_Table.obj ACS_CS_SNMPHandler.obj ACS_CS_Timer.obj ACS_CS_TableOperationWorker.obj \
	ACS_CS_CPIdHandler.obj ACS_CS_IMM_Manager.obj ACS_CS_SubscriptionAgent.obj \
	ACS_CS_HWC_Manager.obj ACS_CS_CPId_Manager.obj ACS_CS_FunctionDistribution_Manager.obj \
	ACS_CS_CPGroup_Manager.obj ACS_CS_Common_Util.obj ACS_CS_NE_Manager.obj ACS_CS_FuncDistHandler.obj \
	ACS_CS_CPGroupHandler.obj ACS_CS_VLANHandler.obj ACS_CS_OiHandler.obj ACS_CS_KeyValueString.obj \
	ACS_CS_Master.obj ACS_CS_Slave.obj ACS_CS_MODDHandler.obj ACS_CS_HAClass.obj ACS_CS_SNMPManager.obj \
	ACS_CS_PTBPeerHandler.obj ACS_CS_SNMPTrapReceiver.obj 

#OTHER_OBJ = ACS_CS_Shelf_Manager.obj ACS_CS_CP_Manager.obj ACS_CS_AP_Manager.obj \
	ACS_CS_CPGroup_Administration.obj 

#OTHER_OBJ_FILES =  $(addprefix ../../cscmd_caa/obj/,$(OTHER_OBJ))

LIBSTATIC_OBJ += $(TMP_OBJ)
LIBSTATIC_OBJ_FILES = $(addprefix ../obj/,$(LIBSTATIC_OBJ))

CFLAGS = -g -Wno-ignored-qualifiers

DEPS := $(SOURCES_TEST:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)
	-include $(DEPS)
endif

vpath %.cpp $(SRCDIR)
vpath %.cpp mocks\$(SRCDIR)
vpath %.obj $(OBJDIR)
		
######################################################################################
# Test binary
# Default target to build the binary of this block
######################################################################################	

.PHONY: $(TEST_NAME)
$(TEST_NAME): $(OBJ_TEST)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating exec: $(TEST_NAME)'
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) $(OBJFILES_TEST) $(OTHER_OBJ_FILES) $(LIBSTATIC_OBJ_FILES) $(LDFLAGS_TEST) $(LIBSDIR) $(LIBS_TEST) $(GTEST_LIB) -o $(TEST_EXE) 
	$(SEPARATOR_STR)
	$(NEW_LINE)	
						


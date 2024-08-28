include ../../../cs_cxc/include.mk

#CAA_NAME = csadm_caa
#CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)

#CFLAGS =  
#CFLAGS += -Wstrict-overflow=0 -Wno-ignored-qualifiers -Wno-empty-body
CCOVFLAGS += -fprofile-arcs -ftest-coverage

ifeq ($(COVERAGE),1)
#CFLAGS += -fprofile-arcs -ftest-coverage --coverage 
#CINCLUDES += -Icsprot/inc
GCOV_FLAGS = -DCP_PROXY_REQUIRED
LIBS += -lgcov
endif

CFLAGS += -DCP_PROXY_REQUIRED -Wno-ignored-qualifiers -Wno-empty-body -Wno-unused-parameter -Wno-unused-local-typedefs 
CFLAGS += 

CSSRV_APNAME = acs_csd

CSSRV_DEBUG = acs_csd_dbg

CSSRV_APBIN = $(OUTDIR)/$(CSSRV_APNAME)

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage
LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
CFLAGS += -I../csadm_caa/csprot/inc -I$(AP_SDK_INC)
GCOV_FLAGS =
LIBS += -lgcov

endif

LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB) -Wl,-rpath-link,$(COREMW_SDK_LIB) -Wl,-rpath-link,$(SEC_SDK_LIB) -Wl,-rpath-link,$(ACE_SDK_LIB) 
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB) -Wl,-rpath-link,$(BOOST_SDK_LIB) -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB) -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)

# here you can add own Include paths and/or other includes
CINCLUDES += -I ../../inc -I ../../csprot/inc -I ../../../csapi_caa/inc_ext -I ../../../csapi_caa/inc_int -I ../../../cscmd_caa/common/inc \
-I ../../common/inc -I ../../inc/imm_mapper

CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(LIB_XERCES_SDK_INC)
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(SEC_SDK_INC)
CINCLUDES += -I$(SEC_CREDU_SDK_INC)
CINCLUDES += -I$(LOTC_SDK_INC)
CINCLUDES += -I$(LOTC_SDK_DHCP_INC)

# here you can add own lib paths
LIBSDIR += -L../../../cs_cxc/bin/lib_int/ -L../../../cs_cxc/bin/lib_ext/
LIBSDIR += -L$(AP_SDK_LIB) 
LIBSDIR += -L$(LIB_XERCES_SDK_LIB)  
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB) 
#LIBSDIR += -L$(LIB_SSH2_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB) 
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB)
LIBSDIR += -L$(LOTC_SDK_DHCP_LIB)

LIBS_TEST += -lgcc_s -lstdc++ -lpthread -lm -lrt -ldhcpctl -lomapi -ldst -lirs -l:libdns.a -l:libisccfg.a -l:libisc.a -lACE -lnetsnmp -lxerces-c -lsec_crypto_api -lsec_credu_api
LIBS_TEST += -lacs_trapdsapi -lacs_apgcc -lacs_tra -lacs_aeh -lacs_nclib
LIBS_TEST += -lboost_serialization -lboost_regex -lboost_system -lboost_filesystem -lboost_thread 
LIBS_TEST += -lacs_csapi -lacs_csimm -lprot $(COREMW_LIBS)

#LIBS_TEST += -lrt -lpthread -ldhcpctl -lomapi -ldst -lprot -lacs_csapi -lacs_tra -lacs_apgcc
#LIBS_TEST += -lboost_serialization -lacs_csimm 

TEST_NAME = test
TEST_EXE = $(TEST_NAME)

SOURCES_TEST = ACS_CS_Protocol_Test.cpp
		
OBJ_TEST = $(SOURCES_TEST:.cpp=.obj)
OBJFILES_TEST = $(addprefix $(OBJDIR)/,$(OBJ_TEST))

 
#OBJFILES_IMM = ACS_CS_ImAdvancedConfiguration.obj ACS_CS_ImAp.obj ACS_CS_ImApBlade.obj ACS_CS_ImApCategory.obj ACS_CS_ImApService.obj \
				ACS_CS_ImApServiceCategory.obj ACS_CS_ImBase.obj ACS_CS_ImBlade.obj ACS_CS_ImBladeClusterInfo.obj ACS_CS_ImConfigurationInfo.obj \
				ACS_CS_ImCp.obj ACS_CS_ImCpBlade.obj ACS_CS_ImCpCategory.obj ACS_CS_ImCpGroup.obj ACS_CS_ImCpGroupCategory.obj \
				ACS_CS_ImHardwareConfigurationCategory.obj ACS_CS_ImImmObject.obj ACS_CS_ImIMMReader.obj ACS_CS_ImModel.obj ACS_CS_ImModelSaver.obj \
				ACS_CS_ImModelSubset.obj ACS_CS_ImObjectCreator.obj ACS_CS_ImOtherBlade.obj ACS_CS_ImRepository.obj ACS_CS_ImShelf.obj ACS_CS_ImUtils.obj \
				ACS_CS_ImValuesDefinitionType.obj ACS_CS_ImVlan.obj ACS_CS_ImVlanCategory.obj 

			     
OBJFILES_IMM_FILES = $(addprefix ../../csadm_caa/src/imm_mapper/obj/,$(OBJFILES_IMM))

VLAN ?= 0

#OBJFILES_IMM_TEST = ACS_CS_ImDefaultObjectCreator.obj
#OBJFILES_IMM_TEST_FILES = $(addprefix ../../csadm_caa/test/obj/,$(OBJFILES_IMM_TEST))
#
#OBJFILES_CSADMS = ACS_CS_SNMPTrapReceiver.obj
#OBJFILES_CSADM_FILES = $(addprefix ../../csadm_caa/src/obj/,$(OBJFILES_CSADM))

# OBJFILES = ACS_CS_API_HWC_Implementation_test.obj 
# OBJFILES = ACS_CS_API_NeHandling.obj
#OBJFILES = ACS_CS_API_CP_Implementation_test.obj
# OBJFILES = ACS_CS_API_CPGroup_Implementation_test.cpp

CFLAGS = -g -Wno-ignored-qualifiers

DEPS := $(SOURCES_TEST:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)
	-include $(DEPS)
endif

vpath %.cpp $(SRCDIR)
vpath %.obj $(OBJDIR)
		
######################################################################################
# Test binary
# Default target to build the binary of this block
######################################################################################	
.PHONY: vlan
vlan: $(TEST_NAME)

.PHONY: $(TEST_NAME)
$(TEST_NAME): $(OBJ_TEST)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating exec: $(TEST_NAME)'
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) $(OBJFILES_CSADM_FILES) $(OBJFILES_IMM_FILES) $(OBJFILES_IMM_TEST_FILES)  $(OBJFILES_TEST) $(LDFLAGS_TEST) $(LIBSDIR) $(LIBS_TEST)  -o $(TEST_EXE)
	$(SEPARATOR_STR)
	$(NEW_LINE)	
						


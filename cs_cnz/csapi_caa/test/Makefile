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

SOURCES_TEST = ACS_CS_API_ALL_test.cpp
		
OBJ_TEST = $(SOURCES_TEST:.cpp=.obj)
OBJFILES_TEST = $(addprefix $(OBJDIR)/,$(OBJ_TEST))

LIBAPI_EXT_DIR = ../../cs_cxc/bin/lib_ext
LIBAPI_INT_DIR = ../../cs_cxc/bin/lib_int

LIBSDIR += -L$(LOTC_SDK_LIB)64 -L$(LIBAPI_EXT_DIR) -L$(LIBAPI_INT_DIR)
LIBSDIR += -L$(AP_SDK_LIB)/ -L../../cs_cxc/ext_lib
LIBSDIR += -L$(BOOST_SDK_LIB)

CINCLUDES += -I$(IODEV_VOB)/BOOST_SDK/boost_1_44_0/include
CINCLUDES += -I../inc_int
CINCLUDES += -I../inc_ext
CINCLUDES += -I../../csadm_caa/inc -I../../csadm_caa/csprot/inc -I../../cscmd_caa/common/inc -I../../csadm_caa/inc/imm_mapper
CINCLUDES += -I../../csadm_caa/test

#LIBS_TEST += -lrt -lpthread -ldhcpctl -lomapi -ldst -lasnmp -lacs_csapi -lacs_tra -lacs_apgcc
#LIBS_TEST += -lstdc++ -lboost_serialization -lacs_csimm 

 
#OBJFILES_IMM = ACS_CS_ImAdvancedConfiguration.obj ACS_CS_ImAp.obj ACS_CS_ImApBlade.obj ACS_CS_ImApCategory.obj ACS_CS_ImApService.obj \
				ACS_CS_ImApServiceCategory.obj ACS_CS_ImBase.obj ACS_CS_ImBlade.obj ACS_CS_ImBladeClusterInfo.obj ACS_CS_ImConfigurationInfo.obj \
				ACS_CS_ImCp.obj ACS_CS_ImCpBlade.obj ACS_CS_ImCpCategory.obj ACS_CS_ImCpGroup.obj ACS_CS_ImCpGroupCategory.obj \
				ACS_CS_ImHardwareConfigurationCategory.obj ACS_CS_ImImmObject.obj ACS_CS_ImIMMReader.obj ACS_CS_ImModel.obj ACS_CS_ImModelSaver.obj \
				ACS_CS_ImModelSubset.obj ACS_CS_ImObjectCreator.obj ACS_CS_ImOtherBlade.obj ACS_CS_ImRepository.obj ACS_CS_ImShelf.obj ACS_CS_ImUtils.obj \
				ACS_CS_ImValuesDefinitionType.obj ACS_CS_ImVlan.obj ACS_CS_ImVlanCategory.obj 

			     
OBJFILES_IMM_FILES = $(addprefix ../../csadm_caa/src/imm_mapper/obj/,$(OBJFILES_IMM))

#OBJFILES_IMM_TEST = ACS_CS_ImDefaultObjectCreator.obj
#OBJFILES_IMM_TEST_FILES = $(addprefix ../../csadm_caa/test/obj/,$(OBJFILES_IMM_TEST))
#
#OBJFILES_CSADMS = ACS_CS_SNMPTrapReceiver.obj
#OBJFILES_CSADM_FILES = $(addprefix ../../csadm_caa/src/obj/,$(OBJFILES_CSADM))

LIBS_TEST += -lrt -lpthread -ldhcpctl -lomapi -ldst  -lacs_csapi -lacs_csimm -lacs_tra -lacs_apgcc
LIBS_TEST += -lstdc++ -lboost_serialization
# OBJFILES = ACS_CS_API_HWC_Implementation_test.obj 
# OBJFILES = ACS_CS_API_NeHandling.obj
OBJFILES = ACS_CS_API_CP_Implementation_test.obj
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

.PHONY: $(TEST_NAME)
$(TEST_NAME): $(OBJ_TEST)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating exec: $(TEST_NAME)'
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) $(OBJFILES_TEST) $(OBJFILES_CSADM_FILES) $(OBJFILES_IMM_FILES) $(OBJFILES_IMM_TEST_FILES) $(LDFLAGS_TEST) $(LIBSDIR) $(LIBS_TEST) $(GTEST_LIB) -o $(TEST_EXE) 
	$(SEPARATOR_STR)
	$(NEW_LINE)	
						


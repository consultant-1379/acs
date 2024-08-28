# **********************************************************************
#
# Short description:
# Makefile template for CS IMM Mapper library (intern)
# **********************************************************************
#
# Ericsson AB 2010 All rights reserved.
# The information in this document is the property of Ericsson.
# Except as specifically authorized in writing by Ericsson, the receiver of this
# document shall keep the information contained herein confidential and shall protect
# the same in whole or in part from disclosure and dissemination to third parties.
# Disclosure and disseminations to the receivers employees shall only be made
# on a strict need to know basis.
#
# **********************************************************************
#
# Rev        Date         Name      What
# -----      -------      --------  --------------------------
#            2011-11-02   xmaglex   Created
#            2011-12-06  xplodam   Changed into static lib
# **********************************************************************
 
include ../../../cs_cxc/include.mk

LIB_NAME = acs_csimm
LIB_NAME_STATIC = lib$(LIB_NAME).a

CURRDIR = $(PWD)

# here you can add own lib paths
LIBSDIR += -L$(LIBAPI_EXT_DIR) 
LIBSDIR += -L$(AP_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)

CFLAGS += -fPIC -fmessage-length=0
CFLAGS += -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-local-typedefs
CFLAGS += 

CCOVFLAGS += -fprofile-arcs -ftest-coverage

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage --coverage
CFLAGS += -I../csadm_caa/csprot/inc -I$(IODEV_VOB)/AP_SDK/Include
GCOV_FLAGS =
LIBS += -lgcov

endif

# here you can add own Include paths and/or other includes
CINCLUDES += -I ../../../csadm_caa/csprot/inc -I ../../../csadm_caa/inc -I ../../../cscmd_caa/common/inc \
-I../../../csadm_caa/common/inc -I../../inc/imm_mapper -I ../../../csapi_caa/inc_ext -I ../../../csapi_caa/inc_int
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)

LIBS += -lrt -lpthread -lacs_tra -lacs_apgcc -lacs_prc -lboost_serialization

SOURCES =	ACS_CS_ImAdvancedConfiguration.cpp ACS_CS_ImAp.cpp ACS_CS_ImApBlade.cpp ACS_CS_ImApService.cpp \
			ACS_CS_ImApServiceCategory.cpp ACS_CS_ImBase.cpp ACS_CS_ImBlade.cpp ACS_CS_ImClusterCp.cpp ACS_CS_ImEquipment.cpp \
			ACS_CS_ImCpCluster.cpp ACS_CS_ImDualSidedCp.cpp  ACS_CS_ImCpBlade.cpp \
			ACS_CS_ImImmObject.cpp ACS_CS_ImIMMReader.cpp ACS_CS_ImModel.cpp ACS_CS_ImModelSaver.cpp \
			ACS_CS_ImModelSubset.cpp ACS_CS_ImObjectCreator.cpp ACS_CS_ImOtherBlade.cpp ACS_CS_ImRepository.cpp ACS_CS_ImShelf.cpp ACS_CS_ImUtils.cpp \
			ACS_CS_ImValuesDefinitionType.cpp ACS_CS_ImVlan.cpp ACS_CS_ImVlanCategory.cpp ACS_CS_ImSearchableBlade.cpp \
			ACS_CS_ImEntryIdHelper.cpp ACS_CS_ImIPAssignmentHelper.cpp ACS_CS_ImHardwareMgmt.cpp ACS_CS_ImLogicalMgmt.cpp ACS_CS_ImCpClusterStruct.cpp \
			ACS_CS_ImOmProfile.cpp ACS_CS_ImCcFile.cpp ACS_CS_ImOmProfileManager.cpp ACS_CS_ImCcFileManager.cpp ACS_CS_ImCandidateCcFile.cpp \
			ACS_CS_ImOmProfileStruct.cpp ACS_CS_ImCandidateCcFileStruct.cpp	ACS_CS_ImFunctionDistribution.cpp ACS_CS_ImApg.cpp ACS_CS_ImFunction.cpp \
			ACS_CS_ImMoveFunctionProgress.cpp ACS_CS_ImCpProductInfo.cpp ACS_CS_ImCrMgmt.cpp ACS_CS_ImComputeResource.cpp ACS_CS_ImIplbCluster.cpp \
			ACS_CS_ImCrmEquipment.cpp ACS_CS_ImCrmComputeResource.cpp ACS_CS_ImComputeResourceNetwork.cpp \
			ACS_CS_ImTransport.cpp ACS_CS_ImHost.cpp ACS_CS_ImInterface.cpp ACS_CS_ImExternalNetwork.cpp ACS_CS_ImInternalNetwork.cpp		 
			
 	
OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

DEPS := $(SOURCES:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)
	-include $(DEPS)
endif

vpath %.cpp $(SRCDIR)
vpath %.h $(INCDIR)
vpath %.obj $(OBJDIR)
vpath %.cpp ../cscmd_caa/common/src
vpath %.cpp ../csadm_caa/csprot/src
vpath %.cpp ../csadm_caa/src
vpath %.cpp ../csadm_caa/common/src

#Create all
.PHONY: all
all: $(LIB_NAME_STATIC)

#Create library
.PHONY: $(LIB_NAME_STATIC)
$(LIB_NAME_STATIC): fs $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(LIB_NAME_STATIC)'
	$(AR) cr $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC) $(OBJFILES)
	cd $(CURRDIR)
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
# Perform prebuild actions like ClearCase checkouts
# Perform postbuild actions like ClearCase checkins
# Undo prebuild actions like ClearCase checkouts
# ClearCase checkouts	
.PHONY: checkout
checkout:	
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Checking out $(LIB_NAME) binary $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC))))' != 'CHECKEDOUT' ]; then cleartool co -nc $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC); else echo 'Binary is already checked out!'; fi
	$(NEW_LINE)

# ClearCase checkins
.PHONY: checkin
checkin:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Checking in $(LIB_NAME) binary $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC))))' = 'CHECKEDOUT' ]; then cleartool ci -nc $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC); else echo 'Binary is not checked out!'; fi
	$(NEW_LINE)
	
# Undo checkouts
.PHONY: uncheckout
uncheckout:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Undoing checkout of $(LIB_NAME) binary $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC))))' = 'CHECKEDOUT' ]; then cleartool uncheckout -rm $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC); else echo 'Binary is not checked out!'; fi
	$(NEW_LINE)
	
#Clean build files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning IMM API object files...'
	$(SILENT)$(RM) -f $(OBJDIR)/*
	$(SILENT)$(ECHO) 'Cleaning IMM API binary file: $(LIB_NAME_STATIC) and links...'
	$(SILENT)$(RM) -f $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC)

#Clean binary files
.PHONY: distclean
distclean: clean
	
# Perform chain of code analysis tools - add more?
.PHONY: metrics
metrics : cccc lint

# CCCC
# C and C++ Code Counter
.PHONY: cccc
cccc:
	$(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
	$(CCCC) $(wildcard $(INCDIR)/*) 
	$(CCCC) $(wildcard $(SRCDIR)/*) 

# LINT
# Splint is a tool for static code analysis
.PHONY: lint
lint:
	$(SILENT)$(ECHO) 'LINT file: $@'
	$(LINT) $(wildcard $(INCDIR)/*) 
	$(LINT) $(wildcard $(SRCDIR)/*) 
	
# Extract debug symbols
.PHONY: symbols
symbols:
	$(SILENT)$(ECHO) 'Extracting symbols from $(LIB_NAME_STATIC)'
#	$(SILENT)$(RM) $(LIBAPI_EXT_DIR)/$(LIB_FILENAME).dbg
#	$(SILENT)$(OBJCOPY) --only-keep-debug $(LIBAPI_EXT_DIR)/$(LIB_FILENAME) $(LIBAPI_EXT_DIR)/$(LIB_FILENAME).dbg
#	$(SILENT)$(OBJCOPY) --strip-debug $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)
#	$(SILENT)$(OBJCOPY) --add-gnu-debuglink=$(LIBAPI_EXT_DIR)/$(LIB_FILENAME) $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)
#	readelf -n $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)
#TODO: RENAME AND MOVE ACCORDING TO DR SS5
#	$(SILENT)$(MV) $(LIBAPI_EXT_DIR)/$(LIB_FILENAME).dbg $(CXCDIR)/debug/SymbolFiles/$(LIB_FILENAME).debug
	
#Create dependencies
.PHONY: depend
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^

#Create obj folder if not existing
.PHONY: fs
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

## DO NOT DELETE THIS LINE -- make depend needs it


# **********************************************************************
#
# Short description:
# Makefile template for CS API library
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
#            2010-08-27   xgalrob   Created
#            2010-09-08   xmikhal   Updated
#            2010-09-24   xpiokry   Updated
#            2010-10-01   xmikhal   Updated with CS common.mk
#            2010-10-18   xminaon   Moved definitions to common.mk
#            2010-11-10   xpiokry   Updated to follow DR
#            2011-01-12   xmaglex	Added targets: prebuild, postbuild and metrics 
#            2017-08-23   xmahima   Added new files for network table
#	         2017-12-04   xmahima   Incremented library version for network table changes
#            2017-12-07   xmahima   Added new file to support backward compatibility for network table changes
#            2018-04-27   xmahima   Fix for TR HW53952 ( coredump issue)
#            2018-05-16   xmahima   Changed a result code for IPLB
#	     2019-04012   xsansud  Fix for TR HX45316 (added ACS_CS_API_Set_R3.cpp)
# **********************************************************************
 

CNZ_NAME ?= cs_cnz
CAA_NAME ?= csapi_caa
CXC_NAME ?= cs_cxc

include ../cs_cxc/include.mk

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

#CAA_NAME = csapi_caa
#CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)

#LIB_NAME = ACS_CSAPI
#LIB_REL_MAJOR ?= 2
#LIB_REL_MINOR ?= 2

LIB_NAME = acs_csapi
LIB_REL_MAJOR ?= 3
LIB_REL_MINOR ?= 14
LIB_REL_BUILD ?= 2
LIB_VER ?= $(LIB_REL_MAJOR).$(LIB_REL_MINOR).$(LIB_REL_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VER)
LIB_FILENAME_DBG = $(LIB_FILENAME)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_REL_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

ACS_CSAPI_APLIB = $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)
ACS_CSAPI_APLIB_SONAME = $(LIBAPI_EXT_DIR)/$(LIB_SONAME)
ACS_CSAPI_APLIB_SO = $(LIBAPI_EXT_DIR)/$(LIB_LINKER_NAME)

CURRDIR = $(PWD)

# here you can add own lib paths
LIBSDIR += -L$(LIBAPI_EXT_DIR) -L$(LIBAPI_INT_DIR) -L$(AP_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)

CFLAGS += -fPIC -fmessage-length=0 -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-local-typedefs
CFLAGS += 
CCOVFLAGS += -fprofile-arcs -ftest-coverage

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage --coverage
CFLAGS += -I../csadm_caa/csprot/inc
GCOV_FLAGS =
LIBS += -lgcov

endif

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME) 

# here you can add own Include paths and/or other includes
CINCLUDES += -I inc_ext -I inc_int -I ../csadm_caa/csprot/inc -I ../csadm_caa/inc -I ../cscmd_caa/common/inc \
-I../csadm_caa/common/inc -I../csadm_caa/inc/imm_mapper
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

LIBS += -lrt -lpthread -lprot -lacs_tra -lacs_apgcc -lacs_csimm -lboost_serialization

SOURCES = ACS_CS_API_Name_Implementation.cpp ACS_CS_API_BoardSearch_R1.cpp ACS_CS_API_BoardSearch_Implementation.cpp \
		ACS_CS_API_TableLoader.cpp ACS_CS_API_IdList_R1.cpp ACS_CS_API_CPGroup_R1.cpp \
		ACS_CS_API_R2.cpp ACS_CS_API_R1.cpp ACS_CS_API_R3.cpp ACS_CS_API_CP_Implementation.cpp ACS_CS_API_SetClusterOpModeChanged.cpp \
		ACS_CS_API_Set_R1.cpp ACS_CS_API_Set_R2.cpp ACS_CS_API_Set_R3.cpp ACS_CS_API_NameList_R1.cpp \
		ACS_CS_API_SubscriptionMgr_R1.cpp ACS_CS_API_SubscriptionMgr_R2.cpp ACS_CS_API_CPGroup_Implementation.cpp \
		ACS_CS_API_SetOmProfileNotification.cpp ACS_CS_TCPServer.cpp \
		ACS_CS_API_ProfileHandling.cpp ACS_CS_Event.cpp ACS_CS_TCPClient.cpp \
		ACS_CS_API_HWC_Implementation.cpp ACS_CS_API_Name_R1.cpp ACS_CS_API_NeHandling.cpp \
		ACS_CS_API_NetworkElement_Implementation.cpp ACS_CS_API_CP_R1.cpp ACS_CS_API_Util_Internal.cpp \
		ACS_CS_API_FunctionDistribution_Implementation.cpp ACS_CS_API_SetQuorumData.cpp \
		ACS_CS_API_NameList_Implementation.cpp ACS_CS_API_FunctionDistribution_R1.cpp ACS_CS_API_HWC_R1.cpp \
		ACS_CS_API_SubscriptionMgr_Implementation.cpp ACS_CS_API_Set_Implementation.cpp \
		ACS_CS_API_IdList_Implementation.cpp ACS_CS_API_NetworkElement_R1.cpp ACS_CS_EventReporter.cpp \
		ACS_CS_TableEntry.cpp ACS_CS_Table.cpp ACS_CS_Attribute.cpp ACS_CS_TableSearch.cpp \
		ACS_CS_API_UsageLogger.cpp ACS_CS_HostFile.cpp ACS_CS_Registry.cpp \
		ACS_CS_ReaderWriterLock.cpp ACS_CS_API_Tracer.cpp \
		ACS_CS_API_NWT_R1.cpp ACS_CS_API_NWT_Implementation.cpp ACS_CS_API_NWT_BoardSearch_Implementation.cpp \
		ACS_CS_API_NWT_BoardSearch_R1.cpp ACS_CS_API_MacList_R1.cpp ACS_CS_API_MacList_Implementation.cpp

 	
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
all: $(LIB_FILENAME)

#Create library
.PHONY: $(LIB_FILENAME)
$(LIB_FILENAME): fs $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(LIB_FILENAME)'
#$(SILENT)$(RM) $(OBJDIR)/$(LIB_FILENAME)
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(ACS_CSAPI_APLIB) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	
	$(SILENT)$(ECHO) 'Symbols stripping requested'
	$(OBJCOPY) --only-keep-debug $(LIBAPI_EXT_DIR)/$(LIB_FILENAME) $(LIBAPI_EXT_DIR)/.debug/$(LIB_FILENAME_DBG)
	$(OBJCOPY) --strip-debug $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)
	$(OBJCOPY) --add-gnu-debuglink=$(LIBAPI_EXT_DIR)/.debug/$(LIB_FILENAME_DBG) $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)

	cd $(LIBAPI_EXT_DIR)
	ln -sf $(LIB_FILENAME) $(LIBAPI_EXT_DIR)/$(LIB_SONAME)
	ln -sf $(LIB_SONAME) $(LIBAPI_EXT_DIR)/$(LIB_LINKER_NAME)
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
	$(SILENT)$(ECHO) 'Checking out $(LIB_NAME) binary $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(LIBAPI_EXT_DIR)/$(LIB_FILENAME))))' != 'CHECKEDOUT' ]; then cleartool co -nc $(LIBAPI_EXT_DIR)/$(LIB_FILENAME); else echo 'Binary is already checked out!'; fi
	$(call ct_co,$(LIBAPI_EXT_DIR)/.debug/$(LIB_FILENAME_DBG))
	$(NEW_LINE)

# ClearCase checkins
.PHONY: checkin
checkin:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Checking in $(LIB_NAME) binary $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(LIBAPI_EXT_DIR)/$(LIB_FILENAME))))' = 'CHECKEDOUT' ]; then cleartool ci -nc $(LIBAPI_EXT_DIR)/$(LIB_FILENAME); else echo 'Binary is not checked out!'; fi
	$(call ct_ci,$(LIBAPI_EXT_DIR)/.debug/$(LIB_FILENAME_DBG))
	$(NEW_LINE)
	
# Undo checkouts
.PHONY: uncheckout
uncheckout:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Undoing checkout of $(LIB_NAME) binary $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(LIBAPI_EXT_DIR)/$(LIB_FILENAME))))' = 'CHECKEDOUT' ]; then cleartool uncheckout -rm $(LIBAPI_EXT_DIR)/$(LIB_FILENAME); else echo 'Binary is not checked out!'; fi
	$(call ct_unco,$(LIBAPI_EXT_DIR)/.debug/$(LIB_FILENAME_DBG))
	$(NEW_LINE)
	
#Clean build files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning API object files...'
	$(SILENT)$(RM) -f $(OBJDIR)/*

#Clean binary files
.PHONY: distclean
distclean: clean
	$(SILENT)$(ECHO) 'Cleaning API binary files: $(LIB_FILENAME) and links...'
	$(SILENT)$(RM) -f $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)
	$(SILENT)$(RM) -f $(LIBAPI_EXT_DIR)/$(LIB_SONAME)
	$(SILENT)$(RM) -f $(LIBAPI_EXT_DIR)/$(LIB_FILENAME)
	
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
	$(SILENT)$(ECHO) 'Extracting symbols from $(LIB_FILENAME)'
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


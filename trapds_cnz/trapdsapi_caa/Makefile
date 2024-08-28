# **********************************************************************
#
# Short description:
# Makefile template for TRAPDS API library
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
#            2016-03-30   xsunach   Created
#          
# **********************************************************************

CNZ_NAME ?= trapds_cnz
CAA_NAME ?= trapdsapi_caa
CXC_NAME ?= trapds_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

OUTDIR = $(CXCDIR)/bin/lib_ext

LIB_NAME = acs_trapdsapi
LIB_REL_MAJOR ?= 1
LIB_REL_MINOR ?= 1
LIB_REL_BUILD ?= 3
LIB_VER ?= $(LIB_REL_MAJOR).$(LIB_REL_MINOR).$(LIB_REL_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VER)
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_REL_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

ACS_TRAPDSAPI_APLIB = $(OUTDIR)/$(LIB_FILENAME)
ACS_TRAPDSAPI_APLIB_SONAME = $(OUTDIR)/$(LIB_SONAME)
ACS_TRAPDSAPI_APLIB_SO = $(OUTDIR)/$(LIB_LINKER_NAME)

#CURRDIR = $(PWD)

# here you can add own lib paths
LIBSDIR += -L$(LIBAPI_EXT_DIR) -L$(LIBAPI_INT_DIR) -L$(AP_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)


CFLAGS += -fPIC -fmessage-length=0 -Wno-ignored-qualifiers -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-local-typedefs
CCOVFLAGS += -fprofile-arcs -ftest-coverage

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage --coverage
#CFLAGS += -I../csadm_caa/csprot/inc
GCOV_FLAGS =
LIBS += -lgcov


endif

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME) 

# here you can add own Include paths and/or other includes
CINCLUDES += -I inc_ext 
CINCLUDES += -I inc_int 
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
LIBS += -lrt -lpthread -lboost_serialization -lacs_csapi -lacs_apgcc -lacs_tra  #-lprot -lacs_tra #-lacs_apgcc 


SOURCES =  ACS_TRAPDS_StructVariable.cpp ACS_TRAPDS_ServerConnection.cpp  ACS_TRAPDS_ServerConnectionImpl.cpp  ACS_TRAPDS_Sender.cpp ACS_TRAPDS_varlist.cpp ACS_TRAPDS_variableData.cpp ACS_TRAPDS_counter64a.cpp \
			ACS_TRAPDS_cl_oid.cpp ACS_TRAPDS_API_R1.cpp ACS_TRAPDS_SNMPTrapReceiver.cpp ACS_TRAPDS_Event.cpp ACS_TRAPDS_Util.cpp ACS_TRAPDS_API_R2.cpp   
		   
OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

DEPS := $(SOURCES:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)
	-include $(DEPS)
endif

vpath %.cpp $(SRCDIR)
vpath %.h $(INCDIR)
vpath %.obj $(OBJDIR)
#vpath %.cpp ../cscmd_caa/common/src
#vpath %.cpp ../csadm_caa/csprot/src
#vpath %.cpp ../csadm_caa/src
#vpath %.cpp ../csadm_caa/common/src


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
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(ACS_TRAPDSAPI_APLIB) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	#cd $(LIBAPI_EXT_DIR)
	ln -sf $(LIB_FILENAME) $(ACS_TRAPDSAPI_APLIB_SONAME)
	ln -sf $(LIB_SONAME) $(ACS_TRAPDSAPI_APLIB_SO)
	#cd $(CURRDIR)
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
#Clean build files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning API object files...'
	
	$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

#Clean binary files
.PHONY: distclean
distclean: clean
	$(SILENT)$(ECHO) 'Cleaning API binary files: $(LIB_FILENAME) and links...'	
	$(SILENT)$(RM) -f $(ACS_TRAPDSAPI_APLIB)
	$(SILENT)$(RM) -f $(ACS_TRAPDSAPI_APLIB_SONAME)
	$(SILENT)$(RM) -f $(ACS_TRAPDSAPI_APLIB)
	
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

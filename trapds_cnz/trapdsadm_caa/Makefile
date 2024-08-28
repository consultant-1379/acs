# **********************************************************************
#
# Short description:
# Makefile template for TRAPDS service binary
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
#           
# **********************************************************************
CNZ_NAME ?= trapds_cnz
CAA_NAME ?= trapdsadm_caa
CXC_NAME ?= trapds_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

include ../trapds_cxc/include.mk

CAA_NAME = trapdsadm_caa
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)

# here you can add own lib paths
LIBSDIR  +=

CFLAGS += -fPIC -fmessage-length=0 -Wstrict-overflow=0 -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-local-typedefs
CCOVFLAGS += -fprofile-arcs -ftest-coverage

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage --coverage
#CINCLUDES += -Icsprot/inc
GCOV_FLAGS =
LIBS += -lgcov
endif

LDFLAGS += -g
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB) -Wl,-rpath-link,$(ACE_SDK_LIB) -Wl,-rpath-link,$(COREMW_SDK_LIB) -Wl,-rpath-link,$(SEC_SDK_LIB) 
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB) -Wl,-rpath-link,$(BOOST_SDK_LIB) -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB) -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)

TRAPDSSRV_APNAME = acs_trapdsd

TRAPDSSRV_DEBUG = acs_trapdsd_dbg

TRAPDSSRV_APBIN = $(OUTDIR)/$(TRAPDSSRV_APNAME)

# here you can add own Include paths and/or other includes
#LIBSDIR_BEFORE += -L /vobs/cm4ap/ntacs/trapds_cnz/trapds_cxc/bin/lib_ext #-L "/vobs/cm4ap/ntacs/apgcc_cnz/apgcc_cxc/bin/lib_ext" 
CINCLUDES += -I inc 

CINCLUDES += -I ../trapdsapi_caa/inc_ext
CINCLUDES += -I ../trapdsapi_caa/inc_int
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
#CINCLUDES += -I$(LIB_XERCES_SDK_INC)
#CINCLUDES += -I$(LOTC_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)


#/Home/qlucdor/libs/usr/lib64

LIBSDIR += -L$(LIBAPI_EXT_DIR) -L$(LIBAPI_INT_DIR) #-L$(CXC_PATH)/ext_lib 
LIBSDIR += -L$(AP_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB)

LIBS += -lrt -lpthread -lACE -lacs_trapdsapi -lacs_apgcc -lacs_tra -lacs_aeh -lnetsnmp 
#-ldhcpctl -lomapi -ldst -lacs_tra -lxerces-c -licuuc -licudata -lboost_serialization  

SOURCES += ACS_TRAPDS_SRV.cpp ACS_TRAPDS_HA_AppManager.cpp ACS_TRAPDS_Server.cpp ACS_TRAPDS_Event.cpp ACS_TRAPDS_Util.cpp ACS_TRAPDS_Imm_Util.cpp \
		   ACS_TRAPDS_ImmHandler.cpp ACS_TRAPDS_OI_TrapSubscriber.cpp ACS_TRAPDS_OI_TrapService.cpp #ACS_TRAPDS_ServerConnectionImpl.cpp ACS_TRAPDS_ServerConnection.cpp ACS_TRAPDS_SNMPTrapReceiver.cpp 
		
	
OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

DEPS := $(SOURCES:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)
	-include $(DEPS)
endif

vpath %.cpp $(SRCDIR) ../trapdsapi_caa/src ../trapdscmd_caa/common/src common/src
vpath %.h $(INCDIR) trapdsprot/inc ../trapdsapi_caa/inc_int ../trapdsapi_caa/inc_ext
vpath %.obj $(OBJDIR)
#temporary added below
#vpath %.cpp trapdsprot/src

#Create all
.PHONY: all
all: $(TRAPDSSRV_APNAME)

#Create binary
.PHONY: $(TRAPDSSRV_APNAME)
$(TRAPDSSRV_APNAME): fs $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating executable : $(TRAPDSSRV_APNAME)'
#   $(SILENT)$(RM) -f $(TRAPDSSRV_APBIN)
#	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(TRAPDSSRV_APBIN) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(CC) $(CFLAGS) $(OBJFILES) $(LDFLAGS) $(LIBSDIR_BEFORE) $(LIBSDIR) $(LIBS) -o $(TRAPDSSRV_APBIN)
	$(call stripp,$(TRAPDSSRV_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
#Clean build files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning SRV build files...'
	$(SILENT)$(RM) -f $(OBJDIR)/*

#Clean binary files
.PHONY: distclean
distclean: clean
	$(SILENT)$(ECHO) 'Cleaning SRV binary files: $(TRAPDSSRV_APNAME)'
	$(SILENT)$(RM) -f $(OUTDIR)/$(TRAPDSSRV_APNAME)

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
	$(SILENT)$(ECHO) 'Extracting symbols from $(TRAPDSSRV_APNAME)'
	$(SILENT)$(RM) $(OUTDIR)/$(TRAPDSSRV_APNAME).dbg
	$(SILENT)$(OBJCOPY) --only-keep-debug $(OUTDIR)/$(TRAPDSSRV_APNAME) $(OUTDIR)/$(TRAPDSSRV_APNAME).dbg
	$(SILENT)$(OBJCOPY) --strip-debug $(OUTDIR)/$(TRAPDSSRV_APNAME)
	$(SILENT)$(OBJCOPY) --add-gnu-debuglink=$(OUTDIR)/$(TRAPDSSRV_APNAME).dbg $(OUTDIR)/$(TRAPDSSRV_APNAME)
	readelf -n $(OUTDIR)/$(TRAPDSSRV_APNAME)
#TODO: RENAME AND MOVE ACCORDING TO DR SS5
	$(SILENT)$(MV) $(OUTDIR)/$(TRAPDSSRV_APNAME).dbg $(CXCDIR)/debug/SymbolFiles/$(TRAPDSSRV_APNAME).debug
	
		
#Create dependencies
.PHONY: depend
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^

#Create obj folder if not existing
.PHONY: fs
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

## DO NOT DELETE THIS LINE -- make depend needs it

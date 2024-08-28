# **********************************************************************
#
# Short description:
# Makefile template for CS service binary
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
#            2010-09-08   x         Updated
#            2010-10-01   xmikhal   Updated with CS common.mk
#            2010-10-18   xminaon   Updated for new ACS_CS_ServiceHandler
#            2010-11-10   xpiokry   Updated to follow DR
#            2011-01-12   xmaglex	Added targets: prebuild, postbuild and metrics 
#	     2014-04-23	  xcsrpad    updated for BRFPersistentDataOwner
# **********************************************************************

CNZ_NAME ?= cs_cnz
CAA_NAME ?= csadm_caa
CXC_NAME ?= cs_cxc

include ../cs_cxc/include.mk

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

#CAA_NAME = csadm_caa
#CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
NTACS  = $(ACS_ROOT)
CXCDIR = $(NTACS)/cs_cnz/cs_cxc
CXCBIN = bin
OUTDIR = $(CXCDIR)/$(CXCBIN)
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
CINCLUDES += -I inc -I csprot/inc -I ../csapi_caa/inc_ext -I ../csapi_caa/inc_int -I ../cscmd_caa/common/inc \
-I common/inc -I inc/imm_mapper

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
LIBSDIR += -L$(LIBAPI_EXT_DIR) -L$(LIBAPI_INT_DIR)
LIBSDIR += -L$(AP_SDK_LIB) 
LIBSDIR += -L$(LIB_XERCES_SDK_LIB)  
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB) 
#LIBSDIR += -L$(LIB_SSH2_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB) 
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB)
LIBSDIR += -L$(LOTC_SDK_DHCP_LIB)
LIBSDIR += -L"$(ACS_ROOT)/cs_cnz/cs_cxc/bin/lib_int"

LIBS += -lm -lrt -lpthread -ldhcpctl -lomapi -lirs -l:libdns.a -l:libisccfg.a -l:libisc.a -lACE -lnetsnmp -lxerces-c -lsec_crypto_api -lsec_credu_api
LIBS += -lacs_trapdsapi -lacs_apgcc -lacs_tra -lacs_aeh -lacs_nclib -lacs_prc
LIBS += -lboost_serialization -lboost_regex -lboost_system -lboost_filesystem -lboost_thread 
LIBS += -lacs_csapi -lacs_csimm -lprot $(COREMW_LIBS)

SOURCES += ACS_CS_BrfSubscriber.cpp ACS_CS_Event.cpp ACS_CS_SRV.cpp ACS_CS_Registry.cpp ACS_CS_HostFile.cpp ACS_CS_TCPClient.cpp \
	ACS_CS_TCPServer.cpp ACS_CS_ServiceConnection.cpp ACS_CS_EventReporter.cpp \
	ACS_CS_NEHandler.cpp ACS_CS_API_Util_Internal.cpp  \
	ACS_CS_Exception.cpp ACS_CS_Error.cpp ACS_CS_IOException.cpp ACS_CS_ProtocolException.cpp \
	ACS_CS_SystemException.cpp ACS_CS_ParsingException.cpp ACS_CS_VersionException.cpp \
	ACS_CS_AttributeException.cpp ACS_CS_TimeoutException.cpp ACS_CS_Throwable.cpp \
	ACS_CS_ReaderWriterLock.cpp ACS_CS_TableSearch.cpp ACS_CS_API_TableLoader.cpp \
	ACS_CS_ServiceHandler.cpp ACS_CS_HWCHandler.cpp ACS_CS_Thread.cpp ACS_CS_Attribute.cpp \
	ACS_CS_IMMCallbackThread.cpp ACS_CS_TableOperation.cpp \
	ACS_CS_SubscriptionConnection.cpp ACS_CS_TableHandler.cpp ACS_CS_Table.cpp ACS_CS_TableEntry.cpp \
	ACS_CS_EntryCounter.cpp ACS_CS_DHCP_Info.cpp ACS_CS_Trace.cpp ACS_CS_DHCP_Configurator.cpp ACS_CS_TrapHandler.cpp  ACS_CS_SNMPTrapReceiver.cpp  \
	ACS_CS_SNMPHandler.cpp ACS_CS_Timer.cpp ACS_CS_TableOperationWorker.cpp \
	ACS_CS_CPIdHandler.cpp ACS_CS_SubscriptionAgent.cpp \
	ACS_CS_Common_Util.cpp ACS_CS_FuncDistHandler.cpp \
	ACS_CS_NetConfBuilderCommand.cpp ACS_CS_NetConfCommand.cpp ACS_CS_NetConfRemoteSender.cpp ACS_CS_NetConfMacParser.cpp ACS_CS_NetConfBladeFilter.cpp \
	ACS_CS_CPGroupHandler.cpp ACS_CS_VLANHandler.cpp ACS_CS_KeyValueString.cpp \
	ACS_CS_Master.cpp ACS_CS_Slave.cpp ACS_CS_MODDHandler.cpp ACS_CS_HAClass.cpp  ACS_CS_SNMPManager.cpp \
	ACS_CS_PTBPeerHandler.cpp ACS_CS_MethodHandler.cpp ACS_CS_BladeHandler.cpp	ACS_CS_LogicalMgmtHandler.cpp ACS_CS_CpClusterHandler.cpp ACS_CS_ApHandler.cpp ACS_CS_CpHandler.cpp \
	ACS_CS_SNMPMasterHandler.cpp ACS_CS_ShelfManager.cpp ACS_CS_IronsideHandler.cpp \
	ACS_CS_SecApi.cpp ACS_CS_SECCallbackThread.cpp \
	ACS_CS_CpClusterManager.cpp ACS_CS_OmProfileHandler.cpp ACS_CS_CcFileHandler.cpp ACS_CS_OmProfileManager.cpp ACS_CS_CandidateCcFile.cpp ACS_CS_LdapProxyHandler.cpp\
	ACS_CS_BrfPersistentDataOwner.cpp ACS_CS_BrfHandler.cpp ACS_CS_ComputeResourceHandler.cpp ACS_CS_RoleAssignment.cpp \
	ACS_CS_GLKHandler.cpp ACS_CS_TFTP_Configurator.cpp ACS_CS_Util.cpp\
	ACS_CS_FTPAccessHandler.cpp	ACS_CS_CRRProxyHandler.cpp \
	ACS_CS_NetworkList.cpp ACS_CS_NetworkHandler.cpp\
	ACS_CS_VirtualVLANHandler.cpp ACS_CS_NativeVLANHandler.cpp ACS_CS_TransportHandler.cpp
	
	 #ACS_CS_ApHandler.cpp  ACS_CS_CpClusterHandler.cpp \
	#ACS_CS_LogicalMgmtHandler.cpp  ACS_CS_CpHandler.cpp 
	
OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

DEPS := $(SOURCES:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)

	-include $(DEPS)
endif

vpath %.cpp $(SRCDIR) $(SRCDIR)/imm_mapper ../csapi_caa/src ../cscmd_caa/common/src common/src
vpath %.h $(INCDIR) csprot/inc ../csapi_caa/inc_int ../csapi_caa/inc_ext
vpath %.obj $(OBJDIR)
#temporary added below
#vpath %.cpp csprot/src

#Create all
.PHONY: all
all: $(CSSRV_APNAME) #$(SCSRV_IMMNAME)
	
#Create binary
.PHONY: $(CSSRV_APNAME)
$(CSSRV_APNAME): fs $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating executable : $(CSSRV_APNAME)'
#   $(SILENT)$(RM) -f $(CSSRV_APBIN)
#	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(CSSRV_APBIN) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(CC) $(OBJFILES) $(CFLAGS) $(LDFLAGS) $(LIBSDIR) $(LIBS) -o $(CSSRV_APBIN)
	$(call stripp,$(CSSRV_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)	
	
#Clean build files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning SRV object files...'
	$(SILENT)$(RM) -f $(OBJDIR)/*

#Clean binary files
.PHONY: distclean
distclean: clean
	$(SILENT)$(ECHO) 'Cleaning SRV binary files: $(CSSRV_APNAME)'
	$(SILENT)$(RM) -f $(OUTDIR)/$(CSSRV_APNAME)

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
	$(SILENT)$(ECHO) 'Extracting symbols from $(CSSRV_APNAME)'
	$(SILENT)$(RM) $(OUTDIR)/$(CSSRV_APNAME).dbg
	$(SILENT)$(OBJCOPY) --only-keep-debug $(OUTDIR)/$(CSSRV_APNAME) $(OUTDIR)/$(CSSRV_APNAME).dbg
	$(SILENT)$(OBJCOPY) --strip-debug $(OUTDIR)/$(CSSRV_APNAME)
	$(SILENT)$(OBJCOPY) --add-gnu-debuglink=$(OUTDIR)/$(CSSRV_APNAME).dbg $(OUTDIR)/$(CSSRV_APNAME)
	readelf -n $(OUTDIR)/$(CSSRV_APNAME)
#TODO: RENAME AND MOVE ACCORDING TO DR SS5
	$(SILENT)$(MV) $(OUTDIR)/$(CSSRV_APNAME).dbg $(CXCDIR)/debug/SymbolFiles/$(CSSRV_APNAME).debug
	
		
#Create dependencies
.PHONY: depend
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^

#Create obj folder if not existing
.PHONY: fs
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

## DO NOT DELETE THIS LINE -- make depend needs it


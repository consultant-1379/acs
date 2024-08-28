# **********************************************************************
#
# Short description:
# Makefile template for XXX library
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
#            2010-11-23   tcsvenc   Created
#            2014-04-25   xquydao   Update to build with ping simluation version
#                                                            
# **********************************************************************

CNZ_NAME ?= nsf_cnz
CAA_NAME ?= nsfadm_caa
CXC_NAME ?= nsf_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

ifeq ($(CXC_PATH),)
	CXC_PATH = $(CURDIR)/../nsf_cxc
endif

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INCDIR   = $(CURDIR)/inc
TESTDIR  = $(CURDIR)/test
NTACS = $(ACS_ROOT)

#export DX_CUSTOM_RPATH=${COREMW_SDK_LIB}:${COREMW_SDK_LIB}/opensaf:${AP_SDK_LIB}:$(ACE_SDK_PATH)/ace:$(ACS_ROOT)/tra_cnz/tra_cxc/bin

# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DNSFDEBUG -g
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE 

ifeq ($(PING_SIM),1)
	CFLAGS += -D_NSF_PING_SIMULATION
endif

#-DNONODESTATEPRC 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR)  
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

#removed by me
#LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64
#LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB) -Wl,-rpath-link,$(COREMW_SDK_LIB) -Wl,-rpath-link,$(SEC_SDK_LIB) -Wl,-rpath-link,$(ACE_SDK_LIB) 
#LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB) -Wl,-rpath-link,$(BOOST_SDK_LIB) -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB) -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(COREMW_SDK_LIB) -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)
 

# here you can add own lib paths
#removed by me
#LIBSDIR_ACS_NSF_SERVER = -L$(AP_SDK_LIB)  -L$(LIB_LOGCPLUS_SDK_LIB) -L$(ACE_SDK_LIB) -L$(COREMW_SDK_LIB) -L$(BOOST_SDK_LIB)

#added by me
LIBSDIR_ACS_NSF_SERVER += -L"$(AP_SDK_LIB)"
LIBSDIR_ACS_NSF_SERVER += -L"$(BOOST_SDK_LIB)"
LIBSDIR_ACS_NSF_SERVER += -L"$(ACE_SDK_LIB)"
LIBSDIR_ACS_NSF_SERVER += -L"$(COREMW_SDK_LIB)"
LIBSDIR_ACS_NSF_SERVER += -L"$(SEC_SDK_LIB)"
LIBSDIR_ACS_NSF_SERVER += -L"$(LIB_LOGCPLUS_SDK_LIB)"
LIBSDIR_ACS_NSF_SERVER += -L"$(LOTC_SDK_LIB)"

#removed by me
#LIBSDIR_ACS_NSF_BIOSTIMERECOVERY += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)
#added by me
LIBSDIR_ACS_NSF_BIOSTIMERECOVERY += -L"$(AP_SDK_LIB)"
LIBSDIR_ACS_NSF_BIOSTIMERECOVERY += -L"$(LIB_LOGCPLUS_SDK_LIB)"
LIBSDIR_ACS_NSF_BIOSTIMERECOVERY += -L"$(ACE_SDK_LIB)"

## # here you can add own libs 
#removed by me
#LIBS_ACS_NSF_SERVER = -lacs_apgcc -lacs_tra -llog4cplus -lacs_prc -lacs_aeh -lACE -lpthread -lSaImmOm -lSaImmOi -lSaAmf -lSaClm -lopensaf_core -lboost_serialization -lboost_filesystem -lboost_system
#added by me
LIBS_ACS_NSF_SERVER += $(LIBS) -lacs_apgcc -lacs_tra -lacs_dsd -lacs_prc -lacs_aeh -lacs_csapi -lpthread -llog4cplus -lACE -lboost_serialization -lboost_filesystem -lboost_system -lSaImmOm -lSaImmOi -lSaAmf -lSaClm  -lsec_crypto_api

LIBS_ACS_NSF_BIOSTIMERECOVERY += $(LIBS) -lACE

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) 

## Source files 

ACS_NSF_SERVER_SRCFILES = acs_nsf_server.cpp acs_nsf_common.cpp acs_nsf_aeh.cpp acs_nsf_command_handler.cpp acs_nsf_service.cpp

ACS_NSF_BIOSTIMERECOVERY_SRCFILES = acs_nsf_biostime_recovery.cpp

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#

ACS_NSF_SERVER_OBJ = \
	 	$(ACS_NSF_SERVER_SRCFILES:%.cpp=%.obj) \
                $(SRCFILES:%.c=%.obj)

ACS_NSF_BIOSTIMERECOVERY_OBJ = \
 		$(ACS_NSF_BIOSTIMERECOVERY_SRCFILES:%.cpp=%.obj) \
                $(SRCFILES:%.c=%.obj)

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC

ACS_NSF_SERVER = acs_nsfserverd
ACS_NSF_BIOSTIMERECOVERY =  acs_nsfbiostimerecovery
ACS_NSF_SERVER_DBG = acs_nsfserverd_dbg
ACS_NSF_BIOSTIMERECOVERY_DBG =  acs_nsfbiostimerecovery_dbg

ACS_NSF_SERVER_EXE = $(OUTDIR)/$(ACS_NSF_SERVER)
ACS_NSF_BIOSTIMERECOVERY_EXE = $(OUTDIR)/$(ACS_NSF_BIOSTIMERECOVERY)

ACS_NSF_SERVER_OBJFILES = $(OBJDIR)/acs_nsf_server.obj $(OBJDIR)/acs_nsf_common.obj $(OBJDIR)/acs_nsf_aeh.obj \
$(OBJDIR)/acs_nsf_command_handler.obj $(OBJDIR)/acs_nsf_service.obj

ACS_NSF_BIOSTIMERECOVERY_OBJFILES = $(OBJDIR)/acs_nsf_biostime_recovery.obj

DEPS := $(ACS_NSF_SERVER_OBJFILES:.obj=.d) \
        $(ACS_NSF_BIOSTIMERECOVERY_OBJFILES:.obj=.d) 

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all

all: $(ACS_NSF_SERVER_EXE) $(ACS_NSF_BIOSTIMERECOVERY_EXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_NSF_SERVER_EXE) $(ACS_NSF_BIOSTIMERECOVERY_EXE)'
		$(SILENT)$(RM) $(ACS_NSF_SERVER_EXE) $(ACS_NSF_BIOSTIMERECOVERY_EXE)
		$(SILENT)$(RM) $(ACS_NSF_SERVER_EXE_DBG) $(ACS_NSF_BIOSTIMERECOVERY_EXE_DBG)

$(ACS_NSF_SERVER_EXE): $(ACS_NSF_SERVER_OBJ) $(METRICS)
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_NSF_SERVER_EXE) $(ACS_NSF_SERVER_OBJFILES) $(LIBSDIR_ACS_NSF_SERVER) $(LIBS_ACS_NSF_SERVER) $(LDFLAGS)
	    $(call stripp,$(ACS_NSF_SERVER))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(ACS_NSF_BIOSTIMERECOVERY_EXE): $(ACS_NSF_BIOSTIMERECOVERY_OBJ) $(METRICS)
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_NSF_BIOSTIMERECOVERY_EXE) $(ACS_NSF_BIOSTIMERECOVERY_OBJFILES) $(LIBSDIR_ACS_NSF_BIOSTIMERECOVERY) $(LIBS_ACS_NSF_BIOSTIMERECOVERY) $(LDFLAGS)
	    $(call stripp,$(ACS_NSF_BIOSTIMERECOVERY))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)


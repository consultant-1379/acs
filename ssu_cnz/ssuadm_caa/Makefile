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
#            2010-02-19   teiggam   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= ssu_cnz
CAA_NAME ?= ssuadm_caa
CXC_NAME ?= ssu_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

BLOCKDIR = $(CAA_PATH)/../ssuadm_caa/
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
INCCMDDIR= $(CURDIR)/../ssucmd_caa/inc
TESTDIR  = $(BLOCKDIR)/test

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DSSUIPTTEST -DSSUDEBUG -DSSUNODEFAILURE -g 
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE 
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES +=  -I$(INCDIR) -I$(INCCMDDIR) \
-I$(ACE_SDK_PATH) \
-I$(COREMW_SDK_INC) \
-I$(AP_SDK_INC) \
-I$(BOOST_SDK_INC) \

## # here you can add own Include paths and/or other includes
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 

LIBSDIR +=  -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) \
-L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB)
LIBSDIR += -L$(LOTC_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB) \
-L$(SEC_ACS_SDK_LIB)

## # here you can add own libs 

LIBS += -lacs_apgcc -lacs_tra -llog4cplus -lacs_prc -lacs_csapi -lacs_aeh \
-lACE -lldap -lboost_serialization -lboost_filesystem -lboost_system -lboost_thread -lpthread -lacs_dsd \
$(COREMW_LIBS) \
-lsec_crypto_api -lsec_credu_api \
$(SEC_LIBS) \
#-lopensaf_core
  
## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) $(CURDIR)/../ssucmd_caa/src

## Source files
#
SRCFILES = acs_ssu_disk_mon_objectimpl.cpp acs_ssu_perf_mon_objectimpl.cpp acs_ssu_monitor.cpp acs_ssu_svc_loader.cpp acs_ssu_disk_mon.cpp acs_ssu_aeh.cpp acs_ssu_path.cpp\
acs_ssu_alarm_handler.cpp acs_ssu_perf_mon.cpp acs_ssu_memory_counter.cpp acs_ssu_counter.cpp acs_ssu_file_handle_counter.cpp \
 acs_ssu_folder_quota_mon.cpp acs_ssu_folder_quota.cpp acs_ssu_execute.cpp acs_ssu_service.cpp 
#acs_ssu_command_handler.cpp acs_ssu_runtimeowner.cpp
IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

## Build instructions
#
APNAME = acs_ssumonitord
APNAME_DBG = acs_ssumonitord_dbg

APEXE = $(OUTDIR)/$(APNAME)

.PHONY: all
all: $(APEXE)

.PHONY: clean
clean:	
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT) rm -f  $(wildcard $(OBJDIR)/*)

.PHONY: distclean
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(APNAME)
		$(RM) $(OUTDIR)/.debug/$(APNAME_DBG)

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*.obj) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: metrics
metrics : cccc lint

.PHONY: cccc
cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
#		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

				
	
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


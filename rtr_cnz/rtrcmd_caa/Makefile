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
#            2010-12-02   tcskrpr   Created
#                                                            
# **********************************************************************

#CAA_NR = 

#include $(COMMON_MK)
CNZ_NAME ?= rtr_cnz
CAA_NAME ?= rtrcmd_caa
CXC_NAME ?= rtr_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
SRCAPIDIR = $(BLOCKDIR)/../rtrapi_caa/src
INCDIR   = $(BLOCKDIR)/inc
RTRAPIINCDIR = $(BLOCKDIR)/../rtrapi_caa/inc_int
ADMINCDIR = $(ACS_ROOT)/rtr_cnz/rtradm_caa/inc
TESTDIR  = $(BLOCKDIR)/test
NTACS = $(ACS_ROOT)
#SECINCDIR = /vobs/IO_Developments/SEC_SDK/inc/sec

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR) -I$(ADMINCDIR)  -I$(RTRAPIINCDIR) -I$(ACS_ROOT)/rtr_cnz/rtrcmd_caa/inc/ -I$(ACE_SDK_INC)

## Object files

# here you can add own lib paths
#LIBSDIR  += -L $(ACS_ROOT)/aca_cnz/aca_cxc/bin/lib_ext -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AP_SDK_LIB) -L$(ACS_ROOT)/rtr_cnz/rtr_cxc/bin/lib_int -L/vobs/IO_Developments/SEC_SDK/lib 

LIBSDIR += -L$(AP_SDK_LIB) -L$(ACS_ROOT)/rtr_cnz/rtr_cxc/bin/lib_int
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB)
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB)

# here you can add own compiler flags
# trace enabled
EXTRA_DEFINES += -DACS_RTR_API_HAS_TRACE
EXTRA_DEFINES += -DACS_RTR_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DACS_RTR_TRACE_HAS_FUNCTION_TRACE
EXTRA_DEFINES += -DACS_RTR_TRACE_USE_PRETTY_FUNCTION
# log enabled
EXTRA_DEFINES += -DACS_RTR_HAS_LOGGING
EXTRA_DEFINES += -DACS_RTR_LOGGING_HAS_SOURCE_INFO
EXTRA_DEFINES += -DACS_RTR_LOGGING_USE_PRETTY_FUNCTION

CFLAGS += $(EXTRA_DEFINES)
CFLAGS += $(CFLAGS_EXTRA)

# -DEMFONSIMAP -DEMFDEBUG
#CFLAGS += -O3 -fPIC -g -fmessage-length=0

# here you can add own Assembler flags
ASMFLAGS += 

#LDFLAGS += -s -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 
#LIBS +=  -lacs_apgcc -lacs_tra -llog4cplus -lacs_prc -lacs_dsd
LIBS += -lrt -lpthread

#ACS_RTR_LIBS = $(LIBS) -lacs_tra -lacs_apgcc -lacs_csapi -laes_ohi -lacs_rtr -lsec_crypto_api

ACS_RTR_LIBS = $(LIBS) -lacs_rtr -lsec_crypto_api -lACE -lacs_apgcc -laes_ohi -laes_dbo -laes_afp -lacs_dsd -lacs_aeh -lacs_tra -lacs_csapi -lacs_prc -lboost_serialization -lboost_filesystem -lboost_system -lboost_thread -lSaImmOm -lSaImmOi -lSaAmf -lSaClm -llog4cplus -lSaNtf -laes_gcc

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) $(SRCAPIDIR)

## Source files
#

#SRCFILES = acs_emf_emfcopy.cpp acs_emf_emfinfo.cpp acs_emf_getopt.c acs_emf_execute.cpp acs_emf_tra.cpp acs_emf_common.cpp 

SRCFILES = acs_rtr_cmd_util.cpp rtrls.cpp rtrdef.cpp  rtrrm.cpp rtrfe.cpp rtrch.cpp
IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 
 

## Object files
#
OBJS =	\
	$(SRCFILES:%.cpp=%.obj) 
	
## Build instructions

RTRDEF = rtrdef
RTRDEF_DBG = rtrdef_dbg

RTRLS = rtrls
RTRLS_DBG = rtrls_dbg

RTRRM = rtrrm
RTRRM_DBG = rtrrm_dbg

RTRFE = rtrfe
RTRFE_DBG = rtrfe_dbg

RTRCH = rtrch
RTRCH_DBG = rtrch_dbg

RTRDEF_EXE = $(OUTDIR)/$(RTRDEF)

RTRLS_EXE = $(OUTDIR)/$(RTRLS)

RTRRM_EXE = $(OUTDIR)/$(RTRRM)

RTRFE_EXE = $(OUTDIR)/$(RTRFE)

RTRCH_EXE = $(OUTDIR)/$(RTRCH)

#DEPS := $(RTRDEF_OBJFILES:.obj=.d)
#DEPS := $(RTRLS_OBJFILES:.obj=.d)
#DEPS := $(RTRRM_OBJFILES:.obj=.d)
#DEPS := $(RTRFE_OBJFILES:.obj=.d)

#ifneq ($(MAKECMDGOALS),clean)
#-include $(DEPS)
#endif

RTRDEF_OBJDIR = $(OBJDIR)/acs_rtr_cmd_util.obj $(OBJDIR)/rtrdef.obj 

RTRLS_OBJDIR = $(OBJDIR)/acs_rtr_cmd_util.obj $(OBJDIR)/rtrls.obj

RTRRM_OBJDIR = $(OBJDIR)/acs_rtr_cmd_util.obj $(OBJDIR)/rtrrm.obj 

RTRFE_OBJDIR = $(OBJDIR)/acs_rtr_cmd_util.obj $(OBJDIR)/rtrfe.obj

RTRCH_OBJDIR = $(OBJDIR)/acs_rtr_cmd_util.obj $(OBJDIR)/rtrch.obj

all: $(RTRDEF_EXE) \
	 $(RTRLS_EXE) \
	 $(RTRRM_EXE) \
         $(RTRFE_EXE) \
         $(RTRCH_EXE)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)
		
.PHONY: distclean 
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: '
		$(RM) $(OUTDIR)/$(RTRDEF_EXE)
		$(RM) $(OUTDIR)/$(RTRLS_EXE)
		$(RM) $(OUTDIR)/$(RTRRM_EXE)
		$(RM) $(OUTDIR)/$(RTRFE_EXE)
		$(RM) $(OUTDIR)/$(RTRCH_EXE)

		$(RM) $(OUTDIR)/.debug/$(RTRDEF_DBG)
		$(RM) $(OUTDIR)/.debug/$(RTRLS_DBG)
		$(RM) $(OUTDIR)/.debug/$(RTRRM_DBG)
		$(RM) $(OUTDIR)/.debug/$(RTRFE_DBG)
		$(RM) $(OUTDIR)/.debug/$(RTRCH_DBG)

$(RTRDEF_EXE): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(RTRDEF_EXE)  $(RTRDEF_OBJDIR) $(LIBSDIR) $(ACS_RTR_LIBS)
	    $(call stripp,$(RTRDEF))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(RTRLS_EXE): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(RTRLS_EXE)  $(RTRLS_OBJDIR) $(LIBSDIR) $(ACS_RTR_LIBS)
	    $(call stripp,$(RTRLS))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    

$(RTRRM_EXE): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(RTRRM_EXE)  $(RTRRM_OBJDIR) $(LIBSDIR) $(ACS_RTR_LIBS)
	    $(call stripp,$(RTRRM))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(RTRFE_EXE): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(RTRFE_EXE) $(RTRFE_OBJDIR) $(LIBSDIR) $(ACS_RTR_LIBS)
	    $(call stripp,$(RTRFE))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(RTRCH_EXE): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(RTRCH_EXE) $(RTRCH_OBJDIR) $(LIBSDIR) $(ACS_RTR_LIBS)
	    $(call stripp,$(RTRCH))
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
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)
	    
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


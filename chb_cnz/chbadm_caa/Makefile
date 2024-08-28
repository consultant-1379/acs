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
#            2010-12-15   XTANAGG   Created
#                                                            
# **********************************************************************
CNZ_NAME ?= chb_cnz
CAA_NAME ?= chbadm_caa
CXC_NAME ?= chb_cxc

include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(ACS_ROOT)/$(CNZ_NAME)/$(CAA_NAME)
OUTDIR   = $(CXCDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
CHBAPIINTINCDIR = $(BLOCKDIR)/../chbapi_caa/inc_int 
CHBAPIEXTINCDIR = $(BLOCKDIR)/../chbapi_caa/inc_ext
TESTDIR  = $(BLOCKDIR)/test

# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DCHBDEBUG 
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES +=-I$(INCDIR) -I$(CHBAPIINTINCDIR)  -I$(CHBAPIEXTINCDIR)

## # here you can add own Include paths and/or other includes 
LDFLAGS      += 

LIBSDIR +=   -L$(ACS_ROOT)/chb_cnz/chbhbeat_cxc/bin/lib_ext -L$(LIB_LOGCPLUS_SDK_LIB) \
 -L$(AP_SDK_LIB)

## # here you can add own libs 
LIBS += -lrt -lpthread

CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

#BOOST
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBS += -lboost_serialization -lboost_filesystem -lboost_system

#SEC SDK
LIBSDIR += -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB)
LIBS += -lsec_crypto_api -lsec_credu_api

ACS_CHB_CLOCKSYNC_LIBS = $(LIBS) -llog4cplus -lacs_tra -lacs_apgcc -lacs_dsd -lacs_prc -lacs_csapi -lacs_jtp -lacs_aeh

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) 

## Source files
ACS_CHB_CLOCKSYNCSRC = acs_chb_aeh.cpp acs_chb_clock_comm.cpp  \
acs_chb_clock_sync.cpp acs_chb_tra.cpp acs_chb_event.cpp acs_chb_clocksync_service.cpp \
acs_chb_clocksync_class.cpp  acs_chb_common.cpp 

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files

ACS_CHB_CLOCKSYNCOBJ = \
		$(ACS_CHB_CLOCKSYNCSRC:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj)

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC

ACS_CHB_CLOCKSYNC = acs_chbclocksyncd
ACS_CHB_CLOCKSYNC_DBG = acs_chbclocksyncd_dbg

ACS_CHB_CLOCKSYNCEXE = $(OUTDIR)/$(ACS_CHB_CLOCKSYNC)
ACS_CHB_CLOCKSYNCEXE_DBG = $(OUTDIR)/.debug/$(ACS_CHB_CLOCKSYNC_DBG)

ACS_CHB_CLOCKSYNCOBJFILES = $(OBJDIR)/acs_chb_aeh.obj $(OBJDIR)/acs_chb_clock_comm.obj \
$(OBJDIR)/acs_chb_clocksync_service.obj $(OBJDIR)/acs_chb_clocksync_class.obj \
$(OBJDIR)/acs_chb_clock_sync.obj $(OBJDIR)/acs_chb_tra.obj $(OBJDIR)/acs_chb_event.obj \
$(OBJDIR)/acs_chb_common.obj

DEPS := $(ACS_CHB_CLOCKSYNCOBJFILES:.obj=.d) 

ifneq ($(MAKECMDGOALS),clean)

-include $(DEPS)
endif

.PHONY: all

all: $(ACS_CHB_CLOCKSYNCEXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_CHB_CLOCKSYNCEXE)'
		$(SILENT)$(RM) $(ACS_CHB_CLOCKSYNCEXE) 
		$(SILENT)$(RM) $(ACS_CHB_CLOCKSYNCEXE_DBG) 

$(ACS_CHB_CLOCKSYNCEXE): $(ACS_CHB_CLOCKSYNCOBJ) $(METRICS)	
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_CHB_CLOCKSYNCEXE) $(ACS_CHB_CLOCKSYNCOBJFILES) $(LIBSDIR) $(ACS_CHB_CLOCKSYNC_LIBS)
	    $(call stripp,$(ACS_CHB_CLOCKSYNC))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)


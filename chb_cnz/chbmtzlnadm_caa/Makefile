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
CAA_NAME ?= chbmtzlnadm_caa
CXC_NAME ?= chbmtzln_cxc

include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(ACS_ROOT)/$(CNZ_NAME)/$(CAA_NAME)
OUTDIR   = $(CXCDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
CHBAPIINTINCDIR = $(BLOCKDIR)/../chbapi_caa/inc_int 
CHBAPIEXTINCDIR = $(BLOCKDIR)/../chbapi_caa/inc_ext
CHBADMINCDIR = $(BLOCKDIR)/../chbadm_caa/inc
TESTDIR  = $(BLOCKDIR)/test

# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DCHBDEBUG 
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES +=-I$(INCDIR) -I$(CHBAPIINTINCDIR)  -I$(CHBAPIEXTINCDIR) -I$(CHBADMINCDIR)

## # here you can add own Include paths and/or other includes 
LDFLAGS      += 

LIBSDIR +=   -L$(ACS_ROOT)/chb_cnz/chb_cxc/bin/lib_ext -L$(LIB_LOGCPLUS_SDK_LIB) \
 -L$(AP_SDK_LIB)

## # here you can add own libs 

CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

LIBS += -lrt -lpthread

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

ACS_CHB_MTZLN_LIBS = $(LIBS) -lacs_apgcc -llog4cplus  -lacs_tra -lacs_prc -lacs_dsd -lacs_csapi -lacs_aeh

ACS_CHB_TZ_LIBS = $(LIBS) 

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) 

## Source files
ACS_CHB_MTZLNSRC = ../chbadm_caa/src/acs_chb_mtzln_implementer.cpp acs_chb_mtzln_svc.cpp ../chbadm_caa/src/acs_chb_tra.cpp \
../chbapi_caa/src/acs_chb_file_handler.cpp ../chbapi_caa/src/acs_chb_ntzone.cpp ../chbadm_caa/src/acs_chb_mtzln_service.cpp

ACS_CHB_TZSRC = ../chbadm_caa/src/acs_chb_tz.cpp 

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files

ACS_CHB_MTZLNOBJ = \
		$(ACS_CHB_MTZLNSRC:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj)

ACS_CHB_TZOBJ = \
		$(ACS_CHB_TZSRC:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj)

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC

ACS_CHB_MTZLN = acs_chbmtzlnd
ACS_CHB_TZ = acs_chbtimezone
ACS_CHB_MTZLN_DBG = acs_chbmtzlnd_dbg
ACS_CHB_TZ_DBG = acs_chbtimezone_dbg

ACS_CHB_MTZLNEXE =  $(OUTDIR)/$(ACS_CHB_MTZLN)
ACS_CHB_TZEXE =  $(OUTDIR)/$(ACS_CHB_TZ)
ACS_CHB_MTZLNEXE_DBG =  $(OUTDIR)/.debug/$(ACS_CHB_MTZLN_DBG)
ACS_CHB_TZEXE_DBG =  $(OUTDIR)/.debug/$(ACS_CHB_TZ_DBG)

ACS_CHB_MTZLNOBJFILES = $(OBJDIR)/acs_chb_mtzln_implementer.obj  $(OBJDIR)/acs_chb_mtzln_svc.obj  $(OBJDIR)/acs_chb_tra.obj $(OBJDIR)/acs_chb_file_handler.obj $(OBJDIR)/acs_chb_mtzln_service.obj \
$(OBJDIR)/acs_chb_ntzone.obj

ACS_CHB_TZOBJFILES = $(OBJDIR)/acs_chb_tz.obj

DEPS := $(ACS_CHB_MTZLNOBJFILES:.obj=.d) \
	$(ACS_CHB_TZOBJFILES:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)

-include $(DEPS)
endif

.PHONY: all

all: $(ACS_CHB_MTZLNEXE) $(ACS_CHB_TZEXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_CHB_MTZLNEXE) $(ACS_CHB_TZEXE)'
		$(SILENT)$(RM) $(ACS_CHB_MTZLNEXE) $(ACS_CHB_TZEXE)
		$(SILENT)$(RM) $(ACS_CHB_MTZLNEXE_DBG) $(ACS_CHB_TZEXE_DBG)

$(ACS_CHB_MTZLNEXE) : $(ACS_CHB_MTZLNOBJ) $(METRICS)
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_CHB_MTZLNEXE) $(ACS_CHB_MTZLNOBJFILES) $(LIBSDIR) $(ACS_CHB_MTZLN_LIBS)
	    $(call stripp,$(ACS_CHB_MTZLN))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(ACS_CHB_TZEXE) : $(ACS_CHB_TZOBJ) $(METRICS)
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_CHB_TZEXE) $(ACS_CHB_TZOBJFILES) $(LIBSDIR) $(ACS_CHB_TZ_LIBS)
	    $(call stripp,$(ACS_CHB_TZ))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)


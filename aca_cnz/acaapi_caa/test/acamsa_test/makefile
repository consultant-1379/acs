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
#            2011-07-27   XTANAGG   Created
#                                                            
# **********************************************************************
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

include $(COMMON_ROOT)/common.mk

BLOCKDIR = $(CURDIR)
OUTDIR   = $(CURDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
ACAAPIINTINCDIR = $(BLOCKDIR)/../../inc_int 
ACAAPIEXTINCDIR = $(BLOCKDIR)/../../inc_ext
ACAADMINTINCDIR = $(BLOCKDIR)/../../../acaadm_caa/inc
ACACMDINTINCDIR = $(BLOCKDIR)/../../../acacmd_caa/inc
TESTDIR  = $(BLOCKDIR)/test

# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DacaDEBUG 
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES += -I$(INCDIR) -I$(ACAADMINTINCDIR) -I$(ACAAPIINTINCDIR) -I$(ACAAPIEXTINCDIR) -I$(ACACMDINTINCDIR) -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)

## # here you can add own Include paths and/or other includes 
LDFLAGS      += 

LIBSDIR +=   -L$(ACS_ROOT)/aca_cnz/aca_cxc/bin/lib_ext -L$(LIB_LOGCPLUS_SDK_LIB) \
 -L$(AP_SDK_LIB)

## # here you can add own libs 
LIBS += -lrt -lpthread

ACS_API_LIBS = $(LIBS) -lacs_tra -lacs_apgcc -lacs_dsd -lacs_csapi -lacs_aca

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) 

## Source files
SRCFILES = acs_aca_program.cpp \
acs_aca_heap_agent.cpp 

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files

OBJFILES =	$(SRCFILES:%.cpp=%.obj) 

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC

ACA_TEST = acamsa_test

ACS_ACA_EXE = $(OUTDIR)/$(ACA_TEST)

ACS_ACA_OBJFILES = $(SRCFILES:%.cpp=%.obj) 

DEPS := $(ACS_ACA_OBJFILES:.obj=.d) 

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all

all: $(ACS_ACA_EXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)
		$(SILENT)$(RM) $(OUTDIR)/$(ACA_TEST)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_ACA_EXE) 
		$(SILENT)$(RM) $(ACS_ACA_EXE) 


$(ACS_ACA_EXE): $(ACS_ACA_OBJFILES) $(METRICS)
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_ACA_EXE) $(wildcard $(OBJDIR)/*.obj) $(LIBSDIR) $(ACS_API_LIBS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	

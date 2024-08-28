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
#            2010-08-10   tcstanu   Created
#                                                            
# **********************************************************************

#CAA_NR = 

#include $(COMMON_MK)
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

CXCDIR = chb_cxc
CAADIR = chbadm_caa
CURDIR = $(CNZ_PATH)/$(CAADIR)/test/CPTest

BLOCKDIR = $(CURDIR)
OUTDIR   = $(CURDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own lib paths
LIBSDIR  += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)

# here you can add own compiler flags
CFLAGS += -g 
 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES +=  

LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 -L/usr/lib 

## # here you can add own libs 
LIBS +=  -lrt -lpthread -llog4cplus -lacs_prc -lacs_tra -lACS_CSAPI -lacs_pha -lacs_dsd -lacs_jtp -lacs_apgcc

## here you can add own File paths
VPATH += 	$(SRCDIR) $(OBJDIR) $(OUTDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/%) 

## Object files
#
SRCFILES = acs_chb_cp_stub.cpp 

OBJS = \
	 $(SRCFILES:%.cpp=%.obj)

$(OBJDIR)/%.obj : $(SRCFILES)
	$(ECHO) 'Compiling SRCFILES'
	$(CC) $(CFLAGS) $(CINCLUDES) $(APGINCLUDE) $*.cpp -o $@

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = acs_chb_cp_test

APEXE = $(OUTDIR)/$(APNAME)

#.PHONY: depend clean
 
.PHONY: all
all: $(APEXE)

.PHONY: clean
clean:		
	$(SILENT)$(ECHO) 'Cleaning object files'
	$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)		

.PHONY: distclean
distclean:		
	$(SILENT)$(ECHO) 'Cleaning file: $(APEXE)'
	$(SILENT)$(RM) $(APEXE)		

$(APEXE): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating Test Application: $@'
	    $(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)


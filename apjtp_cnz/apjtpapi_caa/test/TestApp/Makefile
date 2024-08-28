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
 

CNZ_PATH = $(ACS_ROOT)/apjtp_cnz
CXCDIR = apjtp_cxc
CAADIR = apjtpapi_caa
CURDIR = $(CNZ_PATH)/$(CAADIR)/test/TestApp

BLOCKDIR = $(CURDIR)
OUTDIR   = $(CURDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own lib paths
LIBSDIR  +=  -L$(LIB_LOGCPLUS_SDK_LIB)  \
	-L$(AP_SDK_LIB) 

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA)
 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(ACE_SDK_PATH)  -I$(LOTC_SDK_INC) -I$(LOTC_SDK_INC)/c++/4.3 -I$(LOTC_SDK_INC)/c++/4.3/x86_64-suse-linux -I$(COREMW_SDK_INC)  -I$(AP_SDK_INC)

LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 -L/usr/lib 

## # here you can add own libs 

LIBS +=  -llog4cplus -lacs_prc -lacs_jtp -lacs_csapi -lacs_pha -lacs_apgcc -lacs_dsd

## here you can add own File paths
VPATH += 	$(SRCDIR) $(OUTDIR) $(OBJDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
SRCFILES = JTPTest.cpp 

OBJS = \
	 $(SRCFILES:%.cpp=%.obj)

$(OBJDIR)/%.obj : $(SRCFILES)
	$(ECHO) 'Compiling SRCFILES'
	$(CC) $(CFLAGS) $(CINCLUDES) $(APGINCLUDE) $*.cpp -o $@

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = jtp_test

APEXE = $(OUTDIR)/$(APNAME)

#.PHONY: depend clean
 
.PHONY: all
all: $(APEXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $(APEXE)'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

.PHONY: distclean
distclean:		
		$(SILENT)$(ECHO) 'Cleaning binary file: $(APEXE)'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) 
		
		

$(APEXE): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating APJTP Test Application: $@'
		$(SILENT)$(RM) $(APEXE)
		$(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

METRICS : cccc lint

cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


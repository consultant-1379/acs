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
#            2010-02-19   XKUSATI   Created
#                                                            
# **********************************************************************

#CXC_NR = CXC120333
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

CURDIR   = $(shell pwd)
OUTDIR   = $(CURDIR)/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
CPOBJINCDIR = $(CURDIR)/../../inc
NTACS = ../../../../../../cm4ap/ntacs

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA)  

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
#CINCLUDES = -I$(ACE_SDK_PATH)  -I$(LOTC_SDK_INC) -I$(LOTC_SDK_INC)/c++/4.3 -I$(LOTC_SDK_INC)/c++/4.3/x86_64-suse-linux -I$(COREMW_SDK_INC) -I$(ACS_ROOT)/chb_cnz/chbapi_caa/test/CHBCPOBJAPITEST/inc -I$(CPOBJINCDIR) -I$(ACS_ROOT)/chb_cnz/chbapi_caa/inc -I$(AP_SDK_INC) -I$(ACS_ROOT)/chb_cnz/chbadm_caa/inc
CINCLUDES +=  

## # here you can add own Include paths and/or other includes -- Modified by SirishKumar
LDFLAGS      = 

# EDITED BY Satish
LIBSDIR += -L$(AP_SDK_LIB) -L$(ACS_ROOT)/chb_cnz/chb_cxc/bin 

## # here you can add own libs 
LIBS += -lrt -lpthread -lacs_chb_cpobject

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) $(SRCDIR)

## Source files
SRCFILES = $(SRCDIR)/CHBCPOBJAPITestStub.cpp 

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
#APNAME  = BLOCKNAME1API_CXC
APNAME = ACS_CHB_CP_OBJ_API_Test

APEXE = $(OUTDIR)/$(APNAME)

#.PHONY: depend clean
.PHONY: clean

.PHONY: all 

all: $(APEXE)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

metrics : cccc lint

cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
#		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
#		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

				
	

depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

#DO NOT DELETE THIS LINE -- make depend needs it


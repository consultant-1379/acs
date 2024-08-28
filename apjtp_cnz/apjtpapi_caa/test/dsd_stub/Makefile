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
#            2010-07-20   tcstanu   Created
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
CURDIR = $(CNZ_PATH)/$(CAADIR)/test/dsd_stub

BLOCKDIR = $(CURDIR)
OUTDIR   = $(CURDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own lib paths
LIBSDIR  += -L. -L$(COREMW_SDK_LIB)64 

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA)
CFLAGS += -fPIC 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR)  -I$(ACE_SDK_PATH)/

## # here you can add own libs 
LIBS += -lrt -lACE

## here you can add own File paths
VPATH += 	$(SRCDIR) $(OUTDIR) $(OBJDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
SRCFILES = API_DSD_Base.cpp API_DSD_Client.cpp API_DSD_Session.cpp API_DSD_Server.cpp

OBJS = \
	 $(SRCFILES:%.cpp=%.obj)

$(OBJDIR)/%.obj : $(SRCFILES)
	$(ECHO) 'Compiling SRCFILES'
	$(CC) $(CFLAGS) $(CINCLUDES) $(APGINCLUDE) $*.cpp -o $@

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = libACS_DSD_Stub.so

APLIB = $(OUTDIR)/$(APNAME)

#.PHONY: depend clean
.PHONY: clean
 
all: $(APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)
		#$(SILENT)$(RM) $(OBJDIR)/*.o*

$(APLIB): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating DSD Stub: $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) -shared -o $(APNAME) $(LIBSDIR) $(LIBS) $(wildcard $(OBJDIR)/*)
		mv $(APNAME) $(OUTDIR)
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

# DO NOT DELETE THIS LINE -- make depend needs it


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
#            2010-07-20   tcsvish   Created
#
#***********************************************************************
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

EAPI_PATH = $(ACS_ROOT)/apgcc_cnz/apgccapi_caa/ha/external_api
CXCDIR = demo
CURDIR = $(EAPI_PATH)/$(CXCDIR)

BLOCKDIR = $(CURDIR)
OUTDIR   = $(CURDIR)/bin
OBJDIR	 = $(CURDIR)/obj
SRCDIR	 = $(CURDIR)/src
INCDIR	 = $(CURDIR)/inc
EAPIINCDIR = $(EAPI_PATH)/inc

# Flexelint application
#FL    = $(LINT)     # Global wrap-up mode
#FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
#CPPFLAGS += -g -O3
#CPPFLAGS += -g -Wall -Wextra -fprofile-arcs -ftest-coverage
CFLAGS += $(CFLAGS_EXTRA) -g -Wall -Wextra -fprofile-arcs -ftest-coverage
#LDFLAGS += -s

# here you can add own lib paths
LIBSDIR += -L$(ACS_ROOT)/apgcc_cnz/apgcc_cxc/bin
LIBSDIR += -L"$(ACS_ROOT)/tra_cnz/tra_cxc/bin/"
LIBSDIR += -L"$(AP_SDK_LIB)"

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR)
CINCLUDES += -I$(EAPIINCDIR)
CINCLUDES += -I"$(AP_SDK_INC)"

## # here you can add own libs
LIBS += -lACS_APGCC -lACE -lacs_tra

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) $(OBJDIR)

## Source files
#

SRCFILES = $(SRCDIR)/apg_service_admin_api_test.cpp

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/%) 

## Object files
#

OBJS =  apg_service_admin_api_test.obj

$(OBJDIR)/%.obj : $(SRCFILES)
	$(ECHO) 'Compiling SRCFILES'
	$(CC) $(CFLAGS) $(CINCLUDES) $(LIBSDIR) $*.cpp -o $@

## Build instructions

APNAME = api_test
APLIB = $(OUTDIR)/$(APNAME)

#.PHONY: depend clean
.PHONY: clean

all: $(APLIB)

clean:
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

$(APLIB):   $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating application api_test : $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) $(CFLAGS) $(CINCLUDES) -o $@ $(OBJDIR)/*.obj $(LIBSDIR) $(LIBS)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

metrics : cccc lint

cccc:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
#               $(CCCC) $(wildcard $(BLOCKDIR)/inc/*)
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*)
	$(SEPARATOR_STR)

lint:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'LINT file: $@'
#               $(LINT) $(wildcard $(BLOCKDIR)/inc/*)
		$(LINT) $(wildcard $(BLOCKDIR)/src/*)
	$(SEPARATOR_STR)

depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


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

include $(COMMON_ROOT)/common.mk 

#HA_PATH = /vobs/cm4ap/apos/ha_cnz/ha_appdemo
#CXCDIR = apg_2nred_app1
#CURDIR = $(HA_PATH)/$(CXCDIR)

CXCDIR = .
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 

BLOCKDIR = $(CURDIR)
OUTDIR   = $(CURDIR)/bin
OBJDIR	 = $(CURDIR)/obj
SRCDIR	 = $(CURDIR)/src
INCDIR	 = $(CURDIR)/inc

# Flexelint application
#FL    = $(LINT)     # Global wrap-up mode
#FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
#CPPFLAGS += -g -Wall -Wextra -fprofile-arcs -ftest-coverage
CFLAGS += $(CFLAGS_EXTRA) -g -Wall -Wextra -fprofile-arcs -ftest-coverage
#CPPFLAGS += -fPIC -fmessage-length=0 -O3
#LDFLAGS += -s

# here you can add own lib paths
LIBSDIR  += -L"$(ACS_ROOT)/apgcc_cnz/apgcc_cxc/bin/"
LIBSDIR  += -L"$(AP_SDK_LIB)"

# here you can add own Assembler flags
#ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I"$(INCDIR)"
CINCLUDES += -I$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/ha/app_mngr/inc
CINCLUDES += -I"$(AP_SDK_INC)"

#Next LINe To be deleted once COMMON.mk updated

## # here you can add own libs
LIBS += -lpthread -lSaAmf -lACE -lacs_apgcc -lacs_tra
LIBS += -lacs_dsd
#LIBS += -lpthread -lSaAmf -lACE 

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) $(OBJDIR)

## Source files
#

SRCFILES = $(SRCDIR)/apg_app1_class.cpp \
           $(SRCDIR)/apg_app1_service.cpp

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/%) 

## Object files
#

OBJS =  apg_app1_class.obj \
        apg_app1_service.obj

$(OBJDIR)/%.obj : $(SRCFILES)
	$(ECHO) 'Compiling SRCFILES'
	$(CC) $(CFLAGS) $(CINCLUDES) $(LIBSDIR) $*.cpp -o $@

## Build instructions
#

APNAME = apos_ha_2napp1d
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
	    $(SILENT)$(ECHO) 'Creating application apos_ha_2napp1d : $@'
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


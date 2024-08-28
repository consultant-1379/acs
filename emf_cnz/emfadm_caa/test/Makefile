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

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

#include $(COMMON_MK)
include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(ACS_ROOT)/emf_cnz/emfadm_caa/
OUTDIR   = $(CURDIR)/../../../../emf_cxc/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test
CUTESRCDIR=$(CURDIR)/

# here you can add own lib paths
LIBSDIR  +=  -L/usr/lib64 

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DEMFDEBUG -DACS_EMF_CUTE_TEST -fprofile-arcs -ftest-coverage

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/cc/inc -I$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/acs_security/inc -I$(ACS_ROOT)/emf_cnz/emfadm_caa/inc -I$(INCDIR) -I$(TESTDIR)/cute -I$(ACE_SDK_PATH) -I$(ACS_ROOT)/emf_cnz/emfcmd_caa/inc \
 -I/home/tcsvelu/cute  

## # here you can add own libs 
#LIBS += -lrt -lpthread -lxml2 -lACE -lcurl 
LDFLAGS 	= -L. -lgcov 
LIBSDIR += -L/usr/lib64/ -L$(AP_SDK_LIB)
LIBS +=  -lxml2 -lgcov -lACE -lacs_apgcc -lacs_tra -llog4cplus

## here you can add own File paths
VPATH +=$(SRCDIR) $(CUTESRCDIR) 

## Source files

L_FILES += $(SRCFILES:%=$(SRCDIR)/src/%) 

SRCFILES =$(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_nanousbhandler.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_dvdhandler.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_mediahandler.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_param.cpp Test.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_common.cpp cute_acs_emf_nanousbhandler.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_cmdclient.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_cmd.cpp

#SRCFILES =$(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_nanousbhandler.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_mediahandler.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_dvdhandler.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_param.cpp Test.cpp $(ACS_ROOT)/emf_cnz/emfadm_caa/src/acs_emf_common.cpp cute_acs_emf_common.cpp cute_acs_emf_nanousbhandler.cpp
## Object files
#
OBJS = \
	$(SRCFILES:%.cpp=%.obj)
	
## Build instructions
#
APNAME = emf_cute_test

APLIB = $(TESTDIR)/bin/$(APNAME)

#.PHONY: depend clean
 
all: $(APLIB)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(CUTESRCDIR)/../bin/*) $(wildcard $(OBJDIR)/*)
		#$(SILENT)$(RM) $(OBJDIR)/*.o*

$(APLIB): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating CUTE appl: $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) -o $(APLIB) $(LIBSDIR) $(LIBS) $(LDFLAGS) $(wildcard $(OBJDIR)/*.obj) $(CFLAGS)  
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


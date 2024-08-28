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
#            2011-11-08  XCSSATA   Created
#                                                            
# **********************************************************************

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

include $(COMMON_ROOT)/common.mk

CNZ_NAME ?= acsc_cnz
CAA_NAME ?= logmadm_caa
CXC_NAME ?= logm_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

CXCDIR = $(CXC_PATH)
CAADIR = $(CAA_PATH)
DOCDIR = $(CXCDIR)/doc

BLOCKDIR = $(CAADIR)
BLOCKSRCDIR   = $(BLOCKDIR)/src
BLOCKINCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test
CUTEDIR  = $(TESTDIR)/CUTE
CUTESRCDIR = $(CUTEDIR)/src
CUTEINCDIR = $(CUTEDIR)/inc
CUTEOBJDIR = $(CUTEDIR)/obj
LCOV_OUTPUT = $(DOCDIR)/coverage

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

## # here you can add own libs 
LDFLAGS   = -L. -lgcov -lACE -lacs_apgcc -lacs_tra -llog4cplus    
# here you can add own lib paths
LIBSDIR +=  -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(LOTCROOT)/usr/lib64 -L$(ACE_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(COREMW_SDK_LIB)

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA)  -fprofile-arcs -ftest-coverage -DCUTE_DEBUG -DCUTE_FRIEND -DACS_LOGM_CUTE_TEST  


# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(BLOCKINCDIR) -I$(CUTEINCDIR) -I$(CUTEDIR)/cute -I$(COREMW_SDK_INC) -I$(ACE_SDK_INC) -I$(BOOST_SDK_INC)

## here you can add own File paths
VPATH +=$(BLOCKSRCDIR) $(CUTEOBJDIR) $(CUTESRCDIR) 

## Source files
#
SRCFILES = acs_logm_cleanupAction.cpp acs_logm_base.cpp acs_logm_housekeeping.cpp acs_logm_directory.cpp acs_logm_logfile.cpp \
	  acs_logm_apgccwrapper.cpp acs_logm_configurationdata.cpp acs_logm_time.cpp acs_logm_filetime.cpp \
	  acs_logm_apglogmFactory.cpp acs_logm_logmservice.cpp  acs_logm_tra.cpp \
	  Cute_acs_logm_time.cpp  Cute_acs_logm_filetime.cpp Cute_acs_logm_base.cpp Cute_acs_logm_apglogmfactory.cpp Cute_acs_logm_logfile.cpp Cute_acs_logm_directory.cpp \
	  Test.cpp

## Object files
OBJ = \
	$(SRCFILES:%.cpp=%.obj)\
	
## Build instructions
#
APNAME = acs_cutelogmaintd

APEXE = $(CUTEDIR)/bin/$(APNAME)


.PHONY: all
all: $(APEXE)
		
$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(wildcard $(CUTEOBJDIR)/*.obj) $(LIBSDIR) $(LDFLAGS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.*)
		
.PHONY: coverage_report
coverage_report:
		$(SILENT) $(ECHO) 'Creating coverage report'
		$(SILENT) rm -rf   $(LCOV_OUTPUT)
		$(SILENT) mkdir -p $(LCOV_OUTPUT)
		$(LCOV) -d $(CUTEOBJDIR) --capture -o $(LCOV_OUTPUT)/coverage.app
		$(LCOV_GENHTML) -o $(LCOV_OUTPUT) $(LCOV_OUTPUT)/coverage.app
		$(SILENT) $(ECHO) 'End Creating coverage report'

#DO NOT DELETE THIS LINE -- make depend needs it


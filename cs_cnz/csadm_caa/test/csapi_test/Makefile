# **********************************************************************
#
# Short description:
# Makefile template for CS API test application
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
#            2010-10-19   xpiokry   Created
# **********************************************************************
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk

# here you can add own lib paths
LIBSDIR  += -L../../../csapi_caa/lib/ext -L$(AP_SDK_LIB)

CFLAGS += -O3 -fPIC -fmessage-length=0

# here you can add own Include paths and/or other includes
LIBS += -lrt -lpthread -lacs_csapi

SOURCES = csapi_test.cpp 

OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

CSAPITEST_APNAME = csapi_test
CSAPITEST_APBIN = $(OBJDIR)/$(CSAPITEST_APNAME)

RPM_SPEC = csapi_test.spec
RPM_FILE = csapi_test-3.0-0.x86_64.rpm

vpath %.cpp src
#temporary added below
vpath %.h inc
vpath %.obj obj

release: all #rpm_package
	
#Build all
all: fs $(CSAPITEST_APNAME)

#Build application
$(CSAPITEST_APNAME): fs $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating executable : $(CSAPITEST_APNAME)'
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(CSAPITEST_APBIN) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(SEPARATOR_STR)
	$(NEW_LINE)

#Create rpm package
.PHONY: rpm_package
rpm_package: 
	#$(call rpm_file_creation,$(RPM_SPEC),$(RPM_FILE))
	
#Clean build files
clean:
	$(SILENT)$(ECHO) 'Cleaning build file...'
	$(SILENT)$(RM) -f $(OBJDIR)/*

#Create dependencies
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^
	
#Create obj folder if not existing
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

## DO NOT DELETE THIS LINE -- make depend needs it


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
#            2010-12-08   xpiokry   Created
# **********************************************************************
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk

# here you can add own lib paths
LIBSDIR  += -L$(AP_SDK_LIB) -L../../../cs_cxc/bin/lib_ext

CFLAGS += -O3 -fPIC -fmessage-length=0

# here you can add own Include paths and/or other includes
CINCLUDES +=  -I ../../csprot/inc -I ../../../cscmd_caa/common/inc -I ../../../csapi_caa/inc_int -I ../../inc -I ../../../csmodd_caa/inc
LIBS += -lrt -lpthread -lacs_csapi

SOURCES = group_list.cpp

OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

vpath %.obj obj

#Build all
all: group_list

#Build application
group_list: fs $(OBJ)
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o group_list $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)

#Create obj folder if not existing
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

#Clean build files
clean:
	$(SILENT)$(ECHO) 'Cleaning build file...'
	$(SILENT)$(RM) -f *.obj group_list

## DO NOT DELETE THIS LINE -- make depend needs it


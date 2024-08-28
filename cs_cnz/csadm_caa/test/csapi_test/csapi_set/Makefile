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
#            2010-12-08   xmikhal   Created
# **********************************************************************
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk

# here you can add own lib paths
LIBSDIR  += -L$(AP_SDK_LIB) -L../../../cs_cxc/bin/ \
-L../../../../cs_cxc/bin/lib_int/ -L$(BOOST_SDK_LIB)

CFLAGS += -O3 -fPIC -fmessage-length=0

# here you can add own Include paths and/or other includes
LIBS += -lrt -lpthread -lacs_csapi -lboost_serialization

SOURCES = csapi_set.cpp csapi_subscription.cpp

OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))
OBJFILES += ../../../../csadm_caa/obj/ACS_CS_EventReporter.obj
OBJFILES += ../../../../csadm_caa/obj/ACS_CS_Trace.obj

CINCLUDES += -I../../../../csapi_caa/inc_ext
CINCLUDES += -I../../../../csadm_caa/inc

vpath %.obj obj

#Build all
all: csapi_set

#Build application
csapi_set: fs $(OBJ)
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o csapi_set $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)

#Create obj folder if not existing
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

#Clean build files
clean:
	$(SILENT)$(ECHO) 'Cleaning build file...'
	$(SILENT)$(RM) -f *.obj csapi_set

## DO NOT DELETE THIS LINE -- make depend needs it


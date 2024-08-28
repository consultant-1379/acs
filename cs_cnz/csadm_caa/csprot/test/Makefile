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
#            2010-08-31   xmianon   Created
#            2010-10-18   xminaon   Removed temporary API fix
# **********************************************************************

include ../../cs_cxc/common.mk

CAA_NAME := csprot_caa
CAA_PATH := $(CNZ_PATH)/$(CAA_NAME)

OBJDIR := ../object

CINCLUDES += -I$(CAA_PATH)/inc
CINCLUDES += -I$(CNZ_PATH)/cssrv_caa/inc
CINCLUDES += -I$(CNZ_PATH)/csmodd_caa/inc
CINCLUDES += -I$(IODEV_VOB)/ACE_SDK/ACE_wrappers/
CINCLUDES += -I$(IODEV_VOB)/ACE_SDK/ACE_wrappers/ace

LIBSDIR += -L/home/mann/lib64

PROTOCOL_SOURCES := protocoltest.cpp
PROTOCOL_SOURCES += $(notdir $(wildcard ../src/*.cpp) )
PROTOCOL_SOURCES += ACS_CS_Exception.cpp ACS_CS_Error.cpp ACS_CS_IOException.cpp ACS_CS_ProtocolException.cpp \
ACS_CS_SystemException.cpp ACS_CS_ParsingException.cpp ACS_CS_VersionException.cpp ACS_CS_AttributeException.cpp \
ACS_CS_TimeoutException.cpp ACS_CS_Throwable.cpp ACS_CS_Trace.cpp ACS_CS_Attribute.cpp
PROTOCOL_OBJECTS := $(patsubst %.cpp,%.obj,$(PROTOCOL_SOURCES))

ALL_SOURCES := $(wildcard ../src/*.cpp)

vpath %.cpp .:../src:$(CAA_PATH)/src:$(CNZ_PATH)/cssrv_caa/src:$(CNZ_PATH)/csmodd_caa/src
vpath %.h .:$(CAA_PATH)/inc
vpath %.obj ${OBJDIR}

# Targets

all: protocol

protocol: $(PROTOCOL_OBJECTS)
	$(CC) $(addprefix $(OBJDIR)/,$(PROTOCOL_OBJECTS)) $(LDFLAGS) $(LIBSDIR) $(LIBS) -o $@

.PHONY : cppcheck
cppcheck:
	$(SILENT) echo "Running cppcheck"
	$(SILENT) cppcheck $(ALL_SOURCES) | egrep "(error)|100% done"

.PHONY : clean
clean:
	@echo "Cleaning"
	@rm -f $(OBJDIR)/* ./*.gcov *.output protocol


# **********************************************************************
#
# Short description:
# Makefile template for CS API library
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
#            2010-11-11   xminaon   Created
#            2010-11-16   xpiokry   Updated to follow DR
#            2011-01-12   xmaglex	Added targets: prebuild, postbuild and metrics 
# **********************************************************************

include ../../cs_cxc/include.mk

LIB_FILENAME = libprot.a
LIB_PATH = $(LIBAPI_INT_DIR)
CSPROT_APLIB = $(LIB_PATH)/$(LIB_FILENAME)

# here you can add own lib paths
LIBSDIR  += 

CFLAGS += -fPIC -fmessage-length=0
CFLAGS += 

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage --coverage
#CFLAGS += -I$(INCDIR)
GCOV_FLAGS =
LIBS += -lgcov
endif

# here you can add own Include paths and/or other includes
CINCLUDES += -I../inc -I ../../csapi_caa/inc_ext -I ../../csapi_caa/inc_int -I$(INCDIR)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)

# libcommon.a
PROTOCOL_SRC = $(notdir $(wildcard src/*.cpp))  
PROTOCOL_SRC += ACS_CS_Trace.cpp ACS_CS_Exception.cpp \
		ACS_CS_Throwable.cpp ACS_CS_Error.cpp ACS_CS_IOException.cpp ACS_CS_ProtocolException.cpp \
		ACS_CS_SystemException.cpp ACS_CS_ParsingException.cpp ACS_CS_VersionException.cpp \
		ACS_CS_AttributeException.cpp ACS_CS_TimeoutException.cpp ACS_CS_Throwable.cpp
PROTOCOL_OBJ = $(PROTOCOL_SRC:.cpp=.obj)

DEPS := $(PROTOCOL_SRC:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)
	-include $(DEPS)
endif

vpath %.cpp $(SRCDIR) ../src 
vpath %.h $(INCDIR) ../inc 
vpath %.obj $(OBJDIR)

#Create all
.PHONY: all
all: $(LIB_FILENAME)

#Create library
.PHONY: $(LIB_FILENAME)
$(LIB_FILENAME): fs $(PROTOCOL_OBJ)
	$(AR) cr $(CSPROT_APLIB) $(addprefix $(OBJDIR)/, $(PROTOCOL_OBJ))
	
# Perform prebuild actions like ClearCase checkouts
# Perform postbuild actions like ClearCase checkins
# Undo prebuild actions like ClearCase checkouts
# ClearCase checkouts
.PHONY: checkout
checkout:	
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Checking out CS protocol binary $(CSPROT_APLIB)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(CSPROT_APLIB))))' != 'CHECKEDOUT' ]; then cleartool co -nc $(CSPROT_APLIB); else echo 'Binary is already checked out!'; fi
	$(NEW_LINE)

# ClearCase checkins
.PHONY: checkin
checkin:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Checking in CS protocol binary $(CSPROT_APLIB)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(CSPROT_APLIB))))' = 'CHECKEDOUT' ]; then cleartool ci -nc $(CSPROT_APLIB); else echo 'Binary is not checked out!'; fi
	$(NEW_LINE)

# Undo checkouts
.PHONY: uncheckout
uncheckout:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Undoing checkout of CS protocol binary $(CSPROT_APLIB)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(CSPROT_APLIB))))' = 'CHECKEDOUT' ]; then cleartool uncheckout -rm $(CSPROT_APLIB); else echo 'Binary is not checked out!'; fi
	$(NEW_LINE)

#Create obj folder if not existing
.PHONY: fs
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

#Clean build files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning PROT object files...'
	$(SILENT)$(RM) -f $(OBJDIR)/*
	$(SILENT)$(ECHO) 'Cleaning PROT library file: $(LIB_FILENAME)'
	$(SILENT)$(RM) -f $(LIB_PATH)/$(LIB_FILENAME)

#Clean binary files
.PHONY: distclean
distclean: clean

# Perform chain of code analysis tools - add more?
.PHONY: metrics
metrics : cccc lint

# CCCC
# C and C++ Code Counter
.PHONY: cccc
cccc:
	$(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
	$(CCCC) $(wildcard $(INCDIR)/*) 
	$(CCCC) $(wildcard $(SRCDIR)/*) 

# LINT
# Splint is a tool for static code analysis
.PHONY: lint
lint:
	$(SILENT)$(ECHO) 'LINT file: $@'
	$(LINT) $(wildcard $(INCDIR)/*) 
	$(LINT) $(wildcard $(SRCDIR)/*) 

## DO NOT DELETE THIS LINE -- make depend needs it


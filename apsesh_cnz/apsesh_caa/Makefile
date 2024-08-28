# **********************************************************************
#
# Short description:
# Makefile for APSESH service binary
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
# **********************************************************************************
#
# Rev        Date         Name      What
# -----      -------      --------  --------------------------
#            2011-04-26   xmaglex   Created
#
# **********************************************************************************
SOFTWARE_TYPE ?= app
CNZ_NAME ?= apsesh_cnz
CAA_NAME ?= apsesh_caa
CXC_NAME ?= apsesh_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

MODULE_NAME = APSESH_DAEMON
MODULE_VERSION = 1_0
MODULE_SEARCH = ./src ./inc
export DOXYGEN_OUTPUT
export MODULE_NAME
export MODULE_VERSION
export MODULE_SEARCH

# Setting the application name
BUILD_NAME = APSESH
APP_FILENAME = acs_apseshd
APP_FILENAME_DBG = acs_apseshd_dbg

## Source directories. Here you can list your source directories. Example: SRCDIR src1 src2 src3 (use spaces)
SRCDIRS ?= src

## Source files
SOURCES = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
SOURCES += $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))

## Object files
OBJ = $(SOURCES:.cpp=.obj)
OBJFILES := $(addprefix $(OBJDIR)/,$(notdir $(OBJ)))

# Lib paths
LIBSDIR  += -L$(OUTDIR) -L$(AP_SDK_LIB) -L$(ACE_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)

# Libs to use
LIBS += -lrt -lpthread -lacs_csapi -lacs_apgcc -lacs_aeh -lACE -lacs_jtp -lacs_prc -lacs_tra -lacs_dsd -llog4cplus

## here you can add own compiler flags
CFLAGS +=

# here you can add own linker flags
LDFLAGS +=

OUT_FILENAME = $(APP_FILENAME)

# Include directories
CINCLUDES += -I../apsesh_caa/inc
CINCLUDES += -I$(AP_SDK_INC) -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)

# List of directories to be searched for source files
VPATH += $(SRCDIR) $(INCDIR) $(OUTDIR) $(OBJDIR)

#SOURCES = ACS_APSESH_Trace.cpp \
		  ACS_APSESH_EventMgr.cpp \
          ACS_APSESH_JtpMgr.cpp \
          ACS_APSESH_Main.cpp \
          ACS_APSESH_TestableJtpMgr.cpp \
          ACS_APSESH_Service.cpp
          


######################################################################################
# ALL
# Default target to build the binary of this block
######################################################################################
.PHONY: all
all: $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating executable : $(APP_FILENAME)'
	$(SILENT)$(RM) $(OBJDIR)/$(APP_FILENAME)
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(OUTDIR)/$(APP_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(SEPARATOR_STR)
	$(call stripp,$(APP_FILENAME))
	$(NEW_LINE)	

######################################################################################
# CLEAN
# Delete all files in object directory
######################################################################################	
.PHONY: clean
clean:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Cleaning $(BUILD_NAME) objects...'
	$(SILENT)$(RM) -r $(OBJDIR)/*.obj
	$(SILENT)$(RM) -r $(OBJDIR)/*.d
	$(SEPARATOR_STR)
	$(NEW_LINE)

######################################################################################
# DISTCLEAN
# Delete binary files related to this block
######################################################################################
.PHONY: distclean
distclean: clean
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Cleaning $(BUILD_NAME) binaries: $(RM) -rf $(OUTDIR)/$(APP_FILENAME)...'
	$(SILENT)$(RM) -rf $(OUTDIR)/$(APP_FILENAME)
	$(SEPARATOR_STR)
	$(NEW_LINE)



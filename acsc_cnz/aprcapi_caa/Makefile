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
# PA1        2010-07-20   xtanagg   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= acsc_cnz
CAA_NAME ?= aprcapi_caa
CXC_NAME ?= aprc_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

OUTDIR   = $(CXC_PATH)/bin/lib_ext
OBJDIR   = $(CXC_PATH)/../aprcapi_caa/obj
SRCDIR   = $(CXC_PATH)/../aprcapi_caa/src
INTINCDIR   = $(CXC_PATH)/../aprcapi_caa/inc_int
EXTINCDIR   = $(CXC_PATH)/../aprcapi_caa/inc_ext
TESTDIR  = $(CXC_PATH)/../aprcapi_caa/test

LIB_NAME = acs_aprc

LIB_RELEASE_MAJOR ?= 1
LIB_RELEASE_MINOR ?= 1
LIB_RELEASE_BUILD ?= 1
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INTINCDIR)  -I$(EXTINCDIR) -I$(COM_SDK_INC)

# here you can add own lib paths
LIBSDIR  +=  -L$(AP_SDK_LIB)

# here you can add own compiler flags
#CFLAGS += -O3 -fPIC -fmessage-length=0
CFLAGS += 

# here you can add own Assembler flags
ASMFLAGS += 

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 
LIBS +=  -lrt -lpthread 

## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(INTINCDIR) $(EXTINCDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 


SRCFILES = acs_aprc_lcm.cpp acs_aprc_cmd.cpp

OBJFILES = $(OBJDIR)/acs_aprc_lcm.obj $(OBJDIR)/acs_aprc_cmd.obj 

DEPS := $(OBJFILES:.obj=.d)
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

## Object files
#
OBJS =	\
	$(SRCFILES:%.cpp=%.obj) 

## Build instructions
#

APRC_APLIB = $(OUTDIR)/$(LIB_FILENAME)
APRC_APLIB_SO = $(OUTDIR)/$(LIB_LINKER_NAME)

.PHONY: all
 
all: $(APRC_APLIB)

$(APRC_APLIB): $(OBJS) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library: $@'
		$(CC) -o $(APRC_APLIB) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS) 
	    $(call stripp,$(LIB_FILENAME))
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: clean 
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

.PHONY: distclean 
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APRC_APLIB)'
		$(RM) $(APRC_APLIB)
		$(RM) $(OUTDIR)/.debug/$(LIB_FILENAME_DBG) 



# DO NOT DELETE THIS LINE -- make depend needs it


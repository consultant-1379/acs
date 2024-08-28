# **********************************************************************
#
# Short description:
# Makefile template for DSD Private Implementation Library
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
#	     2011-05-18   xmalsha   created
# **********************************************************************

# Clearcase name organizations
CNZ_NAME ?= usa_cnz
CAA_NAME ?= usaadm_fm_caa
CXC_NAME ?= usa_fm_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=APG_FM
BLOCK_VERSION=1_0
BLOCK_SEARCH=./src ./inc
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

SRCDIR = $(CAA_PATH)/src
INCDIR = $(CAA_PATH)/inc
OBJDIR = $(CAA_PATH)/obj
CXCDIR = $(CNZ_PATH)/$(CXC_NAME)
OUTDIR=  $(CXCDIR)/bin/lib_int

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make LIB_NAME=ACS_DSD2 SO_RELEASE_BUILD=1
LIB_NAME ?= acs_usa_fm
LIB_RELEASE_MAJOR ?= 2
LIB_RELEASE_MINOR ?= 3
LIB_RELEASE_BUILD ?= 0

## Source directories. Here you can list your source directories. Example: SRCDIR src1 src2 src3 (use spaces)
SRCDIRS ?= src

## Include files
CINCLUDES += -I$(INCDIR)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(COM_SDK_INC)
CINCLUDES += -I$(AP_SDK_INC)

## Source files
SRCFILES =	$(SRCDIR)/acs_usa_evtConsumer.cpp 

OBJFILES =	$(OBJDIR)/acs_usa_evtConsumer.obj

## Setting the so library compiler information
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so


VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

## here you can add own compiler flags
CFLAGS += -O3

# here you can add own linker flags
LDFLAGS += -s -shared -Wl,-soname=$(LIB_SONAME)


# here you can add own libs
LIBS += -lrt -lpthread -lacs_aeh -lacs_apgcc -lboost_filesystem  -lboost_system

# here you can add own lib paths
#LIBSDIR += -L"$(CXCDIR)/bin"
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L$(BOOST_SDK_LIB)

## Object files
OBJ =	$(SRCFILES:%.cpp=%.obj) \
	$(SRCFILES:%.c=%.obj)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(LIB_INT)
AP_FILENAME=$(OUTDIR)/$(LIB_FILENAME)


.PHONY: all CFLAGS += $(GCOV_FLAGS)
all:	$(AP_FILENAME)

$(AP_FILENAME): $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(LIB_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(AP_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LIB_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) $(OBJDIR)/*.obj
	$(RM) $(OBJDIR)/*.d
#	$(RM) $(AP_FILENAME)
#	$(RM) $(CXCDIR)/bin/libacs_usa_fm.so.$(LIB_VERSION)

.PHONY: distclean
distclean: clean
	$(RM) -f $(AP_FILENAME)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(call ct_co_ap_sdk_lib_dir)
	$(CP) $(AP_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_ln_ap_sdk_lib_link,$(LIB_FILENAME),$(LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_SONAME),$(LIB_LINKER_NAME))
#	$(call ct_ci,$(AP_SDK_LIB))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'


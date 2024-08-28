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
CAA_NAME ?= comfm_proto/comfmadm_caa
CXC_NAME ?= comfm_proto/comfm_cxc

include $(COMMON_ROOT)/common.mk 

# Basic paths
VOB_PATH = ../../../../../../cm4ap/$(VOB_NAME)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)

NTACS_PATH=../../../..
export NTACS_PATH
VOB_NAME ?= comfmadm_caa
CNZ_NAME ?= comfm_cnz
CAA_NAME ?= comfmadm_caa
CXC_NAME ?= comfm_cxc

include $(COMMON_ROOT)/common.mk 

 

DOXYGENFILECONFIG := $(TOOLS_PATH)/linux_conf_file/Doxyfile.config

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
COMINC = $(CAA_PATH)/inc/com_headers
COMIINC = $(CAA_PATH)/inc/com_iheaders
OBJDIR = $(CAA_PATH)/obj
CXCDIR = $(CNZ_PATH)/$(CXC_NAME)

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make LIB_NAME=ACS_DSD2 SO_RELEASE_BUILD=1
LIB_NAME ?= com_fm_apg
LIB_RELEASE_MAJOR ?= 1
LIB_RELEASE_MINOR ?= 0
LIB_RELEASE_BUILD ?= 0

## Source directories. Here you can list your source directories. Example: SRCDIR src1 src2 src3 (use spaces)
SRCDIRS ?= src

## Include files
CINCLUDES += -I$(INCDIR)
CINCLUDES += -I$(COMINC)
CINCLUDES += -I$(COMIINC)
CINCLUDES += -I$(AP_SDK_INC)

## Source files
SRCFILES =	$(SRCDIR)/FmApg.cpp 

OBJFILES =	$(OBJDIR)/FmApg.obj

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
LIBS += -lrt -lpthread -lacs_tra

# here you can add own lib paths
#LIBSDIR += -L"$(CXCDIR)/bin"
LIBSDIR += -L"$(AP_SDK_LIB)"

## Object files
OBJ =	$(SRCFILES:%.cpp=%.obj) \
	$(SRCFILES:%.c=%.obj)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all:	$(OUTDIR)/$(LIB_FILENAME)

$(OUTDIR)/$(LIB_FILENAME): $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(PROD_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(OUTDIR)/$(LIB_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
#	$(SILENT)$(CC) $(CFLAGS) -o $(OUTDIR)/$(LIB_FILENAME) $(OBJFILES) $(LDFLAGS) 
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d
	$(RM) -r $(CXCDIR)/bin/libcom_fm_apg.so.1.0.0

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(LIB_FILENAME)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(call ct_co_ap_sdk_lib_dir)
	$(CP) $(OUTDIR)/$(LIB_FILENAME) $(AP_SDK_LIB)/.
#	$(call ct_mkelem,$(AP_SDK_LIB)/$(LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_FILENAME),$(LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_SONAME),$(LIB_LINKER_NAME))
#	$(call ct_ci,$(AP_SDK_LIB))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'


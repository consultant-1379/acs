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
#            2010-07-20   tcstanu   Created
#                                                            
# **********************************************************************

#CAA_NR = 

#include $(COMMON_MK)
CNZ_NAME ?= chb_cnz
CAA_NAME ?= chbmtzlnapi_caa
CXC_NAME ?= chbmtzln_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk 

CAADIR = chbmtzlnapi_caa
CURDIR = $(CNZ_PATH)/$(CAADIR)
OUTDIR   = $(CXCDIR)/bin/lib_ext
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INCDIR   = $(CURDIR)/inc
INTINCDIR = $(CNZ_PATH)/chbapi_caa/inc_int
EXTINCDIR = $(CNZ_PATH)/chbapi_caa/inc_ext
ADMINCDIR = $(CNZ_PATH)/chbadm_caa/inc
TESTDIR  = $(CURDIR)/test

ACS_CHB_MTZ_LIB_NAME = acs_chb_mtz

ACS_CHB_MTZ_LIB_RELEASE_MAJOR ?= 2
ACS_CHB_MTZ_LIB_RELEASE_MINOR ?= 5
ACS_CHB_MTZ_LIB_RELEASE_BUILD ?= 3
ACS_CHB_MTZ_LIB_VERSION ?= $(ACS_CHB_MTZ_LIB_RELEASE_MAJOR).$(ACS_CHB_MTZ_LIB_RELEASE_MINOR).$(ACS_CHB_MTZ_LIB_RELEASE_BUILD)
ACS_CHB_MTZ_LIB_FILENAME = lib$(ACS_CHB_MTZ_LIB_NAME).so.$(ACS_CHB_MTZ_LIB_VERSION)
ACS_CHB_MTZ_LIB_FILENAME_DBG = lib$(ACS_CHB_MTZ_LIB_NAME).so.$(ACS_CHB_MTZ_LIB_VERSION)_dbg
ACS_CHB_MTZ_LIB_SONAME = lib$(ACS_CHB_MTZ_LIB_NAME).so.$(ACS_CHB_MTZ_LIB_RELEASE_MAJOR)
ACS_CHB_MTZ_LIB_LINKER_NAME = lib$(ACS_CHB_MTZ_LIB_NAME).so

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR) -I$(EXTINCDIR) -I$(INTINCDIR)  -I$(ADMINCDIR)

# here you can add own lib paths
LIBSDIR  += 

# here you can add own compiler flags
CFLAGS += -O3 -fPIC -fmessage-length=0

# here you can add own Assembler flags
ASMFLAGS += 

LDFLAGS += -s -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 

CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

ACS_CHB_MTZ_LIBS = $(LIBS)  

ADMSRCDIR = $(ACS_ROOT)/chb_cnz/chbadm_caa/src
## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(ADMSRCDIR)

## Source files
#

L_FILES += $(ACS_CHB_MTZOBJ:%=$(BLOCKDIR)/src/%)

ACS_CHB_MTZSRC = ../chbapi_caa/src/acs_chb_file_handler.cpp acs_chb_mtzclass.cpp ../chbapi_caa/src/acs_chb_mtzconv.cpp \
../chbapi_caa/src/acs_chb_ntzone.cpp ../chbapi_caa/src/acs_chb_thread_handler.cpp 

## Object files
OBJFILES = $(OBJDIR)/acs_chb_file_handler.obj $(OBJDIR)/acs_chb_mtzclass.obj \
$(OBJDIR)/acs_chb_mtzconv.obj $(OBJDIR)/acs_chb_ntzone.obj \
$(OBJDIR)/acs_chb_thread_handler.obj  $(OBJDIR)/acs_chb_cp_object.obj 

ACS_CHB_MTZOBJFILES = $(OBJDIR)/acs_chb_file_handler.obj $(OBJDIR)/acs_chb_mtzclass.obj $(OBJDIR)/acs_chb_mtzconv.obj \
$(OBJDIR)/acs_chb_ntzone.obj $(OBJDIR)/acs_chb_thread_handler.obj 

## Object files
#
OBJS =  \
        $(SRCFILES:%.cpp=%.obj)

ACS_CHB_MTZOBJ = \
	$(ACS_CHB_MTZSRC:%.cpp=%.obj)

DEPS  = $(ACS_CHB_MTZOBJ:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)

-include $(DEPS)
endif

## Build instructions
#

ACS_MTZ_APLIB = $(OUTDIR)/$(ACS_CHB_MTZ_LIB_FILENAME)
ACS_MTZ_APLIB_SO = $(OUTDIR)/$(ACS_CHB_MTZ_LIB_SONAME)

.PHONY: clean 
.PHONY: distclean 
.PHONY: all
 
all: $(ACS_MTZ_APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_MTZ_APLIB)'
		$(SILENT)$(RM) $(ACS_MTZ_APLIB) 
		$(SILENT)$(RM) $(OUTDIR)/.debug/$(ACS_CHB_MTZ_LIB_FILENAME_DBG) 

$(ACS_MTZ_APLIB): $(ACS_CHB_MTZOBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library ACS MTZ: $@'
		$(SILENT)$(RM) $(ACS_MTZ_APLIB)
		$(CC) -o $(ACS_MTZ_APLIB) $(ACS_CHB_MTZOBJFILES) $(LDFLAGS) $(LIBSDIR) $(ACS_CHB_MTZ_LIBS) 
	    $(call stripp,$(ACS_CHB_MTZ_LIB_FILENAME))
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(call ct_co_ap_sdk_lib_dir)
	$(CP) $(OUTDIR)/$(ACS_CHB_MTZ_LIB_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_mkelem,$(AP_SDK_LIB)/$(ACS_CHB_MTZ_LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(ACS_CHB_MTZ_LIB_FILENAME),$(ACS_CHB_MTZ_LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(ACS_CHB_MTZ_LIB_SONAME),$(ACS_CHB_MTZ_LIB_LINKER_NAME))
#	$(call ct_ci,$(AP_SDK_LIB))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'


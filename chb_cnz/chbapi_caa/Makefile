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
CAA_NAME ?= chbapi_caa
CXC_NAME ?= chbhbeat_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk 

CURDIR = $(CAA_PATH)
OUTDIR = $(CXC_PATH)/bin/lib_ext
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INTINCDIR  = $(CURDIR)/inc_int
EXTINCDIR  = $(CURDIR)/inc_ext
ADMINCDIR = $(ACS_ROOT)/$(CNZ_NAME)/chbadm_caa/inc
TESTDIR  = $(CURDIR)/test

ACS_CHB_CPOBJECT_LIB_NAME = acs_chb_cpobject

ACS_CHB_CPOBJECT_LIB_RELEASE_MAJOR ?= 2
ACS_CHB_CPOBJECT_LIB_RELEASE_MINOR ?= 1
ACS_CHB_CPOBJECT_LIB_RELEASE_BUILD ?= 1
ACS_CHB_CPOBJECT_LIB_VERSION ?= $(ACS_CHB_CPOBJECT_LIB_RELEASE_MAJOR).$(ACS_CHB_CPOBJECT_LIB_RELEASE_MINOR).$(ACS_CHB_CPOBJECT_LIB_RELEASE_BUILD)
ACS_CHB_CPOBJECT_LIB_FILENAME = lib$(ACS_CHB_CPOBJECT_LIB_NAME).so.$(ACS_CHB_CPOBJECT_LIB_VERSION)
ACS_CHB_CPOBJECT_LIB_FILENAME_DBG = lib$(ACS_CHB_CPOBJECT_LIB_NAME).so.$(ACS_CHB_CPOBJECT_LIB_VERSION)_dbg
ACS_CHB_CPOBJECT_LIB_SONAME = lib$(ACS_CHB_CPOBJECT_LIB_NAME).so.$(ACS_CHB_CPOBJECT_LIB_RELEASE_MAJOR)
ACS_CHB_CPOBJECT_LIB_LINKER_NAME = lib$(ACS_CHB_CPOBJECT_LIB_NAME).so

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(EXTINCDIR) -I$(INTINCDIR)  -I$(ADMINCDIR)

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

ADMSRCDIR = $(ACS_ROOT)/$(CNZ_NAME)/chbadm_caa/src
## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(ADMSRCDIR)

## Source files
#

L_FILES += $(ACS_CHB_CPOBJECTSRC:%=$(BLOCKDIR)/src/%)

ACS_CHB_CPOBJECTSRC = acs_chb_cp_object.cpp acs_chb_cp_object_implementer.cpp acs_chb_error.cpp acs_chb_mtzconv.cpp acs_chb_file_handler.cpp acs_chb_ntzone.cpp 
 
## Object files
OBJFILES = $(OBJDIR)/acs_chb_file_handler.obj $(OBJDIR)/acs_chb_mtzclass.obj \
$(OBJDIR)/acs_chb_mtzconv.obj $(OBJDIR)/acs_chb_ntzone.obj \
$(OBJDIR)/acs_chb_thread_handler.obj  $(OBJDIR)/acs_chb_cp_object.obj 

ACS_CHB_CPOBJECTOBJFILES = $(OBJDIR)/acs_chb_cp_object.obj $(OBJDIR)/acs_chb_error.obj  $(OBJDIR)/acs_chb_cp_object_implementer.obj

## Object files
#
OBJS =  \
        $(SRCFILES:%.cpp=%.obj)

ACS_CHB_CPOBJECTOBJ = \
	$(ACS_CHB_CPOBJECTSRC:%.cpp=%.obj)

DEPS += $(ACS_CHB_CPOBJECTOBJ:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)

-include $(DEPS)
endif

## Build instructions
#

ACS_CHB_CPOBJECT_APLIB =  $(OUTDIR)/$(ACS_CHB_CPOBJECT_LIB_FILENAME)
ACS_CHB_CPOBJECT_APLIB_SO = $(OUTDIR)/$(ACS_CHB_CPOBJECT_LIB_SONAME)

.PHONY: clean 
.PHONY: distclean 
.PHONY: all
 
all: $(ACS_CHB_CPOBJECT_APLIB) $(ACS_MTZ_APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_CHB_CPOBJECT_APLIB)'
		$(SILENT)$(RM) $(ACS_CHB_CPOBJECT_APLIB)
		$(SILENT)$(RM) $(OUTDIR)/.debug/$(ACS_CHB_CPOBJECT_LIB_FILENAME_DBG)

$(ACS_CHB_CPOBJECT_APLIB): $(ACS_CHB_CPOBJECTOBJ)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library ACS CP OBJECT: $@'
	        $(SILENT)$(RM) $(ACS_CHB_CPOBJECT_APLIB)
		$(CC) -o $(ACS_CHB_CPOBJECT_APLIB) $(ACS_CHB_CPOBJECTOBJFILES) $(LDFLAGS) $(LIBSDIR) $(ACS_CHB_CPOBJECT_LIBS)
	 	$(call stripp,$(ACS_CHB_CPOBJECT_LIB_FILENAME))
	 	$(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(call ct_co_ap_sdk_lib_dir)
	$(CP) $(OUTDIR)/$(ACS_CHB_CPOBJECT_LIB_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_mkelem,$(AP_SDK_LIB)/$(ACS_CHB_CPOBJECT_LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(ACS_CHB_CPOBJECT_LIB_FILENAME),$(ACS_CHB_CPOBJECT_LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(ACS_CHB_CPOBJECT_LIB_SONAME),$(ACS_CHB_CPOBJECT_LIB_LINKER_NAME))
#	$(call ct_ci,$(AP_SDK_LIB))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'


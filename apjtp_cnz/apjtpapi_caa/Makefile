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

CAA_NR = 1090870

#include $(COMMON_MK)
CNZ_PATH = $(ACS_ROOT)/apjtp_cnz
include $(COMMON_ROOT)/common.mk

CAADIR = apjtpapi_caa
CURDIR = $(CNZ_PATH)/$(CAADIR)

OUTDIR   = $(CXCDIR)/bin/lib_ext
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INTINCDIR   = $(CURDIR)/inc_int
EXTINCDIR   = $(CURDIR)/inc_ext
TESTDIR  = $(CURDIR)/test

LIB_NAME = acs_jtp

LIB_RELEASE_MAJOR ?= 2
LIB_RELEASE_MINOR ?= 6
LIB_RELEASE_BUILD ?= 0
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INTINCDIR)  -I$(EXTINCDIR)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

# here you can add own lib paths
LIBSDIR  +=  -L$(AP_SDK_LIB)
LIBSDIR  +=  -L$(ACE_SDK_LIB)
LIBSDIR  +=  -L$(COREMW_SDK_LIB)
LIBSDIR  +=  -L$(SEC_CREDU_SDK_LIB)

# here you can add own compiler flags
#CFLAGS += -O3 -fPIC -fmessage-length=0
CFLAGS += 

# here you can add own Assembler flags
ASMFLAGS += 

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 
LIBS +=  -lrt -lpthread -lACE -lacs_tra -lacs_prc -lacs_dsd

## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(INTINCDIR) $(EXTINCDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES = ACS_JTP_Conversation_R2A.cpp ACS_JTP_Job_R2A.cpp ACS_JTP_Service_R2A.cpp \
ACS_PRC_info.cpp jtp.cpp JTP_Service.cpp JTP_Session.cpp JTP_Service_R2A.cpp JTP_Session_R2A.cpp\
JTP_Service_R3A.cpp JTP_Session_R3A.cpp ACS_JTP_Conversation_R3A.cpp ACS_JTP_Job_R3A.cpp ACS_JTP_Service_R3A.cpp 

OBJFILES = $(OBJDIR)/ACS_JTP_Conversation_R2A.obj $(OBJDIR)/ACS_JTP_Job_R2A.obj $(OBJDIR)/ACS_JTP_Service_R2A.obj \
$(OBJDIR)/ACS_PRC_info.obj $(OBJDIR)/jtp.obj $(OBJDIR)/JTP_Service.obj $(OBJDIR)/JTP_Session.obj $(OBJDIR)/JTP_Service_R2A.obj \
$(OBJDIR)/JTP_Session_R2A.obj $(OBJDIR)/JTP_Service_R3A.obj $(OBJDIR)/JTP_Session_R3A.obj \
$(OBJDIR)/ACS_JTP_Conversation_R3A.obj $(OBJDIR)/ACS_JTP_Job_R3A.obj \
$(OBJDIR)/ACS_JTP_Service_R3A.obj 

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

APJTP_APLIB = $(OUTDIR)/$(LIB_FILENAME)
APJTP_APLIB_SO = $(OUTDIR)/$(LIB_LINKER_NAME)

.PHONY: all
 
all: $(APJTP_APLIB)

$(APJTP_APLIB): $(OBJS) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library: $@'
		$(CC) -o $(APJTP_APLIB) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS) 
	    $(call stripp,$(LIB_FILENAME))
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: clean 
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

.PHONY: distclean 
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APJTP_APLIB)'
		$(RM) $(APJTP_APLIB)
		$(RM) $(OUTDIR)/.debug/$(LIB_FILENAME_DBG) 

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(call ct_co_ap_sdk_lib_dir)
	$(CP) $(OUTDIR)/$(LIB_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_mkelem,$(AP_SDK_LIB)/$(LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_FILENAME),$(LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_SONAME),$(LIB_LINKER_NAME))
#	$(call ct_ci,$(AP_SDK_LIB))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

# DO NOT DELETE THIS LINE -- make depend needs it


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
#            2010-02-19   teiggam   Created
#            2010-10-21   xantior   Adjusted
#            2010-11-24   xgencol   Adjusted
#	     2010-11-24   xmalsha   modified
#	     2011-06-o8   xantior   add libacs_dsd link
# **********************************************************************

# Clearcase name organizations
CNZ_NAME ?= apgcc_cnz
CAA_NAME ?= apgccapi_caa
CXC_NAME ?= apgcc_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

DOCDIR = $(ACS_ROOT)/apgccapi_cnz/apgcc_cxc/doc
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=APGCC_API_CAA
BLOCK_VERSION=1_0
BLOCK_SEARCH=./src ./inc
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

SRCDIRCC = $(CAA_PATH)/cc/src
SRCDIROM = $(CAA_PATH)/OMHandling/src
SRCDIRHACC = $(CAA_PATH)/ha
SRCDIRHAAPPMNGR = $(CAA_PATH)/ha/app_mngr/src
SRCDIRHAEAPI = $(CAA_PATH)/ha/external_api/src
SRCDIRACS  = $(CAA_PATH)/acs_security/src
INCDIRACSEXT  = $(CAA_PATH)/acs_security/inc_ext
INCDIRACSINT = $(CAA_PATH)/acs_security/inc_int
INCDIRCC = $(CAA_PATH)/cc/inc
INCDIROMINT = $(CAA_PATH)/OMHandling/inc_int
INCDIROMEXT = $(CAA_PATH)/OMHandling/inc_ext
INCDIRHAAPPMNGR = $(CAA_PATH)/ha/app_mngr/inc
INCDIRHAEAPI = $(CAA_PATH)/ha/external_api/inc
INCDIRCCINT = $(CAA_PATH)/cc/inc_int
INCDIRCCEXT = $(CAA_PATH)/cc/inc_ext

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make LIB_NAME=ACS_DSD2 SO_RELEASE_BUILD=1
LIB_NAME ?= acs_apgcc
LIB_RELEASE_MAJOR ?= 5
LIB_RELEASE_MINOR ?= 16
LIB_RELEASE_BUILD ?= 2

#for symbol strip
OUTDIR=$(CXC_PATH)/bin/lib_ext

## Source directories. Here you can list your source directories. Example: SRCDIR src1 src2 src3 (use spaces)
SRCDIRS ?= src

## Include files
CINCLUDES += -I$(INCDIR)
#CINCLUDES += -I"$(ACS_ROOT)/tra_cnz/traapi_caa/inc"
CINCLUDES += -I"$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/cc/inc"
CINCLUDES += -I"$(INCDIRHAAPPMNGR)"
CINCLUDES += -I"$(INCDIRHAEAPI)"
CINCLUDES += -I"$(INCDIRACSEXT)"
CINCLUDES += -I"$(INCDIRACSINT)"
CINCLUDES += -I"$(INCDIROMINT)"
CINCLUDES += -I"$(INCDIROMEXT)"
CINCLUDES += -I"$(INCDIRCCINT)"
CINCLUDES += -I"$(INCDIRCCEXT)"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(SEC_SDK_INC)"
CINCLUDES += -I"$(LOTC_SDK_INC)"
CINCLUDES += -I"$(BOOST_SDK_INC)"
CINCLUDES += -I"$(SEC_ACS_SDK_INC)"

## Source files
SRCFILES =	$(SRCDIRCC)/ACS_APGCC_Util.cpp \
	$(SRCDIROM)/acs_apgcc_adminoperationasync_V2.cpp \
	$(SRCDIROM)/acs_apgcc_adminoperationasync_V2_impl.cpp \
	$(SRCDIRCC)/ACS_APGCC_ManagerDaemons.cpp \
	$(SRCDIRCC)/ACS_APGCC_Daemon.cpp \
	$(SRCDIRCC)/ACS_APGCC_DSD.cpp \
	$(SRCDIRCC)/ACS_APGCC_Directory.cpp \
	$(SRCDIRCC)/ACS_APGCC_Event.cpp \
	$(SRCDIRCC)/ACS_APGCC_FileStream.cpp \
	$(SRCDIRCC)/ACS_APGCC_InitFile.cpp \
	$(SRCDIRCC)/ACS_APGCC_LockFile.cpp \
	$(SRCDIRCC)/ACS_APGCC_PIPE_R1.cpp \
	$(SRCDIRCC)/ACS_APGCC_Path.cpp \
	$(SRCDIRCC)/ACS_APGCC_Command.cpp \
	$(SRCDIRCC)/ACS_APGCC_TempFile.cpp \
	$(SRCDIRCC)/ACS_APGCC_Time_Value.cpp \
	$(SRCDIRCC)/ACS_APGCC_CmdParams_R1.cpp \
	$(SRCDIROM)/OmHandler.cpp \
	$(SRCDIROM)/ACS_APGCC_OiHandler.cpp \
	$(SRCDIROM)/ACS_APGCC_ObjectImplementerInterface.cpp \
	$(SRCDIROM)/ACS_APGCC_RuntimeOwner.cpp \
	$(SRCDIROM)/ACS_APGCC_ObjectImplementer_EventHandler.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementerinterface_impl.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementer_eventhandler_impl.cpp \
	$(SRCDIROM)/acs_apgcc_runtimeowner_impl.cpp \
	$(SRCDIROM)/acs_apgcc_oihandler_impl.cpp \
	$(SRCDIROM)/acs_apgcc_omhandler_impl.cpp \
	$(SRCDIROM)/acs_apgcc_adminoperation.cpp \
	$(SRCDIROM)/acs_apgcc_adminoperation_impl.cpp \
	$(SRCDIROM)/acs_apgcc_adminoperationimplementer.cpp \
	$(SRCDIROM)/acs_apgcc_adminoperationimplementer_impl.cpp \
	$(SRCDIROM)/acs_apgcc_adminoperationasync.cpp \
	$(SRCDIROM)/acs_apgcc_adminoperationasync_impl.cpp \
	$(SRCDIROM)/acs_apgcc_paramhandling.cpp \
	$(SRCDIROM)/ACS_APGCC_RuntimeOwner_V2.cpp \
	$(SRCDIROM)/acs_apgcc_runtimeowner_impl_v2.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementerinterface_V2.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementerinterface_V2_impl.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementereventhandler_V2.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementereventhandler_V2_impl.cpp \
	$(SRCDIROM)/acs_apgcc_oihandler_V2.cpp \
	$(SRCDIROM)/acs_apgcc_oihandler_V2_impl.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementerinterface_V3.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementerinterface_V3_impl.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementereventhandler_V3.cpp \
	$(SRCDIROM)/acs_apgcc_objectimplementereventhandler_V3_impl.cpp \
	$(SRCDIROM)/acs_apgcc_oihandler_V3.cpp \
	$(SRCDIROM)/acs_apgcc_oihandler_V3_impl.cpp \
        $(SRCDIROM)/acs_apgcc_oihandler_V4.cpp \
        $(SRCDIROM)/acs_apgcc_oihandler_V4_impl.cpp \
	$(SRCDIRHAAPPMNGR)/ACS_APGCC_ApplicationManager.cpp \
	$(SRCDIRHAAPPMNGR)/APGCC_Amf_CC.cpp \
	$(SRCDIRHAEAPI)/APGCC_Service_Operation.cpp \
	$(SRCDIRACS)/ACS_APGCC_CommonLib.cpp \
	$(SRCDIRACS)/sha.cpp \
	$(SRCDIRACS)/twofish.cpp \
	$(SRCDIRACS)/ACS_APGCC_CommonUtil.cpp \
	$(SRCDIRACS)/ACS_APGCC_OpenSSLThreading.cpp \
	$(SRCDIRACS)/StdAfx.cpp

OBJFILES =	$(OBJDIR)/ACS_APGCC_CmdParams_R1.obj \
	$(OBJDIR)/acs_apgcc_adminoperationasync_V2.obj \
	$(OBJDIR)/acs_apgcc_adminoperationasync_V2_impl.obj \
	$(OBJDIR)/ACS_APGCC_ManagerDaemons.obj \
	$(OBJDIR)/ACS_APGCC_Daemon.obj \
	$(OBJDIR)/ACS_APGCC_Command.obj \
	$(OBJDIR)/ACS_APGCC_DSD.obj \
	$(OBJDIR)/ACS_APGCC_Directory.obj \
	$(OBJDIR)/ACS_APGCC_Event.obj \
	$(OBJDIR)/ACS_APGCC_FileStream.obj \
	$(OBJDIR)/ACS_APGCC_InitFile.obj \
	$(OBJDIR)/ACS_APGCC_LockFile.obj \
	$(OBJDIR)/ACS_APGCC_PIPE_R1.obj \
	$(OBJDIR)/ACS_APGCC_Path.obj \
	$(OBJDIR)/ACS_APGCC_TempFile.obj \
	$(OBJDIR)/ACS_APGCC_Time_Value.obj \
	$(OBJDIR)/ACS_APGCC_Util.obj \
	$(OBJDIR)/OmHandler.obj \
	$(OBJDIR)/ACS_APGCC_OiHandler.obj \
	$(OBJDIR)/ACS_APGCC_ObjectImplementerInterface.obj \
	$(OBJDIR)/acs_apgcc_objectimplementerinterface_impl.obj \
	$(OBJDIR)/acs_apgcc_objectimplementer_eventhandler_impl.obj \
	$(OBJDIR)/acs_apgcc_runtimeowner_impl.obj \
	$(OBJDIR)/acs_apgcc_oihandler_impl.obj \
	$(OBJDIR)/acs_apgcc_omhandler_impl.obj \
	$(OBJDIR)/acs_apgcc_adminoperation.obj \
	$(OBJDIR)/acs_apgcc_adminoperation_impl.obj \
	$(OBJDIR)/acs_apgcc_adminoperationimplementer.obj \
	$(OBJDIR)/acs_apgcc_adminoperationimplementer_impl.obj \
	$(OBJDIR)/acs_apgcc_adminoperationasync.obj \
	$(OBJDIR)/acs_apgcc_adminoperationasync_impl.obj \
	$(OBJDIR)/acs_apgcc_paramhandling.obj \
	$(OBJDIR)/ACS_APGCC_RuntimeOwner.obj \
	$(OBJDIR)/ACS_APGCC_RuntimeOwner_V2.obj \
	$(OBJDIR)/acs_apgcc_runtimeowner_impl_v2.obj \
	$(OBJDIR)/ACS_APGCC_ObjectImplementer_EventHandler.obj \
	$(OBJDIR)/acs_apgcc_objectimplementerinterface_V2.obj \
	$(OBJDIR)/acs_apgcc_objectimplementerinterface_V2_impl.obj \
	$(OBJDIR)/acs_apgcc_objectimplementereventhandler_V2.obj \
	$(OBJDIR)/acs_apgcc_objectimplementereventhandler_V2_impl.obj \
	$(OBJDIR)/acs_apgcc_oihandler_V2.obj \
	$(OBJDIR)/acs_apgcc_oihandler_V2_impl.obj \
	$(OBJDIR)/acs_apgcc_objectimplementerinterface_V3.obj \
	$(OBJDIR)/acs_apgcc_objectimplementerinterface_V3_impl.obj \
	$(OBJDIR)/acs_apgcc_objectimplementereventhandler_V3.obj \
	$(OBJDIR)/acs_apgcc_objectimplementereventhandler_V3_impl.obj \
	$(OBJDIR)/acs_apgcc_oihandler_V3.obj \
	$(OBJDIR)/acs_apgcc_oihandler_V3_impl.obj \
	$(OBJDIR)/ACS_APGCC_ApplicationManager.obj \
	$(OBJDIR)/APGCC_Amf_CC.obj \
	$(OBJDIR)/APGCC_Service_Operation.obj \
	$(OBJDIR)/ACS_APGCC_CommonLib.obj \
	$(OBJDIR)/sha.obj \
	$(OBJDIR)/twofish.obj \
	$(OBJDIR)/ACS_APGCC_CommonUtil.obj \
	$(OBJDIR)/ACS_APGCC_OpenSSLThreading.obj \
	$(OBJDIR)/StdAfx.obj

## Setting the so library compiler information
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

## here you can add own compiler flags
CFLAGS += -O3 -Wno-ignored-qualifiers -std=c++11 -Wno-unused-local-typedefs

# here you can add own linker flags
LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

# here you can add own libs
LIBS += -lrt -lpthread -lACE $(COREMW_LIBS) -lacs_prc -lacs_tra -lacs_dsd -lacs_aeh -lsec_credu_api -lsec_crypto_api -lsec_acs_api -lldap -lboost_serialization -lboost_filesystem -lpam

# here you can add own lib paths
#LIBSDIR += -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin
#LIBSDIR += -L$(ACS_ROOT)/pha_cnz/pha_cxc/bin
LIBSDIR += -L"$(CXC_PATH)/bin"
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(ACE_SDK_LIB)" 
LIBSDIR += -L"$(SEC_SDK_LIB)"
LIBSDIR += -L"$(SEC_CREDU_SDK_LIB)"
LIBSDIR += -L"$(LOTC_SDK_LIB)"
LIBSDIR += -L"$(COREMW_SDK_LIB)"
LIBSDIR += -L"$(BOOST_SDK_LIB)"
LIBSDIR += -L"$(SEC_ACS_SDK_LIB)"

## Object files
OBJ =	$(SRCFILES:%.cpp=%.obj) \
			$(SRCFILES:%.c=%.obj)

VPATH += $(SRCDIR) $(OUTDIR) $(LIB_EXT) $(INCDIR) $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
#all:	$(LIB_EXT)/$(LIB_FILENAME)
all:	$(OUTDIR)/$(LIB_FILENAME)

#$(LIB_EXT)/$(LIB_FILENAME): $(OBJ)
$(OUTDIR)/$(LIB_FILENAME): $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(PROD_FILENAME)'
#	$(SILENT)$(CC) $(CFLAGS) -o $(LIB_EXT)/$(LIB_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(LIB_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LIB_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
distclean: clean
	#$(RM) -r $(LIB_EXT)/$(LIB_FILENAME)
	$(RM) -r $(OUTDIR)/$(LIB_FILENAME)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	#$(CP) $(LIB_EXT)/$(LIB_FILENAME) $(AP_SDK_LIB)/.
	$(CP) $(OUTDIR)/$(LIB_FILENAME) $(AP_SDK_LIB)/.
#	$(call ct_mkelem,$(AP_SDK_LIB)/$(LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_FILENAME),$(LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_SONAME),$(LIB_LINKER_NAME))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'


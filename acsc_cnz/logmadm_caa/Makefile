## **********************************************************************
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
# Rev        Date         Name     		 What
# -----      -------      --------  	--------------------------
# PA1	    25/11/2010	 Initial 		Release                                                       
# **********************************************************************

CNZ_NAME ?= acsc_cnz
CAA_NAME ?= logmadm_caa
CXC_NAME ?= logm_cxc/

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

BLOCKDIR = $(CURDIR)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(CAA_PATH)/obj
SRCDIR   = $(CAA_PATH)/src
INCDIR   = $(CAA_PATH)/inc

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DLOGMNODESTATUS -DACE_NTRACE 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(COREMW_SDK_INC) -I$(ACE_SDK_INC)
CINCLUDES += -I$(INCDIR)  

LDFLAGS  = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64

LIBSDIR += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(SEC_SDK_LIB) 
LIBSDIR += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)

LIBS += -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf
LIBS += -lpthread -lboost_serialization -lboost_filesystem -lboost_system -lsec_crypto_api
LIBS += -lacs_apgcc -llog4cplus -lacs_tra -lacs_prc -lacs_aeh -lacs_dsd -lacs_csapi 

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) 

## Source files
#

SRCFILES = acs_logm_cleanupAction.cpp acs_logm_base.cpp acs_logm_housekeeping.cpp acs_logm_directory.cpp acs_logm_logfile.cpp \
	  acs_logm_apgccwrapper.cpp acs_logm_configurationdata.cpp acs_logm_time.cpp acs_logm_filetime.cpp \
	  acs_logm_apglogmFactory.cpp acs_logm_logmservice.cpp acs_logm_main.cpp acs_logm_tra.cpp  acs_logm_scxlogfile.cpp acs_logm_cmxlogfile.cpp \
	  acs_logm_smxlogfile.cpp \


IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(CURDIR)/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) \



## Build instructions

APNAME = acs_logmaintd
APNAME_DBG = acs_logmaintd_dbg

APEXE = $(OUTDIR)/$(APNAME)

.PHONY: all
all: $(APEXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(APEXE)'
		$(SILENT)$(RM) $(APEXE)
		$(SILENT)$(RM) $(OUTDIR)/.debug/$(APNAME_DBG)


$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*.obj) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(APNAME))	
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    

	
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

#DO NOT DELETE THIS LINE -- make depend needs it

# **********************************************************************
#
# Short description:
# Makefile template for XXX library
# **********************************************************************
#
# Ericsson AB 2012 All rights reserved.
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
#   A        2013-05-18   XLANSRI   Created
# **********************************************************************

CNZ_NAME ?= hcs_cnz
CAA_NAME ?= hcsadm_caa
CXC_NAME ?= hcs_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 


NTACS = $(ACS_ROOT)
CXC_PATH   = $(NTACS)/hcs_cnz/hcs_cxc

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
SRCAPIDIR = $(BLOCKDIR)/../hcsapi_caa/src
INCDIR   = $(BLOCKDIR)/inc
HCAPIINCDIR = $(BLOCKDIR)/../hcsapi_caa/inc_int 

# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DacaDEBUG 
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  
CFLAGS += -DACS_HC_HAS_LOGGING

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

#CINCLUDES +=-I$(INCDIR)  -I$(HCAPIINCDIR) -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)
CINCLUDES +=-I$(INCDIR)  -I$(HCAPIINCDIR) -I$(LIB_XERCES_SDK_INC) -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)
#-I$(LIB_XERCES_SDK_INC) 

## # here you can add own Include paths and/or other includes 
LDFLAGS      += 

LIBSDIR +=   -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AP_SDK_LIB) -L$(LIB_XERCES_SDK_LIB) -L$(ACE_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(COREMW_SDK_LIB) -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB)  -L$(SEC_ACS_SDK_LIB)
#-L$(LIB_XERCES_SDK_LIB) 
#-L/home/tcssril/xerces/xerces-c-3.1.1-x86_64-linux-gcc-3.4/lib64
#-L/vobs/IO_Developments/3PP_SDK/libXerces 
# -L$(AP_SDK_LIB)

## # here you can add own libs 
LIBS += -lrt -lpthread -lm -lACE -lboost_serialization -lboost_filesystem -lboost_system -lboost_thread -lacs_aeh -laes_gcc -laes_ohi -laes_afp -laes_dbo  -lsec_crypto_api -laes_ohi -lacs_prc
LIBS += $(COREMW_LIBS)
LIBS += $(SEC_LIBS)
LIBS += $(SEC_CREDU_LIBS)

ACS_HC_LIBS = $(LIBS) -lacs_tra -lacs_apgcc -laes_ohi -lxerces-c -lacs_dsd -lacs_csapi -lcap

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) $(SRCAPIDIR) $(OBJDIR) 

SRCFILES = acs_hcs_cache.cpp\
acs_hcs_progressReport_objectImpl.cpp\
acs_hcs_progressReportHandler.cpp\
acs_hcs_calenderScheduling.cpp\
acs_hcs_inotify.cpp\
acs_hcs_commandclient.cpp\
acs_hcs_commandclientmml.cpp\
acs_hcs_configuration.cpp\
acs_hcs_directory.cpp\
acs_hcs_documentprintout.cpp\
acs_hcs_documentxml.cpp\
acs_hcs_environment.cpp\
acs_hcs_exception.cpp\
acs_hcs_healthcheckservice.cpp\
acs_hcs_jobexecutor.cpp\
acs_hcs_printoutparser.cpp\
acs_hcs_formattertxt.cpp\
acs_hcs_printoutparsermml.cpp\
acs_hcs_printoutparsermmlallip.cpp\
acs_hcs_synchron.cpp\
acs_hcs_reportgenerator.cpp\
acs_hcs_properties.cpp\
acs_hcs_rulechecker.cpp\
acs_hcs_scheduler.cpp\
acs_hcs_timeutils.cpp\
acs_hcs_transferqueue.cpp\
acs_hcs_logger.cpp\
acs_hcs_jobScheduler_objectImpl.cpp\
acs_hcs_singleEvent_objectImpl.cpp\
acs_hcs_periodicEvent_objectImpl.cpp\
acs_hcs_calenderPeriodicEvent_objectImpl.cpp\
acs_hcs_inUse_objectImpl.cpp\
acs_hcs_readyToUse_objectImpl.cpp\
acs_hcs_jobbased_objectImpl.cpp\
acs_hcs_haservice.cpp\
acs_hcs_global_ObjectImpl.cpp\
acs_hcs_jobscheduler.cpp\
acs_hcs_periodicscheduling.cpp\
acs_hcs_tra.cpp\
acs_hcs_brfinterface.cpp\

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files

OBJFILES =	$(SRCFILES:%.cpp=%.obj) 

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC

ACS_HC = acs_hcd
ACS_HC_DBG = acs_hcd_dbg
ACS_HC_EXE = $(OUTDIR)/$(ACS_HC)

ACS_HC_OBJFILES = $(SRCFILES:%.cpp=%.obj) 

DEPS := $(ACS_HC_OBJFILES:.obj=.d) 

#ifneq ($(MAKECMDGOALS),clean)
#-include $(DEPS)
#endif

.PHONY: all

all: $(ACS_HC_EXE)
	
$(ACS_HC_EXE): $(ACS_HC_OBJFILES)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building target: $@'
	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
	#$(CC) -o $(ACS_HC_EXE) $(wildcard $(OBJDIR)/*.obj) $(LIBSDIR) $(ACS_HC_LIBS)
	$(CC) -o $(ACS_HC_EXE) $(OBJDIR)/*.obj $(LIBSDIR) $(ACS_HC_LIBS)
	$(call stripp,$(ACS_HC))
	$(SILENT)$(ECHO) 'Finished building target: $@'
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:	
	$(SILENT)$(ECHO) 'Cleaning object files '
	$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
	$(SILENT)$(ECHO) 'Cleaning file: $(ACS_HC_EXE)' 
	$(SILENT)$(RM) $(ACS_HC_EXE) 

	


metrics : cccc lint

.PHONY: cccc
cccc:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
	$(CCCC) $(wildcard $(BLOCKDIR)/inc/*)
	$(CCCC) $(wildcard $(BLOCKDIR)/src/*)
	$(SEPARATOR_STR)


lint:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'LINT file: $@'
#               $(LINT) $(wildcard $(BLOCKDIR)/inc/*)
	$(LINT) $(wildcard $(BLOCKDIR)/src/*)
	$(SEPARATOR_STR)




.PHONY: depend
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

	


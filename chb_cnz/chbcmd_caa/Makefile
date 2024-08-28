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
# PA1		 25/11/2010		XTANAGG	Initial Release                                                       
# **********************************************************************

#CXC_NR = CXC120333
CNZ_NAME ?= chb_cnz
CAA_NAME ?= chbcmd_caa
CXC_NAME ?= chb_cxc

include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(ACS_ROOT)/$(CNZ_NAME)/$(CAA_NAME)
OUTDIR   = $(CXCDIR)/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INCDIR   = $(CURDIR)/inc
CHBAPISRCDIR = $(CURDIR)/../chbapi_caa/src
CHBAPIINTINCDIR = $(CURDIR)/../chbapi_caa/inc_int
CHBAPIEXTINCDIR = $(CURDIR)/../chbapi_caa/inc_ext
CHBADMINCDIR = $(CURDIR)/../chbadm_caa/inc
CHBADMSRCDIR = $(CURDIR)/../chbadm_caa/src

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(CHBAPIINTINCDIR) -I$(CHBAPIEXTINCDIR) -I$(CHBADMINCDIR)

LIBSDIR +=  -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)

## # here you can add own libs 
LIBS +=  -lrt -lpthread -lacs_apgcc -lacs_tra -llog4cplus -lacs_prc -lacs_dsd -lacs_csapi -lacs_aeh

CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

#BOOST
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBS += -lboost_serialization -lboost_filesystem -lboost_system

#SEC SDK
LIBSDIR += -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB)
LIBS += -lsec_crypto_api -lsec_credu_api

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) 

## Source files
#

SRCFILES = acs_chb_mtzln.cpp $(CHBAPISRCDIR)/acs_chb_file_handler.cpp \
	   $(CHBAPISRCDIR)/acs_chb_ntzone.cpp 
	   

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(CURDIR)/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) \

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = mtzln
APNAME_DBG = mtzln_dbg

APEXE = $(OUTDIR)/$(APNAME)
APEXE_DBG = $(OUTDIR)/.debug/$(APNAME_DBG)

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
		$(SILENT)$(RM) $(APEXE_DBG)

		
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

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '******     NOT NEEDED      *******'
	$(SILENT)$(ECHO) '**********************************'

#DO NOT DELETE THIS LINE -- make depend needs it


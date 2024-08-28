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
#            2012-05-11   XSAMECH   Created
#			 2014-05-16   XSAMECH   Updated for new files.
#                                                            
# **********************************************************************
CNZ_NAME ?= swr_cnz
CAA_NAME ?= swrcmd_caa
CXC_NAME ?= swr_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

LIBSDIR += -L$(ACE_SDK_LIB)

CINCLUDES += -I$(ACE_SDK_INC)

LIBSDIR += -L$(COREMW_SDK_LIB)

CINCLUDES += -I$(COREMW_SDK_INC)

LIBS += $(COREMW_LIBS)
#SEC_LIBS = -lsec_crypto_api
# SEC libraries
#LIBS += $(SEC_LIBS)

BLOCKDIR = $(ACS_ROOT)/$(CNZ_NAME)/$(CAA_NAME)/../swrcmd_caa/
OUTDIR   = $(CXCDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
SWRADMINCDIR= $(CURDIR)/../swradm_caa/inc
TESTDIR  = $(BLOCKDIR)/test

# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE
CFLAGS = -std=c++11  
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES+= -I$(INCDIR) -I$(SWRADMINCDIR) -I$(ACS_ROOT)/swr_cnz/swrcmd_caa/inc 
 

## # here you can add own Include paths and/or other includes
#LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 

## # here you can add own Include paths and/or other includes
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 

CINCLUDES += -I$(SEC_CREDU_SDK_INC)
CINCLUDES += -I$(SEC_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC) 
CINCLUDES += -I$(SEC_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(AP_SDK_INC)

#LIBSDIR +=  -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin/

LIBSDIR += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) 
LIBSDIR += -L$(SEC_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(AP_SDK_LIB)

LDFLAGS += -Wl,-rpath-link,$(ACE_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(COREMW_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)

## # here you can add own libs 

# SEC libraries

#LIBS += -lACE  -lacs_apgcc  

LIBS += -lACE -lacs_apgcc -lacs_tra -lacs_aeh -lacs_dsd -lsec_crypto_api -lsec_credu_api -lboost_serialization -lboost_filesystem -lboost_system -lacs_prc -llog4cplus -lacs_csapi

RPSWRLIBS += -lgcc_s -lstdc++ -lrt -lpthread

## here you can add own File paths
VPATH +=	$(SRCDIR)  $(OBJDIR) $(OUTDIR) $(NTACS)/swr_cnz/swradm_caa/src/

## Source files
#
SRCFILES = acs_swr_global.cpp swr_Swrprint.cpp 
RPSWRSRCFILES = rpsw_rpSwrprint.cpp 

		     
IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

RPSWROBJ = \
		$(RPSWRSRCFILES:%.cpp=%.obj)
## Build instructions
SWRPRINT_APNAME = swrprint

SWRPRINT_APNAME_DBG = swrprint_dbg

SWRPRINT_APEXE = $(OUTDIR)/$(SWRPRINT_APNAME)

SWRPRINT_OBJDIR =$(OBJDIR)/acs_swr_global.obj $(OBJDIR)/swr_Swrprint.obj 

## Build instructions
RPSWRPRINT_APNAME = rpswrprint

RPSWRPRINT_APNAME_DBG = rpswrprint_dbg

RPSWRPRINT_APEXE = $(OUTDIR)/$(RPSWRPRINT_APNAME)

RPSWRPRINT_OBJDIR =$(OBJDIR)/rpsw_rpSwrprint.obj

#CFLAGS-rpsw_rpSwrprint.obj = -std=c++11  # adding cpp11 compile option only for rpswrprint 

.PHONY: all
#all: $(OUTDIR)/$(SWRPRINT_APNAME)
all: $(OUTDIR)/$(SWRPRINT_APNAME) $(OUTDIR)/$(RPSWRPRINT_APNAME) 
		 

.PHONY: distclean
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(SWRPRINT_APNAME)
		$(RM) $(OUTDIR)/$(RPSWRPRINT_APNAME)
		$(RM) $(OUTDIR)/.debug/$(SWRPRINT_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(RPSWRPRINT_APNAME_DBG)
		
.PHONY: $(SWRPRINT_APNAME)
$(SWRPRINT_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(SWRPRINT_APEXE) $(SWRPRINT_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(SWRPRINT_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: $(RPSWRPRINT_APNAME)
$(RPSWRPRINT_APEXE): $(RPSWROBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) $(CFLAGS) -o $(RPSWRPRINT_APEXE) $(RPSWRPRINT_OBJDIR) -L$(AP_SDK_LIB)-L$(COREMW_SDK_LIB) $(RPSWRLIBS)
	    $(call stripp,$(RPSWRPRINT_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: metrics
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
#		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

				
	
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


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
#            2010-11-23   tcsvenc   Created
#                                                            
# **********************************************************************

#CXC_NR = CXC120333

CNZ_PATH = $(ACS_ROOT)/nsf_cnz
CNZ_NAME ?= nsf_cnz
CAA_NAME ?= nsfcmd_caa
CXC_NAME ?= nsf_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(CAA_PATH)/obj
SRCDIR   = $(CAA_PATH)/src
INCDIR   = $(CAA_PATH)/inc
TESTDIR  = $(CAA_PATH)/test

#export DX_CUSTOM_RPATH=${COREMW_SDK_LIB}:${COREMW_SDK_LIB}/opensaf:${AP_SDK_LIB}:$(ACE_SDK_PATH)/ace:$(ACS_ROOT)/tra_cnz/tra_cxc/bin

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  -DNONODESTATEPRC

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR) -I$(ACS_ROOT)/nsf_cnz/nsfadm_caa/inc 

LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 

# here you can add own lib paths
LIBSDIR += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)

## # here you can add own libs 
LIBS += -lacs_apgcc  -lacs_prc -lacs_csapi  -lacs_tra -llog4cplus

## here you can add own File paths
#VPATH =	$(BLOCKDIR)/src/ifu $(BLOCKDIR)/src/swu $(OUTDIR)
VPATH +=	$(SRCDIR) $(OUTDIR) $(CNZ_PATH)/nsfadm_caa/src 

## Source files

SRCFILES = acs_nsf_command.cpp acs_nsf_common.cpp

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
NSFSTART_APNAME = nsfstart
NSFSTOP_APNAME =  nsfstop
NSFRESET_APNAME = nsfreset
NSFSTART_APNAME_DBG = nsfstart_dbg
NSFSTOP_APNAME_DBG =  nsfstop_dbg
NSFRESET_APNAME_DBG = nsfreset_dbg

NSFSTART_APEXE = $(OUTDIR)/$(NSFSTART_APNAME)
NSFSTOP_APEXE  = $(OUTDIR)/$(NSFSTOP_APNAME)
NSFRESET_APEXE = $(OUTDIR)/$(NSFRESET_APNAME)

#.PHONY: depend clean
.PHONY: clean

 
all: $(NSFSTART_APEXE) \
     $(NSFSTOP_APEXE)  \
     $(NSFRESET_APEXE)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

$(NSFSTART_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(NSFSTART_APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(NSFSTART_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(NSFSTOP_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(NSFSTOP_APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(NSFSTOP_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(NSFRESET_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(NSFRESET_APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(NSFRESET_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

# DO NOT DELETE THIS LINE -- make depend needs it


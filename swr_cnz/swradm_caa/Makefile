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
#                                                            
# **********************************************************************

CNZ_NAME ?= swr_cnz
CAA_NAME ?= swradm_caa
CXC_NAME ?= swr_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

BLOCKDIR = $(CAA_PATH)/../swradm_caa/
OUTDIR   = $(CXCDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
SWRCMDCDIR= $(CAA_PATH)/../swrcmd_caa/inc
TESTDIR  = $(BLOCKDIR)/test

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES+= -I$(INCDIR) -I$(SWRCMDCDIR) -I$(ACS_ROOT)/swr_cnz/swradm_caa/inc  

## # here you can add own Include paths and/or other includes
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 

#LIBSDIR +=  -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin/
LIBSDIR += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) 

## # here you can add own libs 

LIBS += -lACE  -lacs_apgcc -lacs_tra -lacs_dsd -lacs_prc -lacs_jtp -llog4cplus

## here you can add own File paths
VPATH +=	$(SRCDIR)  $(OBJDIR) $(OUTDIR) $(ACS_ROOT)/swr_cnz/swrcmd_caa/src/

## Source files
#
SRCFILES = swr_Swrprint.cpp 

		     
IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

## Build instructions
SWRPRINT_APNAME = swrprint

SWRPRINT_APEXE = $(OUTDIR)/$(SWRPRINT_APNAME)

SWRTESTACT_OBJDIR = $(OBJDIR)/swr_Swrprint.obj    

.PHONY: all
#all: $(OUTDIR)/$(SWRPRINT_APNAME) 
		 

.PHONY: distclean
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(SWRPRINT_APNAME)
		

#$(SWRPRINT_APEXE): $(OBJ) $(METRICS)
#	    $(NEW_LINE)
#	    $(SEPARATOR_STR)
#		$(SILENT)$(ECHO) 'Building target: $@'
#	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
#	    $(CC) -o $(SWRPRINT_APEXE) $(SWRPRINT_OBJDIR) $(LIBSDIR) $(LIBS)
#	    $(call stripp,$(SWRPRINT_APNAME))
#	    $(SILENT)$(ECHO) 'Finished building target: $@'
#	    $(SEPARATOR_STR)
#	    $(NEW_LINE)

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


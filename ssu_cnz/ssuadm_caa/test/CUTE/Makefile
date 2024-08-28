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
#            2010-02-19   teiggam   Created
#                                                            
# **********************************************************************

#CXC_NR = CXC120333

CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(ACS_ROOT)/ssu_cnz/ssuadm_caa/
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test
CUTESRCDIR = $(TESTDIR)/CUTE/src
OBJDIR = $(TESTDIR)/CUTE/obj
OUTDIR = $(TESTDIR)/CUTE/bin

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
#Use ACS_SSU_CUTE_TEST only for CUTE test
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DSSUDEBUG -DACS_SSU_CUTE_TEST  -fprofile-arcs -ftest-coverage
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES=  -I$(AP_SDK_INC)  -I$(ACE_SDK_PATH)  -I$(LOTC_SDK_INC) \
-I$(LOTC_SDK_INC)/c++/4.3 -I$(LOTC_SDK_INC)/c++/4.3/x86_64-suse-linux \
-I$(COREMW_SDK_INC) -I$(ACS_ROOT)/ssu_cnz/ssuadm_caa/inc \
-I$(CUTESRCDIR)/../cute -I/home1/sdk -I$(CURDIR) \
-I$(ACS_ROOT)/ssu_cnz/ssucmd_caa/inc -I$(ACS_ROOT)/ssu_cnz/ssuadm_caa/test/CUTE/src

## # here you can add own Include paths and/or other includes -- Modified by SirishKumar
#CPPFLAGS     = -I$(ACE_ROOT) -I$(ACE_ROOT)/ace -I$(LOTCROOT)/usr/include -I$(LOTCROOT)/usr/include/c++/4.1.2/ -I$(LOTCROOT)/usr/include/c++/4.1.2/x86_64-suse-linux/ -I$(SSUROOT)/ssumonitor_caa/inc/
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 -lgcov

# EDITED BY Sirish
LIBSDIR += -L$(AP_SDK_LIB)/ -L$(LIB_LOGCPLUS_SDK_LIB)

## # here you can add own libs 
LIBS += -lACE -lacs_apgcc -lgcov -lacs_tra -llog4cplus -lacs_prc -lacs_csapi -lacs_aeh

## here you can add own File paths
#VPATH =	$(BLOCKDIR)/src/ifu $(BLOCKDIR)/src/swu $(OUTDIR)
VPATH +=	$(SRCDIR) $(OUTDIR)  $(CUTESRCDIR) $(CUTESRCDIR)/../cute $(ACS_ROOT)/ssu_cnz/ssucmd_caa/src

## Source files
#

SRCFILES = Test.cpp acs_ssu_svc_loader.cpp acs_ssu_disk_mon.cpp \
	acs_ssu_aeh.cpp \
	acs_ssu_perf_mon.cpp acs_ssu_param.cpp acs_ssu_file_handle_counter.cpp \
	acs_ssu_folder_quota_mon.cpp acs_ssu_folder_quota.cpp acs_ssu_path.cpp \
	acs_ssu_execute.cpp acs_ssu_root_ObjectImpl.cpp acs_ssu_command_handler.cpp \
	acs_ssu_memory_counter.cpp \
	acs_ssu_counter.cpp acs_ssu_file_handle_counter.cpp acs_ssu_memory_counter.cpp \
	acs_ssu_path.cpp acs_ssu_alarm_handler.cpp  Cute_SSU_FolderQuota.cpp \
	Cute_SSU_FolderQuotaMon.cpp Cute_SSU_DiskMon.cpp Cute_SSU_Monitor_Service.cpp \
	Cute_SSU_AlarmHandler.cpp Cute_SSU_Memory_Counter.cpp \
	Cute_SSU_FileHandle_Counter.cpp Cute_SSU_Common.cpp

#acs_ssu_perf_mon.cpp acs_ssu_counter.cpp acs_ssu_file_handle_counter.cpp Cute_SSU_FileHandle_Counter.cpp 

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
APNAME = CUTE_SSUMONITORSERVICE_CXC

APEXE = $(OUTDIR)/$(APNAME)

#.PHONY: depend clean
.PHONY: clean

 
all: $(APEXE)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*.obj) $(LIBSDIR) $(LIBS) $(CINCLUDES) $(LDFLAGS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

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

				
	

depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


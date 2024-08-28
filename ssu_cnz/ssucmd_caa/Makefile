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
#            2010-11-24   xnadnar   Created
#                                                            
# **********************************************************************

COMMON_PATH = ../../../../../vobs/IO_Developments/Tools/linux_conf_file
include $(COMMON_PATH)/common.mk

BLOCKDIR = $(CURDIR)/../ssucmd_caa/
OUTDIR   = $(CXCDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
SSUADMINCDIR= $(CURDIR)/../ssuadm_caa/inc
TESTDIR  = $(BLOCKDIR)/test
NTACS = /vobs/cm4ap/ntacs


# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DSSUDEBUG -DSSUNODEFAILURE -g
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE 
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES+= -I$(INCDIR) -I$(SSUADMINCDIR)

## # here you can add own Include paths and/or other includes
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 

#LIBSDIR +=  -L/vobs/cm4ap/ntacs/tra_cnz/tra_cxc/bin/
LIBSDIR += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)

## # here you can add own libs 

LIBS += -lacs_apgcc -lacs_prc -lacs_tra -llog4cplus

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) $(NTACS)/ssu_cnz/ssuadm_caa/src/

## Source files
#
SRCFILES =  acs_ssu_param.cpp acs_ssu_execute.cpp acs_ssu_ssucfg.cpp acs_ssu_ssuls.cpp

		     
IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 



## Build instructions
SSUCFG_APNAME = ssucfg
SSULS_APNAME = ssuls
SSUCFG_APNAME_DBG = ssucfg_dbg
SSULS_APNAME_DBG = ssuls_dbg


SSUCFG_APEXE = $(OUTDIR)/$(SSUCFG_APNAME)

SSULS_APEXE = $(OUTDIR)/$(SSULS_APNAME)

SSUCFG_OBJDIR = $(OBJDIR)/acs_ssu_param.obj $(OBJDIR)/acs_ssu_execute.obj $(OBJDIR)/acs_ssu_ssucfg.obj
SSULS_OBJDIR = $(OBJDIR)/acs_ssu_param.obj  $(OBJDIR)/acs_ssu_execute.obj $(OBJDIR)/acs_ssu_ssuls.obj

 
.PHONY: all
all: $(OUTDIR)/$(SSUCFG_APNAME)\
	 $(OUTDIR)/$(SSULS_APNAME)

.PHONY: prebuild
prebuild:
	$(call ct_co,$(OUTDIR)/$(SSUCFG_APNAME))
	$(call ct_co,$(OUTDIR)/$(SSULS_APNAME))
	$(call ct_co,$(OUTDIR)/.debug/$(SSUCFG_APNAME_DBG))
	$(call ct_co,$(OUTDIR)/.debug/$(SSULS_APNAME_DBG))

.PHONY: postbuild
postbuild:
	$(call ct_ci,$(OUTDIR)/$(SSUCFG_APNAME))
	$(call ct_ci,$(OUTDIR)/$(SSULS_APNAME))
	$(call ct_ci,$(OUTDIR)/.debug/$(SSUCFG_APNAME_DBG))
	$(call ct_ci,$(OUTDIR)/.debug/$(SSULS_APNAME_DBG))

.PHONY: undobuild
undobuild:
	$(call ct_unco,$(OUTDIR)/$(SSUCFG_APNAME))
	$(call ct_unco,$(OUTDIR)/$(SSULS_APNAME))
	$(call ct_unco,$(OUTDIR)/.debug/$(SSUCFG_APNAME_DBG))
	$(call ct_unco,$(OUTDIR)/.debug/$(SSULS_APNAME_DBG))
	 

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT) rm -f $(wildcard $(OBJDIR)/*)
		

.PHONY: distclean
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(SSUCFG_APNAME)
		$(RM) $(OUTDIR)/$(SSULS_APNAME)
		$(RM) $(OUTDIR)/.debug/$(SSUCFG_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(SSULS_APNAME_DBG)

$(SSUCFG_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(SSUCFG_APEXE) $(SSUCFG_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(SSUCFG_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(SSULS_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(SSULS_APEXE) $(SSULS_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(SSULS_APNAME))
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

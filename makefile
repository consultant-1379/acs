# **********************************************************************
#
# Short description:
# Makefile template for XXX library
# **********************************************************************
#
# ï¿½ Ericsson AB 2010 All rights reserved.
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
#            2010-06-29   teiggam   Created
#                                                            
# **********************************************************************


# command line usage
TARGET ?= all

#CURDIR = $(shell pwd)
IO_DEV = /vobs/IO_Developments/Tools/linux_conf_file
COMMON_MK = $(IO_DEV)/common.mk
export COMMON_MK

include $(COMMON_MK)

CXC_DIRS =	apgcc_cnz/apgccapi_cxc\
		tra_cnz/tra_cxc\
		ssu_cnz/ssu_cxc\
		apjtp_cnz/apjtp_cxc\
		dsd_cnz/dsd_cxc\
		pha_cnz/pha_cxc\
		prc_cnz/prc_cxc

subsystems: 
		@for i in $(CXC_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		 echo "*******************" ; \
		 echo "* $$i" ; \
		 echo "*******************" ; \
		 echo cd $$i ; \
		 cd $$i ; \
		 $(MAKE) $(MFLAGS) all) ;\
		 echo "+++++++++++++++++" ; \
		done
        
print_build_envs:
	$(SILENT)$(ECHO) ' Build Verbose is $(V)'
	$(SILENT)$(ECHO) ' Build Verbose is $(SILENT)'
	$(SILENT)$(ECHO) ' Build Verbose is $(quiet)'
	$(SILENT)$(SEPARATOR_STR)
	$(SILENT)$(ECHO) $(AS)	
	$(SILENT)$(ECHO) $(LD)	
	$(SILENT)$(ECHO) $(CC)	
	$(SILENT)$(ECHO) $(CPP)	
	$(SILENT)$(ECHO) $(NM)	
	$(SILENT)$(ECHO) $(STRIP)	
	$(SILENT)$(ECHO) $(OBJCOPY)	
	$(SILENT)$(ECHO) $(OBJDUMP)
	$(SILENT)$(ECHO) $(AWK)	
	$(SILENT)$(ECHO) $(PERL)	
	$(SILENT)$(ECHO) $(AS)	
	$(SILENT)$(ECHO) $(RM)	
	$(SILENT)$(ECHO) $(TOUCH)	
	$(SILENT)$(ECHO) $(CHECK)	
	$(SILENT)$(ECHO) $(LINT)	
	$(SILENT)$(ECHO) $(DOXYGEN)	
	$(SILENT)$(ECHO) $(CCCC)	
	$(SILENT)$(ECHO) $(SLOCCOUNT)	
	$(SILENT)$(ECHO) $(EU_READELF)
	$(SILENT)$(ECHO) $(CLEARTOOL)	
	$(SILENT)$(ECHO) $(SDPTOOL)	
	$(SILENT)$(ECHO) $(CONFIG_SHELL)
	$(SILENT)$(ECHO) $(PWD)	
	$(SILENT)$(SEPARATOR_STR)
	
debug: 	print_build_envs
	$(SILENT)$(ECHO) $(srctree)         
	$(SILENT)$(ECHO) $(objtree)         
	$(SILENT)$(ECHO) $(src)            
	$(SILENT)$(ECHO) $(obj)
	$(SILENT)$(ECHO) $(VPATH)
#	$(SILENT)$(ECHO) $(CURDIR)
	
all: subsystems


documentation:
	$(SILENT)$(ECHO) ' Removing $(DOCGEN_DIR) directory'
	$(SILENT)$(RM) -r -f $(DOCGEN_DIR)
	$(SILENT)$(ECHO) ' Creating $(DOCGEN_DIR) directory'
	$(SILENT)mkdir $(DOCGEN_DIR)
	$(SILENT)$(DOXYGEN) $(DOXYGENFILECONFIG) 
	

metrics:
		@for i in $(SUBSYSTEM_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			 echo "cd $$i" ; \
			 cd $$i ; \
			 $(MAKE) $(MFLAGS) metrics ) ;\
			 echo "+++++++++++++++++" ; \
		done	
		
clean:
		@for i in $(SUBSYSTEM_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			 echo "cd $$i" ; \
			 cd $$i ; \
			 $(MAKE) $(MFLAGS) clean ) ;\
			 echo "+++++++++++++++++" ; \
		done

help:
		$(SILENT)$(ECHO)  'Cleaning targets:'
		$(SILENT)$(ECHO)  '  clean           - Remove most generated files but keep the config and'
		$(NEW_LINE)
		$(SILENT)$(ECHO)  'Other generic targets:'
		$(SILENT)$(ECHO)  '  all             - Build all targets'
		$(SILENT)$(ECHO)  '  Execute "make" or "make all" to build all targets '
		$(NEW_LINE)
		$(SILENT)$(ECHO)  '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
		$(SILENT)$(ECHO)  '  make V=2   [targets] 2 => give reason for rebuild of target'
		$(SILENT)$(ECHO)  '  make O=dir [targets] Locate all output files in "dir", including .config'
		$(SILENT)$(ECHO)  '  make C=1   [targets] Check all c source with $$CHECK (sparse by default)'
		$(SILENT)$(ECHO)  '  make C=2   [targets] Force check of all c source with $$CHECK'

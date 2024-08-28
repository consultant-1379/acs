# **********************************************************************
#
# Short description:
# Makefile template for Function Change Software development package Campaigner (FCSC) Service
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
# P1A        2010-08-26   teiggam   Created
#                                                            
# **********************************************************************

#CXA_NR = CXA120011
#CXC_NR = CXC120333

BLOCKNAMES_CAA_DIRS =	fcscadm_caa\
			fcsccmd_caa

.PHONY: ssuadmn_caa
blocks_caa:
		@for i in $(BLOCKNAMES_CAA_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			 echo "cd $$i" ; \
			 cd $$i ; \
			 $(MAKE) $(MFLAGS) all) ;\
			 echo "+++++++++++++++++" ; \
		done

## Build instructions
#

all: blocks_caa

clean:
		@for i in $(BLOCKNAMES_CAA_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			 echo "cd $$i" ; \
			 cd $$i ; \
			 $(MAKE) $(MFLAGS) clean ) ;\
			 echo "+++++++++++++++++" ; \
		done

depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^
		

# DO NOT DELETE THIS LINE -- make depend needs it


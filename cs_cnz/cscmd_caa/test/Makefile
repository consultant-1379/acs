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
#            2010-11-23   xmianon   Created
# **********************************************************************

include ../../cs_cxc/common.mk

.PHONY : cppcheck
cppcheck:
	$(SILENT) echo "Running cppcheck"
	$(SILENT) cppcheck `find .. -name "*.cpp"` | egrep "(error)|100% done"


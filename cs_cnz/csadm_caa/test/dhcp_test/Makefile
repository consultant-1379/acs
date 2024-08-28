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
#            2010-08-31   xmianon   Created
#            2010-10-18   xminaon   Removed absolute pathes
# **********************************************************************

include ../../cs_cxc/common.mk

CAA_NAME := cssrv_caa
CAA_PATH := $(CNZ_PATH)/$(CAA_NAME)

OBJDIR := ../obj

CINCLUDES += -I$(CAA_PATH)/inc
CINCLUDES += -I$(CNZ_PATH)/cscmd_caa/common/inc
#CINCLUDES += -I$(CAA_PATH)/inc
CINCLUDES += -I$(CNZ_PATH)/csprot_caa/inc
#CINCLUDES += -I$(CNZ_PATH)/csmodd_caa/inc
#CINCLUDES += -I$(IODEV_VOB)/ACE_SDK/ACE_wrappers/
#CINCLUDES += -I$(IODEV_VOB)/ACE_SDK/ACE_wrappers/ace
#CINCLUDES += -I$(IODEV_VOB)/ACE_SDK/ACE_wrappers/ASNMP

#LIBSDIR += -L$(CNZ_PATH)/cs_cxc/lib
#LIBS += -lasnmp -ldhcpctl -lomapi -ldst -lACE
LIBSDIR +=  -L/cc/IO_Developments/AP_SDK/Lib/

SNMP_SOURCES:= snmptest.cpp ACS_CS_Protocol.cpp ACS_CS_SNMPHandler.cpp ACS_CS_Thread.cpp \
ACS_CS_DHCP_Info.cpp ACS_CS_Trace.cpp ACS_CS_EventReporter.cpp ACS_CS_Event.cpp
SNMP_OBJECTS := $(patsubst %.cpp,%.obj,$(SNMP_SOURCES))

DEMO_SOURCES:= ACS_CS_DHCP_Configurator.cpp ACS_CS_DHCP_Info.cpp ACS_CS_Protocol.cpp \
				dhcptestclient.cpp
DEMO_OBJECTS := $(patsubst %.cpp,%.obj,$(DEMO_SOURCES))

TIMER_SOURCES := ACS_CS_Timer.cpp ACS_CS_Event.cpp ACS_CS_Trace.cpp timertest.cpp ACS_CS_Thread.cpp
TIMER_OBJECTS := $(patsubst %.cpp,%.obj,$(TIMER_SOURCES))

HWCPRINT_SOURCES = printattributes.cpp
HWCPRINT_OBJECTS = $(patsubst %.cpp,%.obj,$(HWCPRINT_SOURCES))

ALL_SOURCES := $(wildcard ../src/*.cpp)

vpath %.cpp .:$(CAA_PATH)/src:$(CNZ_PATH)/csprot_caa/src:$(CAA_PATH)/src:$(CNZ_PATH)/csmodd_caa/src
vpath %.h .:$(CAA_PATH)/inc
vpath %.obj ${OBJDIR}

LDFLAGS += -L$(CNZ_PATH)/cs_cxc/bin  -L$(CNZ_PATH)/cs_cxc/lib
LIBS += -lACS_CSAPI -lprot

# Targets

all: snmp demo timer

demo: $(DEMO_OBJECTS)
	$(CC) $(addprefix $(OBJDIR)/,$(DEMO_OBJECTS)) $(LDFLAGS) $(LIBSDIR) $(LIBS) -o $@
	
snmp: $(SNMP_OBJECTS)
	$(CC) $(addprefix $(OBJDIR)/,$(SNMP_OBJECTS)) $(LDFLAGS) $(LIBSDIR) $(LIBS) -o $@

timer: $(TIMER_OBJECTS)
	$(CC) $(addprefix $(OBJDIR)/,$(TIMER_OBJECTS)) $(LDFLAGS) $(LIBSDIR) $(LIBS) -o $@

hwcprint: $(HWCPRINT_OBJECTS)
	$(CC) $(addprefix $(OBJDIR)/,$(HWCPRINT_OBJECTS)) $(LDFLAGS) $(LIBSDIR) $(LIBS) -o $@

.PHONY : cppcheck
cppcheck:
	$(SILENT) echo "Running cppcheck"
	$(SILENT) cppcheck $(ALL_SOURCES) | egrep "(error)|100% done"

.PHONY : clean
clean:
	@echo "Cleaning"
	@rm -f $(OBJDIR)/* ./*.gcov *.output snmp demo timer


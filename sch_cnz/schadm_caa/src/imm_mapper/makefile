#include ../../../sch_cxc/include.mk
CNZ_NAME ?= sch_cnz
CAA_NAME ?= schadm_caa
CXC_NAME ?= sch_cxc
 
# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk
                                
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

LIB_NAME = acs_schimm
LIB_NAME_STATIC = lib$(LIB_NAME).a
VOB_NAME = ntacs
ifeq ($(ECLIPSE),1)
	IODEV_VOB := /cc/IO_Developments
VOB_PATH := /cc/cm4ap/$(VOB_NAME)
else
	VOB_PATH = $(ACS_ROOT)
endif
CNZ_NAME = sch_cnz
LIBAPI_INT_DIR = $(VOB_PATH)/$(CNZ_NAME)/sch_cxc/bin/lib_int

CURRDIR = $(PWD)

# here you can add own lib paths
LIBSDIR += -L$(LIBAPI_EXT_DIR)
LIBSDIR += -L$(AP_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)

CFLAGS += -fPIC -fmessage-length=0
CFLAGS += -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-local-typedefs
CFLAGS +=

CCOVFLAGS += -fprofile-arcs -ftest-coverage

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage --coverage
CFLAGS += -I../csadm_caa/csprot/inc -I$(AP_SDK_INC)
GCOV_FLAGS =
LIBS += -lgcov
endif

# here you can add own Include paths and/or other includes
CINCLUDES +=  -I../../inc/imm_mapper 
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)

LIBS += -lrt -lpthread -lacs_tra -lacs_apgcc -lboost_serialization

SOURCES = ACS_SCH_ImBase.cpp ACS_SCH_ImObjectCreator.cpp ACS_SCH_ImRepository.cpp \
           ACS_SCH_ImUtils.cpp ACS_SCH_ImBlade.cpp ACS_SCH_ImIMMReader.cpp ACS_SCH_ImImmObject.cpp \
           ACS_SCH_ImModel.cpp ACS_SCH_ImModelSubset.cpp ACS_SCH_ImTypes.cpp \
          ACS_SCH_ImValuesDefinitionType.cpp 

OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

DEPS := $(SOURCES:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)
	-include $(DEPS)
endif

vpath %.cpp $(SRCDIR) $(SRCDIR)/imm_mapper
vpath %.h $(INCDIR)
vpath %.obj $(OBJDIR)
vpath %.cpp ../schadm_caa/src

#Create all
.PHONY: all
all: $(LIB_NAME_STATIC)

#Create library
.PHONY: $(LIB_NAME_STATIC)
$(LIB_NAME_STATIC): fs $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(LIB_NAME_STATIC)'
	$(AR) cr $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC) $(OBJFILES)
	cd $(CURRDIR)
	$(SEPARATOR_STR)
	$(NEW_LINE)

#Clean build files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning IMM API object files...'
	$(SILENT)$(RM) -f $(OBJDIR)/*
	$(SILENT)$(ECHO) 'Cleaning IMM API binary file: $(LIB_NAME_STATIC) and links...'
	$(SILENT)$(RM) -f $(LIBAPI_INT_DIR)/$(LIB_NAME_STATIC)

#Clean binary files
.PHONY: distclean
distclean: clean

# Perform chain of code analysis tools - add more?
.PHONY: metrics
metrics : cccc lint

# CCCC
# C and C++ Code Counter
.PHONY: cccc
cccc:
	$(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
	$(CCCC) $(wildcard $(INCDIR)/*)
	$(CCCC) $(wildcard $(SRCDIR)/*)

# LINT
# Splint is a tool for static code analysis
.PHONY: lint
lint:
	$(SILENT)$(ECHO) 'LINT file: $@'
	$(LINT) $(wildcard $(INCDIR)/*)
	$(LINT) $(wildcard $(SRCDIR)/*)

# Extract debug symbols
.PHONY: symbols
symbols:
	$(SILENT)$(ECHO) 'Extracting symbols from $(LIB_NAME_STATIC)'

#Create dependencies
.PHONY: depend
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^

#Create obj folder if not existing
.PHONY: fs
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

## DO NOT DELETE THIS LINE -- make depend needs it


#include "ACS_LM_Trace.h"

#define ACS_LM_DefineTrace(aClass) \
    static ACS_LM_Trace lm_trace_##aClass(#aClass); 

#define ACS_LM_PrintTrace(aClass, aMsg) \
		if (lm_trace_##aClass.isActive()) \
		{ \
			lm_trace_##aClass << " : " << aMsg; \
			lm_trace_##aClass.print(); \
      }



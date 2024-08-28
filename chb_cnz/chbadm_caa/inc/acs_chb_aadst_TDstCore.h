#include <limits>
#include <limits.h>
#include <stdint.h>
#include "errno.h"
#include "acs_chb_aadst_tzfile.h"
#include "acs_chb_aadst_TDstTypes.h"

#define GRANDPARENTED	"Local time zone must be set--see zic manual page"

/* Unlike <ctype.h>'s isdigit, this also works if c < 0 | c > UCHAR_MAX. */
#define is_digit(c) ((unsigned)(c) - '0' <= 9)

#ifndef TRUE
#define TRUE	1
#endif /* !defined TRUE */

#ifndef FALSE
#define FALSE	0
#endif /* !defined FALSE */

#ifndef TYPE_BIT
#define TYPE_BIT(type)	(sizeof (type) * CHAR_BIT)
#endif /* !defined TYPE_BIT */

#ifndef TYPE_SIGNED
#define TYPE_SIGNED(type) (((type) -1) < 0)
#endif /* !defined TYPE_SIGNED */

#define INITIALIZE(x)	((x) = 0)

#ifndef YEARSPERREPEAT
#define YEARSPERREPEAT		400	/* years before a Gregorian repeat */
#endif /* !defined YEARSPERREPEAT */

/*
** The Gregorian year averages 365.2425 days, which is 31556952 seconds.
*/

#ifndef AVGSECSPERYEAR
#define AVGSECSPERYEAR		31556952L
#endif /* !defined AVGSECSPERYEAR */

#ifndef SECSPERREPEAT
#define SECSPERREPEAT		((int_fast64_t) YEARSPERREPEAT * (int_fast64_t) AVGSECSPERYEAR)
#endif /* !defined SECSPERREPEAT */

#ifndef SECSPERREPEAT_BITS
#define SECSPERREPEAT_BITS	34	/* ceil(log2(SECSPERREPEAT)) */
#endif /* !defined SECSPERREPEAT_BITS */

#define MAX_STRING_LENGTH			1024

#define SECSPERNYEAR					(SECSPERDAY * DAYSPERNYEAR)
#define SECSPERLYEAR					(SECSPERNYEAR + SECSPERDAY)

//#define STR_TIMEZONE_ENTRY		"AADSTTZ"

#define MY_TZNAME_MAX	255
#define WILDABBR	"   "

#ifndef TZ_STRLEN_MAX
#define TZ_STRLEN_MAX 255
#endif /* !defined TZ_STRLEN_MAX */

//static char		wildabbr[] = WILDABBR;

static const char	AADSTgmt[] = "GMT";

#define BIGGEST(a, b)	(((a) > (b)) ? (a) : (b))

struct ttinfo {				/* time type information */
	int_fast32_t	tt_gmtoff;	/* UT offset in seconds */
	int		tt_isdst;	/* used to set tm_isdst */
	int		tt_abbrind;	/* abbreviation list index */
	int		tt_ttisstd;	/* TRUE if transition is std time */
	int		tt_ttisgmt;	/* TRUE if transition is UT */
};

struct lsinfo {				/* leap second information */
	time_t		ls_trans;	/* transition time */
	int_fast64_t	ls_corr;	/* correction to apply */
};

struct state {
	int		leapcnt;
	int		timecnt;
	int		typecnt;
	int		charcnt;
	int		goback;
	int		goahead;
	time_t		ats[TZ_MAX_TIMES];
	unsigned char	types[TZ_MAX_TIMES];
	struct ttinfo	ttis[TZ_MAX_TYPES];
	char		chars[BIGGEST(BIGGEST(TZ_MAX_CHARS + 1, sizeof AADSTgmt),
				(2 * (MY_TZNAME_MAX + 1)))];
	struct lsinfo	lsis[TZ_MAX_LEAPS];
	int		defaulttype; /* for early times or if no transitions */
};

struct rule {
	int		r_type;		/* type of rule--see below */
	int		r_day;		/* day number of rule */
	int		r_week;		/* week number of rule */
	int		r_mon;		/* month number of rule */
	int_fast32_t	r_time;		/* transition time of rule */
};

class TAadstLocTime
{
	public:
	////////////////////////////////////////////// Costructor/Distructor
		TAadstLocTime();
		virtual ~TAadstLocTime();
		int_fast32_t AADST_detzcode(const char * codep);
		int_fast64_t AADST_detzcode64(const char * codep);
		int AADST_differ_by_repeat(time_t t1, time_t t0);
		const char* AADST_getzname(const char * strp);
		const char* AADST_getqzname(const char * strp, const int delim);
		const char* AADST_getnum(const char * strp, int * nump, int min, int max);
		const char* AADST_getsecs(const char * strp, int_fast32_t * secsp);
		const char* AADST_getoffset(const char * strp, int_fast32_t * offsetp);
		const char* AADST_getrule(const char * strp, struct rule * rulep);
		void AADST_gmtload(struct state * sp);
		struct tm* AADST_gmtsub(const time_t * timep, int_fast32_t offset,	struct tm * tmp);
		struct tm* AADST_localsub(const time_t * timep, int_fast32_t offset,	struct tm * tmp);
		int AADST_increment_overflow(int * number, int delta);
		int AADST_leaps_thru_end_of(int y);
		int AADST_increment_overflow32(int_fast32_t * number, int delta);
		int AADST_increment_overflow_time(time_t *t, int_fast32_t delta);
		int AADST_normalize_overflow32(int_fast32_t * tensptr, int * unitsptr, int base);
		int AADST_normalize_overflow(int * tensptr, int * unitsptr,	int base);
		void AADST_settzname(void);
		time_t AADST_time1(struct tm * tmp,	struct tm * (TAadstLocTime::*funcp)(const time_t *, int_fast32_t, struct tm *), int_fast32_t offset);
		time_t AADST_time2(struct tm *tmp,	struct tm * (TAadstLocTime::*funcp)(const time_t *,	int_fast32_t, struct tm*),	int_fast32_t offset, int * okayp);
		time_t AADST_time2sub(struct tm *tmp,	struct tm * (TAadstLocTime::*funcp)(const time_t *,	int_fast32_t, struct tm*),	int_fast32_t offset, int * okayp, int do_norm_secs);
		struct tm* AADST_timesub(const time_t * timep, int_fast32_t offset,	const struct state * sp, struct tm * tmp);
		int AADST_tmcomp(const struct tm * atmp, const struct tm * btmp);
		int_fast32_t AADST_transtime(int year, const struct rule * rulep, int_fast32_t offset);
		int AADST_typesequiv(const struct state * sp, int a, int b);
		int AADST_tzload(const char * name, struct state * sp,	int doextend);
		int AADST_tzparse(const char * name, struct state * sp,	int lastditch);
		void AADST_tzsetwall(void);
		// Original not static
		void AADST_tzset(const char *name);
		struct tm* AADST_localtime(const time_t *const timep, const char* szName);
		struct tm* AADST_localtime_r(const time_t *const timep, struct tm *tmp);
		struct tm* AADST_gmtime(const time_t *const timep);
		struct tm* AADST_gmtime_r(const time_t *const timep, struct tm *tmp);
		char* AADST_ctime(const time_t *const timep, char* szName);
		char* AADST_ctime_r(const time_t *const timep, char *buf);
		time_t AADST_mktime(struct tm *const tmp, char* szName);
		// ATTRIBUTE
		static char		wildabbr[];
		struct state	m_lclmem;
		struct state	m_gmtmem;
		int		m_lcl_is_set;
		int		m_gmt_is_set;
		char		m_lcl_TZname[TZ_STRLEN_MAX + 1];
		char *m_tzname[2];
		struct tm	m_tmTime;
		struct ttinfo m_zttinfo;
};

// Class for extract core
class TDstCore
{
	public:
	////////////////////////////////////////////// Costructor/Distructor
		TDstCore();
		virtual ~TDstCore();
	////////////////////////////////////////////// Public method
		// Fill the TZdumpItem* list with time zone entry
		void getDstList(const char *szTimeZone, int cutloyear, int cuthiyear, std::list<TZdumpItem*> *plstItem);
	////////////////////////////////////////////// Private method
	private:
		// Create new environment with "'STR_TIMEZONE_ENTRY'=xxx" variable
		char** createNewEnvironment(const char *szTimeZone);
		// Restore old environment (first of the "createNewEnvironment" call
		void restoreEnvironment(char **pEnv);
		// Fill the timezone object
		void FillTimezoneObj(time_t t, const char* szName, std::list<TZdumpItem*> *plstItem);
		// Geograph zone
		const char* abbr(struct tm *tmp);
		// Calculate year in seconds
		time_t yeartot(int y);
		// Difference between two date
		int delta(struct tm * newp, struct tm *oldp);
		// Iteration from near date
		time_t hunt(time_t lot, time_t hit, const char* szName, std::list<TZdumpItem*> *plstItem);
	////////////////////////////////////////////// Private Attributes
	private:
		TAadstLocTime m_objDstTime;
	////////////////////////////////////////////// Private static Attribute
	static const char szNullChar;
};

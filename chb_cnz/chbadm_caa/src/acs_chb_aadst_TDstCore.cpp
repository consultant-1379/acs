#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "fcntl.h"
#include <unistd.h>
#include "acs_chb_aadst_TDstCore.h"

// Static attributes initialization
const char TDstCore::szNullChar = '\0';


#ifndef TZ_ABBR_MAX_LEN
#define TZ_ABBR_MAX_LEN	16
#endif /* !defined TZ_ABBR_MAX_LEN */

#ifndef TZ_ABBR_CHAR_SET
#define TZ_ABBR_CHAR_SET \
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :+-._"
#endif /* !defined TZ_ABBR_CHAR_SET */

#ifndef TZ_ABBR_ERR_CHAR
#define TZ_ABBR_ERR_CHAR	'_'
#endif /* !defined TZ_ABBR_ERR_CHAR */

#define OPEN_MODE	O_RDONLY

/*
** The DST rules to use if TZ has no rules and we can't load TZDEFRULES.
** We default to US rules as of 1999-08-17.
** POSIX 1003.1 section 8.1.1 says that the default DST rules are
** implementation dependent; for historical reasons, US rules are a
** common default.
*/
#ifndef TZDEFRULESTRING
#define TZDEFRULESTRING ",M4.1.0,M10.5.0"
#endif /* !defined TZDEFDST */

char TAadstLocTime::wildabbr[] = WILDABBR;

#define JULIAN_DAY		0	/* Jn - Julian day */
#define DAY_OF_YEAR		1	/* n - day of year */
#define MONTH_NTH_DAY_OF_WEEK	2	/* Mm.n.d - month, week, day of week */

#define lclptr		(&m_lclmem)
#define gmtptr		(&m_gmtmem)

TAadstLocTime::TAadstLocTime()
{
	m_tzname[0] = TAadstLocTime::wildabbr;
	m_tzname[1] = TAadstLocTime::wildabbr;
}

TAadstLocTime::~TAadstLocTime()
{
}

int_fast32_t TAadstLocTime::AADST_detzcode(const char *const codep)
{
	register int_fast32_t	result;
	register int		i;

	result = (codep[0] & 0x80) ? -1 : 0;
	for (i = 0; i < 4; ++i)
		result = (result << 8) | (codep[i] & 0xff);
	return result;
}

int_fast64_t TAadstLocTime::AADST_detzcode64(const char *const codep)
{
	register int_fast64_t result;
	register int	i;

	result = (codep[0] & 0x80) ? -1 : 0;
	for (i = 0; i < 8; ++i)
		result = (result << 8) | (codep[i] & 0xff);
	return result;
}

void TAadstLocTime::AADST_settzname(void)
{
	register struct state * const	sp = lclptr;
	register int			i;

	m_tzname[0] = TAadstLocTime::wildabbr;
	m_tzname[1] = TAadstLocTime::wildabbr;

	/*
	** And to get the latest zone names into tzname. . .
	*/
	for (i = 0; i < sp->typecnt; ++i) {
		register const struct ttinfo * const	ttisp = &sp->ttis[i];

		m_tzname[ttisp->tt_isdst] = &sp->chars[ttisp->tt_abbrind];
	}
	for (i = 0; i < sp->timecnt; ++i) {
		register const struct ttinfo * const	ttisp =
							&sp->ttis[
								sp->types[i]];

		m_tzname[ttisp->tt_isdst] =
			&sp->chars[ttisp->tt_abbrind];
	}
	/*
	** Finally, scrub the abbreviations.
	** First, replace bogus characters.
	*/
	for (i = 0; i < sp->charcnt; ++i)
		if (strchr(TZ_ABBR_CHAR_SET, sp->chars[i]) == NULL)
			sp->chars[i] = TZ_ABBR_ERR_CHAR;
	/*
	** Second, truncate long abbreviations.
	*/
	for (i = 0; i < sp->typecnt; ++i) {
		register const struct ttinfo * const	ttisp = &sp->ttis[i];
		register char *				cp = &sp->chars[ttisp->tt_abbrind];

		if (strlen(cp) > TZ_ABBR_MAX_LEN &&
			strcmp(cp, GRANDPARENTED) != 0)
				*(cp + TZ_ABBR_MAX_LEN) = '\0';
	}
}

int TAadstLocTime::AADST_differ_by_repeat(const time_t t1, const time_t t0)
{
	if (TYPE_BIT(time_t) - TYPE_SIGNED(time_t) < SECSPERREPEAT_BITS)
		return 0;
	return t1 - t0 == SECSPERREPEAT;
}

int TAadstLocTime::AADST_tzload(register const char *name, register struct state *const sp, register const int doextend)
{
	register const char *		p;
	register int			i;
	register int			fid;
	register int			stored;
	register int			nread;
	typedef union {
		struct tzhead	tzhead;
		char		buf[2 * sizeof(struct tzhead) +
					2 * sizeof *sp +
					4 * TZ_MAX_TIMES];
	} u_t;

	u_t				u;
	register u_t * const		up = &u;

	sp->goback = sp->goahead = FALSE;
	if (name == NULL && (name = TZDEFAULT) == NULL)
		goto oops;
	{
		register int	doaccess;
		/*
		** Section 4.9.1 of the C standard says that
		** "FILENAME_MAX expands to an integral constant expression
		** that is the size needed for an array of char large enough
		** to hold the longest file name string that the implementation
		** guarantees can be opened."
		*/
		char		fullname[FILENAME_MAX + 1];

		if (name[0] == ':')
			++name;
		doaccess = name[0] == '/';
		if (!doaccess) {
			if ((p = TZDIR) == NULL)
				goto oops;
			if ((strlen(p) + strlen(name) + 1) >= sizeof fullname)
				goto oops;
			(void) strcpy(fullname, p);
			(void) strcat(fullname, "/");
			(void) strcat(fullname, name);
			/*
			** Set doaccess if '.' (as in "../") shows up in name.
			*/
			if (strchr(name, '.') != NULL)
				doaccess = TRUE;
			name = fullname;
		}
		if (doaccess && access(name, R_OK) != 0)
			goto oops;
		if ((fid = open(name, OPEN_MODE)) == -1)
			goto oops;
	}
	nread = read(fid, up->buf, sizeof up->buf);
	if (close(fid) < 0 || nread <= 0)
		goto oops;
	for (stored = 4; stored <= 8; stored *= 2) {
		int		ttisstdcnt;
		int		ttisgmtcnt;
		int		timecnt;

		ttisstdcnt = (int) AADST_detzcode(up->tzhead.tzh_ttisstdcnt);
		ttisgmtcnt = (int) AADST_detzcode(up->tzhead.tzh_ttisgmtcnt);
		sp->leapcnt = (int) AADST_detzcode(up->tzhead.tzh_leapcnt);
		sp->timecnt = (int) AADST_detzcode(up->tzhead.tzh_timecnt);
		sp->typecnt = (int) AADST_detzcode(up->tzhead.tzh_typecnt);
		sp->charcnt = (int) AADST_detzcode(up->tzhead.tzh_charcnt);
		p = up->tzhead.tzh_charcnt + sizeof up->tzhead.tzh_charcnt;
		if (sp->leapcnt < 0 || sp->leapcnt > TZ_MAX_LEAPS ||
			sp->typecnt <= 0 || sp->typecnt > TZ_MAX_TYPES ||
			sp->timecnt < 0 || sp->timecnt > TZ_MAX_TIMES ||
			sp->charcnt < 0 || sp->charcnt > TZ_MAX_CHARS ||
			(ttisstdcnt != sp->typecnt && ttisstdcnt != 0) ||
			(ttisgmtcnt != sp->typecnt && ttisgmtcnt != 0))
				goto oops;
		if (nread - (p - up->buf) <
			sp->timecnt * stored +		/* ats */
			sp->timecnt +			/* types */
			sp->typecnt * 6 +		/* ttinfos */
			sp->charcnt +			/* chars */
			sp->leapcnt * (stored + 4) +	/* lsinfos */
			ttisstdcnt +			/* ttisstds */
			ttisgmtcnt)			/* ttisgmts */
				goto oops;
		timecnt = 0;
		for (i = 0; i < sp->timecnt; ++i) {
			int_fast64_t at
			  = stored == 4 ? AADST_detzcode(p) : AADST_detzcode64(p);
			sp->types[i] = ((TYPE_SIGNED(time_t)
					 ? std::numeric_limits<time_t>::min() <= at
					 : 0 <= at)
					&& at <= std::numeric_limits<time_t>::max());
			if (sp->types[i]) {
				if (i && !timecnt && at != std::numeric_limits<time_t>::min()) {
					/*
					** Keep the earlier record, but tweak
					** it so that it starts with the
					** minimum time_t value.
					*/
					sp->types[i - 1] = 1;
					sp->ats[timecnt++] = std::numeric_limits<time_t>::min();
				}
				sp->ats[timecnt++] = at;
			}
			p += stored;
		}
		timecnt = 0;
		for (i = 0; i < sp->timecnt; ++i) {
			unsigned char typ = *p++;
			if (sp->typecnt <= typ)
				goto oops;
			if (sp->types[i])
				sp->types[timecnt++] = typ;
		}
		sp->timecnt = timecnt;
		for (i = 0; i < sp->typecnt; ++i) {
			register struct ttinfo *	ttisp;

			ttisp = &sp->ttis[i];
			ttisp->tt_gmtoff = AADST_detzcode(p);
			p += 4;
			ttisp->tt_isdst = (unsigned char) *p++;
			if (ttisp->tt_isdst != 0 && ttisp->tt_isdst != 1)
				goto oops;
			ttisp->tt_abbrind = (unsigned char) *p++;
			if (ttisp->tt_abbrind < 0 ||
				ttisp->tt_abbrind > sp->charcnt)
					goto oops;
		}
		for (i = 0; i < sp->charcnt; ++i)
			sp->chars[i] = *p++;
		sp->chars[i] = '\0';	/* ensure '\0' at end */
		for (i = 0; i < sp->leapcnt; ++i) {
			register struct lsinfo *	lsisp;

			lsisp = &sp->lsis[i];
			lsisp->ls_trans = (stored == 4) ?
				AADST_detzcode(p) : AADST_detzcode64(p);
			p += stored;
			lsisp->ls_corr = AADST_detzcode(p);
			p += 4;
		}
		for (i = 0; i < sp->typecnt; ++i) {
			register struct ttinfo *	ttisp;

			ttisp = &sp->ttis[i];
			if (ttisstdcnt == 0)
				ttisp->tt_ttisstd = FALSE;
			else {
				ttisp->tt_ttisstd = *p++;
				if (ttisp->tt_ttisstd != TRUE &&
					ttisp->tt_ttisstd != FALSE)
						goto oops;
			}
		}
		for (i = 0; i < sp->typecnt; ++i) {
			register struct ttinfo *	ttisp;

			ttisp = &sp->ttis[i];
			if (ttisgmtcnt == 0)
				ttisp->tt_ttisgmt = FALSE;
			else {
				ttisp->tt_ttisgmt = *p++;
				if (ttisp->tt_ttisgmt != TRUE &&
					ttisp->tt_ttisgmt != FALSE)
						goto oops;
			}
		}
		/*
		** If this is an old file, we're done.
		*/
		if (up->tzhead.tzh_version[0] == '\0')
			break;
		nread -= p - up->buf;
		for (i = 0; i < nread; ++i)
			up->buf[i] = p[i];
		/*
		** If this is a signed narrow time_t system, we're done.
		*/
		if (TYPE_SIGNED(time_t) && stored >= (int) sizeof(time_t))
			break;
	}
	if (doextend && nread > 2 &&
		up->buf[0] == '\n' && up->buf[nread - 1] == '\n' &&
		sp->typecnt + 2 <= TZ_MAX_TYPES) {
			struct state	ts;
			register int	result;

			up->buf[nread - 1] = '\0';
			result = AADST_tzparse(&up->buf[1], &ts, FALSE);
			if (result == 0 && ts.typecnt == 2 &&
				sp->charcnt + ts.charcnt <= TZ_MAX_CHARS) {
					for (i = 0; i < 2; ++i)
						ts.ttis[i].tt_abbrind +=
							sp->charcnt;
					for (i = 0; i < ts.charcnt; ++i)
						sp->chars[sp->charcnt++] =
							ts.chars[i];
					i = 0;
					while (i < ts.timecnt &&
						ts.ats[i] <=
						sp->ats[sp->timecnt - 1])
							++i;
					while (i < ts.timecnt &&
					    sp->timecnt < TZ_MAX_TIMES) {
						sp->ats[sp->timecnt] =
							ts.ats[i];
						sp->types[sp->timecnt] =
							sp->typecnt +
							ts.types[i];
						++sp->timecnt;
						++i;
					}
					sp->ttis[sp->typecnt++] = ts.ttis[0];
					sp->ttis[sp->typecnt++] = ts.ttis[1];
			}
	}
	if (sp->timecnt > 1) {
		for (i = 1; i < sp->timecnt; ++i)
			if (AADST_typesequiv(sp, sp->types[i], sp->types[0]) &&
				AADST_differ_by_repeat(sp->ats[i], sp->ats[0])) {
					sp->goback = TRUE;
					break;
				}
		for (i = sp->timecnt - 2; i >= 0; --i)
			if (AADST_typesequiv(sp, sp->types[sp->timecnt - 1],
				sp->types[i]) &&
				AADST_differ_by_repeat(sp->ats[sp->timecnt - 1],
				sp->ats[i])) {
					sp->goahead = TRUE;
					break;
		}
	}
	/*
	** If type 0 is is unused in transitions,
	** it's the type to use for early times.
	*/
	for (i = 0; i < sp->typecnt; ++i)
		if (sp->types[i] == 0)
			break;
	i = (i >= sp->typecnt) ? 0 : -1;
	/*
	** Absent the above,
	** if there are transition times
	** and the first transition is to a daylight time
	** find the standard type less than and closest to
	** the type of the first transition.
	*/
	if (i < 0 && sp->timecnt > 0 && sp->ttis[sp->types[0]].tt_isdst) {
		i = sp->types[0];
		while (--i >= 0)
			if (!sp->ttis[i].tt_isdst)
				break;
	}
	/*
	** If no result yet, find the first standard type.
	** If there is none, punt to type zero.
	*/
	if (i < 0) {
		i = 0;
		while (sp->ttis[i].tt_isdst)
			if (++i >= sp->typecnt) {
				i = 0;
				break;
			}
	}
	sp->defaulttype = i;
	return 0;
oops:
	return -1;
}

int TAadstLocTime::AADST_typesequiv(const struct state *const sp, const int a, const int b)
{
	register int	result;

	if (sp == NULL ||
		a < 0 || a >= sp->typecnt ||
		b < 0 || b >= sp->typecnt)
			result = FALSE;
	else {
		register const struct ttinfo *	ap = &sp->ttis[a];
		register const struct ttinfo *	bp = &sp->ttis[b];
		result = ap->tt_gmtoff == bp->tt_gmtoff &&
			ap->tt_isdst == bp->tt_isdst &&
			ap->tt_ttisstd == bp->tt_ttisstd &&
			ap->tt_ttisgmt == bp->tt_ttisgmt &&
			strcmp(&sp->chars[ap->tt_abbrind],
			&sp->chars[bp->tt_abbrind]) == 0;
	}
	return result;
}

static const int	mon_lengths[2][MONSPERYEAR] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const int	year_lengths[2] = {
	DAYSPERNYEAR, DAYSPERLYEAR
};

/*
** Given a pointer into a time zone string, scan until a character that is not
** a valid character in a zone name is found. Return a pointer to that
** character.
*/

const char* TAadstLocTime::AADST_getzname(register const char *strp)
{
	register char	c;

	while ((c = *strp) != '\0' && !is_digit(c) && c != ',' && c != '-' &&
		c != '+')
			++strp;
	return strp;
}

/*
** Given a pointer into an extended time zone string, scan until the ending
** delimiter of the zone name is located. Return a pointer to the delimiter.
**
** As with AADST_getzname above, the legal character set is actually quite
** restricted, with other characters producing undefined results.
** We don't do any checking here; checking is done later in common-case code.
*/

const char* TAadstLocTime::AADST_getqzname(register const char *strp, const int delim)
{
	register int	c;

	while ((c = *strp) != '\0' && c != delim)
		++strp;
	return strp;
}

/*
** Given a pointer into a time zone string, extract a number from that string.
** Check that the number is within a specified range; if it is not, return
** NULL.
** Otherwise, return a pointer to the first character not part of the number.
*/

const char* TAadstLocTime::AADST_getnum(register const char *strp, int *const nump, const int min, const int max)
{
	register char	c;
	register int	num;

	if (strp == NULL || !is_digit(c = *strp))
		return NULL;
	num = 0;
	do {
		num = num * 10 + (c - '0');
		if (num > max)
			return NULL;	/* illegal value */
		c = *++strp;
	} while (is_digit(c));
	if (num < min)
		return NULL;		/* illegal value */
	*nump = num;
	return strp;
}

/*
** Given a pointer into a time zone string, extract a number of seconds,
** in hh[:mm[:ss]] form, from the string.
** If any error occurs, return NULL.
** Otherwise, return a pointer to the first character not part of the number
** of seconds.
*/

const char* TAadstLocTime::AADST_getsecs(register const char *strp, int_fast32_t *const secsp)
{
	int	num;

	/*
	** `HOURSPERDAY * DAYSPERWEEK - 1' allows quasi-Posix rules like
	** "M10.4.6/26", which does not conform to Posix,
	** but which specifies the equivalent of
	** ``02:00 on the first Sunday on or after 23 Oct''.
	*/
	strp = AADST_getnum(strp, &num, 0, HOURSPERDAY * DAYSPERWEEK - 1);
	if (strp == NULL)
		return NULL;
	*secsp = num * (int_fast32_t) SECSPERHOUR;
	if (*strp == ':') {
		++strp;
		strp = AADST_getnum(strp, &num, 0, MINSPERHOUR - 1);
		if (strp == NULL)
			return NULL;
		*secsp += num * SECSPERMIN;
		if (*strp == ':') {
			++strp;
			/* `SECSPERMIN' allows for leap seconds. */
			strp = AADST_getnum(strp, &num, 0, SECSPERMIN);
			if (strp == NULL)
				return NULL;
			*secsp += num;
		}
	}
	return strp;
}

/*
** Given a pointer into a time zone string, extract an offset, in
** [+-]hh[:mm[:ss]] form, from the string.
** If any error occurs, return NULL.
** Otherwise, return a pointer to the first character not part of the time.
*/

const char* TAadstLocTime::AADST_getoffset(register const char *strp, int_fast32_t *const offsetp)
{
	register int	neg = 0;

	if (*strp == '-') {
		neg = 1;
		++strp;
	} else if (*strp == '+')
		++strp;
	strp = AADST_getsecs(strp, offsetp);
	if (strp == NULL)
		return NULL;		/* illegal time */
	if (neg)
		*offsetp = -*offsetp;
	return strp;
}

/*
** Given a pointer into a time zone string, extract a rule in the form
** date[/time]. See POSIX section 8 for the format of "date" and "time".
** If a valid rule is not found, return NULL.
** Otherwise, return a pointer to the first character not part of the rule.
*/

const char* TAadstLocTime::AADST_getrule(const char *strp, register struct rule *const rulep)
{
	if (*strp == 'J') {
		/*
		** Julian day.
		*/
		rulep->r_type = JULIAN_DAY;
		++strp;
		strp = AADST_getnum(strp, &rulep->r_day, 1, DAYSPERNYEAR);
	} else if (*strp == 'M') {
		/*
		** Month, week, day.
		*/
		rulep->r_type = MONTH_NTH_DAY_OF_WEEK;
		++strp;
		strp = AADST_getnum(strp, &rulep->r_mon, 1, MONSPERYEAR);
		if (strp == NULL)
			return NULL;
		if (*strp++ != '.')
			return NULL;
		strp = AADST_getnum(strp, &rulep->r_week, 1, 5);
		if (strp == NULL)
			return NULL;
		if (*strp++ != '.')
			return NULL;
		strp = AADST_getnum(strp, &rulep->r_day, 0, DAYSPERWEEK - 1);
	} else if (is_digit(*strp)) {
		/*
		** Day of year.
		*/
		rulep->r_type = DAY_OF_YEAR;
		strp = AADST_getnum(strp, &rulep->r_day, 0, DAYSPERLYEAR - 1);
	} else	return NULL;		/* invalid format */
	if (strp == NULL)
		return NULL;
	if (*strp == '/') {
		/*
		** Time specified.
		*/
		++strp;
		strp = AADST_getoffset(strp, &rulep->r_time);
	} else	rulep->r_time = 2 * SECSPERHOUR;	/* default = 2:00:00 */
	return strp;
}

/*
** Given a year, a rule, and the offset from UT at the time that rule takes
** effect, calculate the year-relative time that rule takes effect.
*/

int_fast32_t TAadstLocTime::AADST_transtime(const int year, register const struct rule *const rulep, const int_fast32_t offset)
{
	register int	leapyear;
	register int_fast32_t value;
	register int	i;
	int		d, m1, yy0, yy1, yy2, dow;

	INITIALIZE(value);
	leapyear = isleap(year);
	switch (rulep->r_type) {

	case JULIAN_DAY:
		/*
		** Jn - Julian day, 1 == January 1, 60 == March 1 even in leap
		** years.
		** In non-leap years, or if the day number is 59 or less, just
		** add SECSPERDAY times the day number-1 to the time of
		** January 1, midnight, to get the day.
		*/
		value = (rulep->r_day - 1) * SECSPERDAY;
		if (leapyear && rulep->r_day >= 60)
			value += SECSPERDAY;
		break;

	case DAY_OF_YEAR:
		/*
		** n - day of year.
		** Just add SECSPERDAY times the day number to the time of
		** January 1, midnight, to get the day.
		*/
		value = rulep->r_day * SECSPERDAY;
		break;

	case MONTH_NTH_DAY_OF_WEEK:
		/*
		** Mm.n.d - nth "dth day" of month m.
		*/

		/*
		** Use Zeller's Congruence to get day-of-week of first day of
		** month.
		*/
		m1 = (rulep->r_mon + 9) % 12 + 1;
		yy0 = (rulep->r_mon <= 2) ? (year - 1) : year;
		yy1 = yy0 / 100;
		yy2 = yy0 % 100;
		dow = ((26 * m1 - 2) / 10 +
			1 + yy2 + yy2 / 4 + yy1 / 4 - 2 * yy1) % 7;
		if (dow < 0)
			dow += DAYSPERWEEK;

		/*
		** "dow" is the day-of-week of the first day of the month. Get
		** the day-of-month (zero-origin) of the first "dow" day of the
		** month.
		*/
		d = rulep->r_day - dow;
		if (d < 0)
			d += DAYSPERWEEK;
		for (i = 1; i < rulep->r_week; ++i) {
			if (d + DAYSPERWEEK >=
				mon_lengths[leapyear][rulep->r_mon - 1])
					break;
			d += DAYSPERWEEK;
		}

		/*
		** "d" is the day-of-month (zero-origin) of the day we want.
		*/
		value = d * SECSPERDAY;
		for (i = 0; i < rulep->r_mon - 1; ++i)
			value += mon_lengths[leapyear][i] * SECSPERDAY;
		break;
	}

	/*
	** "value" is the year-relative time of 00:00:00 UT on the day in
	** question. To get the year-relative time of the specified local
	** time on that day, add the transition time and the current offset
	** from UT.
	*/
	return value + rulep->r_time + offset;
}

/*
** Given a POSIX section 8-style TZ string, fill in the rule tables as
** appropriate.
*/

int TAadstLocTime::AADST_tzparse(const char *name, register struct state *const sp,	const int lastditch)
{
	const char *			stdname;
	const char *			dstname;
	size_t				stdlen;
	size_t				dstlen;
	int_fast32_t			stdoffset;
	int_fast32_t			dstoffset;
	register char *			cp;
	register int			load_result;
	//static struct ttinfo		zttinfo;

	INITIALIZE(dstname);
	stdname = name;
	if (lastditch) {
		stdlen = strlen(name);	/* length of standard zone name */
		name += stdlen;
		if (stdlen >= sizeof sp->chars)
			stdlen = (sizeof sp->chars) - 1;
		stdoffset = 0;
	} else {
		if (*name == '<') {
			name++;
			stdname = name;
			name = AADST_getqzname(name, '>');
			if (*name != '>')
				return (-1);
			stdlen = name - stdname;
			name++;
		} else {
			name = AADST_getzname(name);
			stdlen = name - stdname;
		}
		if (*name == '\0')
			return -1;
		name = AADST_getoffset(name, &stdoffset);
		if (name == NULL)
			return -1;
	}
	load_result = AADST_tzload(TZDEFRULES, sp, FALSE);
	if (load_result != 0)
		sp->leapcnt = 0;		/* so, we're off a little */
	if (*name != '\0') {
		if (*name == '<') {
			dstname = ++name;
			name = AADST_getqzname(name, '>');
			if (*name != '>')
				return -1;
			dstlen = name - dstname;
			name++;
		} else {
			dstname = name;
			name = AADST_getzname(name);
			dstlen = name - dstname; /* length of DST zone name */
		}
		if (*name != '\0' && *name != ',' && *name != ';') {
			name = AADST_getoffset(name, &dstoffset);
			if (name == NULL)
				return -1;
		} else	dstoffset = stdoffset - SECSPERHOUR;
		if (*name == '\0' && load_result != 0)
			name = TZDEFRULESTRING;
		if (*name == ',' || *name == ';') {
			struct rule	start;
			struct rule	end;
			register int	year;
			register int	yearlim;
			register int	timecnt;
			time_t		janfirst;

			++name;
			if ((name = AADST_getrule(name, &start)) == NULL)
				return -1;
			if (*name++ != ',')
				return -1;
			if ((name = AADST_getrule(name, &end)) == NULL)
				return -1;
			if (*name != '\0')
				return -1;
			sp->typecnt = 2;	/* standard time and DST */
			/*
			** Two transitions per year, from EPOCH_YEAR forward.
			*/
			sp->ttis[0] = sp->ttis[1] = m_zttinfo;
			sp->ttis[0].tt_gmtoff = -dstoffset;
			sp->ttis[0].tt_isdst = 1;
			sp->ttis[0].tt_abbrind = stdlen + 1;
			sp->ttis[1].tt_gmtoff = -stdoffset;
			sp->ttis[1].tt_isdst = 0;
			sp->ttis[1].tt_abbrind = 0;
			timecnt = 0;
			janfirst = 0;
			yearlim = EPOCH_YEAR + YEARSPERREPEAT;
			for (year = EPOCH_YEAR; year < yearlim; year++) {
				int_fast32_t
				  starttime = AADST_transtime(year, &start, stdoffset),
				  endtime = AADST_transtime(year, &end, dstoffset);
				int_fast32_t
				  yearsecs = (year_lengths[isleap(year)]
					      * SECSPERDAY);
				int reversed = endtime < starttime;
				if (reversed) {
					int_fast32_t swap = starttime;
					starttime = endtime;
					endtime = swap;
				}
				if (reversed
				    || (starttime < endtime
					&& (endtime - starttime
					    < (yearsecs
					       + (stdoffset - dstoffset))))) {
					if (TZ_MAX_TIMES - 2 < timecnt)
						break;
					yearlim = year + YEARSPERREPEAT + 1;
					sp->ats[timecnt] = janfirst;
					if (AADST_increment_overflow_time
					    (&sp->ats[timecnt], starttime))
						break;
					sp->types[timecnt++] = reversed;
					sp->ats[timecnt] = janfirst;
					if (AADST_increment_overflow_time
					    (&sp->ats[timecnt], endtime))
						break;
					sp->types[timecnt++] = !reversed;
				}
				if (AADST_increment_overflow_time(&janfirst, yearsecs))
					break;
			}
			sp->timecnt = timecnt;
			if (!timecnt)
				sp->typecnt = 1;	/* Perpetual DST.  */
		} else {
			register int_fast32_t	theirstdoffset;
			register int_fast32_t	theirdstoffset;
			register int_fast32_t	theiroffset;
			register int		isdst;
			register int		i;
			register int		j;

			if (*name != '\0')
				return -1;
			/*
			** Initial values of theirstdoffset and theirdstoffset.
			*/
			theirstdoffset = 0;
			for (i = 0; i < sp->timecnt; ++i) {
				j = sp->types[i];
				if (!sp->ttis[j].tt_isdst) {
					theirstdoffset =
						-sp->ttis[j].tt_gmtoff;
					break;
				}
			}
			theirdstoffset = 0;
			for (i = 0; i < sp->timecnt; ++i) {
				j = sp->types[i];
				if (sp->ttis[j].tt_isdst) {
					theirdstoffset =
						-sp->ttis[j].tt_gmtoff;
					break;
				}
			}
			/*
			** Initially we're assumed to be in standard time.
			*/
			isdst = FALSE;
			theiroffset = theirstdoffset;
			/*
			** Now juggle transition times and types
			** tracking offsets as you do.
			*/
			for (i = 0; i < sp->timecnt; ++i) {
				j = sp->types[i];
				sp->types[i] = sp->ttis[j].tt_isdst;
				if (sp->ttis[j].tt_ttisgmt) {
					/* No adjustment to transition time */
				} else {
					/*
					** If summer time is in effect, and the
					** transition time was not specified as
					** standard time, add the summer time
					** offset to the transition time;
					** otherwise, add the standard time
					** offset to the transition time.
					*/
					/*
					** Transitions from DST to DDST
					** will effectively disappear since
					** POSIX provides for only one DST
					** offset.
					*/
					if (isdst && !sp->ttis[j].tt_ttisstd) {
						sp->ats[i] += dstoffset -
							theirdstoffset;
					} else {
						sp->ats[i] += stdoffset -
							theirstdoffset;
					}
				}
				theiroffset = -sp->ttis[j].tt_gmtoff;
				if (sp->ttis[j].tt_isdst)
					theirdstoffset = theiroffset;
				else	theirstdoffset = theiroffset;
			}
			/*
			** Finally, fill in ttis.
			*/
			sp->ttis[0] = sp->ttis[1] = m_zttinfo;
			sp->ttis[0].tt_gmtoff = -stdoffset;
			sp->ttis[0].tt_isdst = FALSE;
			sp->ttis[0].tt_abbrind = 0;
			sp->ttis[1].tt_gmtoff = -dstoffset;
			sp->ttis[1].tt_isdst = TRUE;
			sp->ttis[1].tt_abbrind = stdlen + 1;
			sp->typecnt = 2;
		}
	} else {
		dstlen = 0;
		sp->typecnt = 1;		/* only standard time */
		sp->timecnt = 0;
		sp->ttis[0] = m_zttinfo;
		sp->ttis[0].tt_gmtoff = -stdoffset;
		sp->ttis[0].tt_isdst = 0;
		sp->ttis[0].tt_abbrind = 0;
	}
	sp->charcnt = stdlen + 1;
	if (dstlen != 0)
		sp->charcnt += dstlen + 1;
	if ((size_t) sp->charcnt > sizeof sp->chars)
		return -1;
	cp = sp->chars;
	(void) strncpy(cp, stdname, stdlen);
	cp += stdlen;
	*cp++ = '\0';
	if (dstlen != 0) {
		(void) strncpy(cp, dstname, dstlen);
		*(cp + dstlen) = '\0';
	}
	return 0;
}

void TAadstLocTime::AADST_gmtload(struct state *const sp)
{
	if (AADST_tzload(AADSTgmt, sp, TRUE) != 0)
		(void) AADST_tzparse(AADSTgmt, sp, TRUE);
}

void TAadstLocTime::AADST_tzsetwall(void)
{
	if (m_lcl_is_set < 0)
		return;
	m_lcl_is_set = -1;

	if (AADST_tzload(NULL, lclptr, TRUE) != 0)
		AADST_gmtload(lclptr);
	AADST_settzname();
}

void TAadstLocTime::AADST_tzset(const char *name)
{
	//register const char *	name;

	//name = getenv(STR_TIMEZONE_ENTRY);
	if (name == NULL) {
		AADST_tzsetwall();
		return;
	}

	if (m_lcl_is_set > 0 && strcmp(m_lcl_TZname, name) == 0)
		return;
	m_lcl_is_set = strlen(name) < sizeof m_lcl_TZname;
	if (m_lcl_is_set)
		(void) strcpy(m_lcl_TZname, name);
	if (*name == '\0') {
		/*
		** User wants it fast rather than right.
		*/
		lclptr->leapcnt = 0;		/* so, we're off a little */
		lclptr->timecnt = 0;
		lclptr->typecnt = 0;
		lclptr->ttis[0].tt_isdst = 0;
		lclptr->ttis[0].tt_gmtoff = 0;
		lclptr->ttis[0].tt_abbrind = 0;
		(void) strcpy(lclptr->chars, AADSTgmt);
	} else if (AADST_tzload(name, lclptr, TRUE) != 0)
		if (name[0] == ':' || AADST_tzparse(name, lclptr, FALSE) != 0)
			(void) AADST_gmtload(lclptr);
	AADST_settzname();
}

/*
** The easy way to behave "as if no library function calls" localtime
** is to not call it--so we drop its guts into "AADST_localsub", which can be
** freely called. (And no, the PANS doesn't require the above behavior--
** but it *is* desirable.)
**
** The unused offset argument is for the benefit of AADST_mktime variants.
*/

/*ARGSUSED*/
struct tm* TAadstLocTime::AADST_localsub(const time_t *const timep, const int_fast32_t offset, struct tm *const tmp)
{
	register struct state *		sp;
	register const struct ttinfo *	ttisp;
	register int			i;
	register struct tm *		result;
	const time_t			t = *timep;

	sp = lclptr;

	if ((sp->goback && t < sp->ats[0]) ||
		(sp->goahead && t > sp->ats[sp->timecnt - 1])) {
			time_t			newt = t;
			register time_t		seconds;
			register time_t		years;

			if (t < sp->ats[0])
				seconds = sp->ats[0] - t;
			else	seconds = t - sp->ats[sp->timecnt - 1];
			--seconds;
			years = (seconds / SECSPERREPEAT + 1) * YEARSPERREPEAT;
			seconds = years * AVGSECSPERYEAR;
			if (t < sp->ats[0])
				newt += seconds;
			else	newt -= seconds;
			if (newt < sp->ats[0] ||
				newt > sp->ats[sp->timecnt - 1])
					return NULL;	/* "cannot happen" */
			result = AADST_localsub(&newt, offset, tmp);
			if (result == tmp) {
				register time_t	newy;

				newy = tmp->tm_year;
				if (t < sp->ats[0])
					newy -= years;
				else	newy += years;
				tmp->tm_year = newy;
				if (tmp->tm_year != newy)
					return NULL;
			}
			return result;
	}
	if (sp->timecnt == 0 || t < sp->ats[0]) {
		i = sp->defaulttype;
	} else {
		register int	lo = 1;
		register int	hi = sp->timecnt;

		while (lo < hi) {
			register int	mid = (lo + hi) >> 1;

			if (t < sp->ats[mid])
				hi = mid;
			else	lo = mid + 1;
		}
		i = (int) sp->types[lo - 1];
	}
	ttisp = &sp->ttis[i];
	/*
	** To get (wrong) behavior that's compatible with System V Release 2.0
	** you'd replace the statement below with
	**	t += ttisp->tt_gmtoff;
	**	AADST_timesub(&t, 0L, sp, tmp);
	*/
	result = AADST_timesub(&t, ttisp->tt_gmtoff, sp, tmp);
	tmp->tm_isdst = ttisp->tt_isdst;
	m_tzname[tmp->tm_isdst] = &sp->chars[ttisp->tt_abbrind];

	return result;
}

struct tm* TAadstLocTime::AADST_localtime(const time_t *const timep, const char* szName)
{
	AADST_tzset(szName);
	return AADST_localsub(timep, 0L, &m_tmTime);
}

/*
** Re-entrant version of localtime.
*/

struct tm* TAadstLocTime::AADST_localtime_r(const time_t *const timep, struct tm *tmp)
{
	return AADST_localsub(timep, 0L, tmp);
}

/*
** AADST_gmtsub is to AADST_gmtime as AADST_localsub is to localtime.
*/

struct tm* TAadstLocTime::AADST_gmtsub(const time_t *const timep, const int_fast32_t offset, struct tm *const tmp)
{
	register struct tm *	result;

	if (!m_gmt_is_set) {
		m_gmt_is_set = TRUE;

			AADST_gmtload(gmtptr);
	}
	result = AADST_timesub(timep, offset, gmtptr, tmp);
	return result;
}

struct tm* TAadstLocTime::AADST_gmtime(const time_t *const timep)
{
	return AADST_gmtsub(timep, 0L, &m_tmTime);
}

/*
* Re-entrant version of AADST_gmtime.
*/

struct tm* TAadstLocTime::AADST_gmtime_r(const time_t *const timep, struct tm *tmp)
{
	return AADST_gmtsub(timep, 0L, tmp);
}

/*
** Return the number of leap years through the end of the given year
** where, to make the math easy, the answer for year zero is defined as zero.
*/

int TAadstLocTime::AADST_leaps_thru_end_of(register const int y)
{
	return (y >= 0) ? (y / 4 - y / 100 + y / 400) :
		-(AADST_leaps_thru_end_of(-(y + 1)) + 1);
}

struct tm* TAadstLocTime::AADST_timesub(const time_t *const timep, const int_fast32_t offset,	register const struct state *const sp, register struct tm *const tmp)
{
	register const struct lsinfo *	lp;
	register time_t			tdays;
	register int			idays;	/* unsigned would be so 2003 */
	register int_fast64_t		rem;
	int				y;
	register const int *		ip;
	register int_fast64_t		corr;
	register int			hit;
	register int			i;

	corr = 0;
	hit = 0;
	i = sp->leapcnt;
	while (--i >= 0) {
		lp = &sp->lsis[i];
		if (*timep >= lp->ls_trans) {
			if (*timep == lp->ls_trans) {
				hit = ((i == 0 && lp->ls_corr > 0) ||
					lp->ls_corr > sp->lsis[i - 1].ls_corr);
				if (hit)
					while (i > 0 &&
						sp->lsis[i].ls_trans ==
						sp->lsis[i - 1].ls_trans + 1 &&
						sp->lsis[i].ls_corr ==
						sp->lsis[i - 1].ls_corr + 1) {
							++hit;
							--i;
					}
			}
			corr = lp->ls_corr;
			break;
		}
	}
	y = EPOCH_YEAR;
	tdays = *timep / SECSPERDAY;
	rem = *timep - tdays * SECSPERDAY;
	while (tdays < 0 || tdays >= year_lengths[isleap(y)]) {
		int		newy;
		register time_t	tdelta;
		register int	idelta;
		register int	leapdays;

		tdelta = tdays / DAYSPERLYEAR;
		if (! ((! TYPE_SIGNED(time_t) || INT_MIN <= tdelta)
		       && tdelta <= INT_MAX))
			return NULL;
		idelta = tdelta;
		if (idelta == 0)
			idelta = (tdays < 0) ? -1 : 1;
		newy = y;
		if (AADST_increment_overflow(&newy, idelta))
			return NULL;
		leapdays = AADST_leaps_thru_end_of(newy - 1) -
			AADST_leaps_thru_end_of(y - 1);
		tdays -= ((time_t) newy - y) * DAYSPERNYEAR;
		tdays -= leapdays;
		y = newy;
	}
	{
		register int_fast32_t	seconds;

		seconds = tdays * SECSPERDAY;
		tdays = seconds / SECSPERDAY;
		rem += seconds - tdays * SECSPERDAY;
	}
	/*
	** Given the range, we can now fearlessly cast...
	*/
	idays = tdays;
	rem += offset - corr;
	while (rem < 0) {
		rem += SECSPERDAY;
		--idays;
	}
	while (rem >= SECSPERDAY) {
		rem -= SECSPERDAY;
		++idays;
	}
	while (idays < 0) {
		if (AADST_increment_overflow(&y, -1))
			return NULL;
		idays += year_lengths[isleap(y)];
	}
	while (idays >= year_lengths[isleap(y)]) {
		idays -= year_lengths[isleap(y)];
		if (AADST_increment_overflow(&y, 1))
			return NULL;
	}
	tmp->tm_year = y;
	if (AADST_increment_overflow(&tmp->tm_year, -TM_YEAR_BASE))
		return NULL;
	tmp->tm_yday = idays;
	/*
	** The "extra" mods below avoid overflow problems.
	*/
	tmp->tm_wday = EPOCH_WDAY +
		((y - EPOCH_YEAR) % DAYSPERWEEK) *
		(DAYSPERNYEAR % DAYSPERWEEK) +
		AADST_leaps_thru_end_of(y - 1) -
		AADST_leaps_thru_end_of(EPOCH_YEAR - 1) +
		idays;
	tmp->tm_wday %= DAYSPERWEEK;
	if (tmp->tm_wday < 0)
		tmp->tm_wday += DAYSPERWEEK;
	tmp->tm_hour = (int) (rem / SECSPERHOUR);
	rem %= SECSPERHOUR;
	tmp->tm_min = (int) (rem / SECSPERMIN);
	/*
	** A positive leap second requires a special
	** representation. This uses "... ??:59:60" et seq.
	*/
	tmp->tm_sec = (int) (rem % SECSPERMIN) + hit;
	ip = mon_lengths[isleap(y)];
	for (tmp->tm_mon = 0; idays >= ip[tmp->tm_mon]; ++(tmp->tm_mon))
		idays -= ip[tmp->tm_mon];
	tmp->tm_mday = (int) (idays + 1);
	tmp->tm_isdst = 0;
//#ifdef TM_GMTOFF
	tmp->tm_gmtoff = offset;
//#endif /* defined TM_GMTOFF */
	return tmp;
}

char* TAadstLocTime::AADST_ctime(const time_t *const timep, char* szName)
{
/*
** Section 4.12.3.2 of X3.159-1989 requires that
**	The AADST_ctime function converts the calendar time pointed to by timer
**	to local time in the form of a string. It is equivalent to
**		asctime(localtime(timer))
*/
	return asctime(AADST_localtime(timep, szName));
}

char* TAadstLocTime::AADST_ctime_r(const time_t *const timep, char *buf)
{
	struct tm	mytm;

	return asctime_r(AADST_localtime_r(timep, &mytm), buf);
}

/*
** Adapted from code provided by Robert Elz, who writes:
**	The "best" way to do AADST_mktime I think is based on an idea of Bob
**	Kridle's (so its said...) from a long time ago.
**	It does a binary search of the time_t space. Since time_t's are
**	just 32 bits, its a max of 32 iterations (even at 64 bits it
**	would still be very reasonable).
*/

#ifndef WRONG
#define WRONG	(-1)
#endif /* !defined WRONG */

/*
** Normalize logic courtesy Paul Eggert.
*/

int TAadstLocTime::AADST_increment_overflow(int *const ip, int j)
{
	register int const	i = *ip;

	/*
	** If i >= 0 there can only be overflow if i + j > INT_MAX
	** or if j > INT_MAX - i; given i >= 0, INT_MAX - i cannot overflow.
	** If i < 0 there can only be overflow if i + j < INT_MIN
	** or if j < INT_MIN - i; given i < 0, INT_MIN - i cannot overflow.
	*/
	if ((i >= 0) ? (j > INT_MAX - i) : (j < INT_MIN - i))
		return TRUE;
	*ip += j;
	return FALSE;
}

int TAadstLocTime::AADST_increment_overflow32(int_fast32_t *const lp, int const m)
{
	register int_fast32_t const	l = *lp;

//	if ((l >= 0) ? (m > INT_FAST32_MAX - l) : (m < INT_FAST32_MIN - l))

	if ((l >= 0) ? (m >  std::numeric_limits<int_fast32_t>::max() - l) : (m < std::numeric_limits<int_fast32_t>::min() - l))
		return TRUE;
	*lp += m;
	return FALSE;
}

int TAadstLocTime::AADST_increment_overflow_time(time_t *tp, int_fast32_t j)
{
	/*
	** This is like
	** 'if (! (time_t_min <= *tp + j && *tp + j <= time_t_max)) ...',
	** except that it does the right thing even if *tp + j would overflow.
	*/
	if (! (j < 0
	       ? (TYPE_SIGNED(time_t) ? std::numeric_limits<time_t>::min() - j <= *tp : -1 - j < *tp)
	       : *tp <= std::numeric_limits<time_t>::max() - j))
		return TRUE;
	*tp += j;
	return FALSE;
}

int TAadstLocTime::AADST_normalize_overflow(int *const tensptr, int *const unitsptr, const int base)
{
	register int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return AADST_increment_overflow(tensptr, tensdelta);
}

int TAadstLocTime::AADST_normalize_overflow32(int_fast32_t *const tensptr, int *const unitsptr, const int base)
{
	register int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return AADST_increment_overflow32(tensptr, tensdelta);
}

int TAadstLocTime::AADST_tmcomp(register const struct tm *const atmp, register const struct tm *const btmp)
{
	register int	result;

	if (atmp->tm_year != btmp->tm_year)
		return atmp->tm_year < btmp->tm_year ? -1 : 1;
	if ((result = (atmp->tm_mon - btmp->tm_mon)) == 0 &&
		(result = (atmp->tm_mday - btmp->tm_mday)) == 0 &&
		(result = (atmp->tm_hour - btmp->tm_hour)) == 0 &&
		(result = (atmp->tm_min - btmp->tm_min)) == 0)
			result = atmp->tm_sec - btmp->tm_sec;
	return result;
}

time_t TAadstLocTime::AADST_time2sub(struct tm *const tmp, struct tm *(TAadstLocTime::*funcp)(const time_t *, int_fast32_t, struct tm *),
	 const int_fast32_t offset,
	 int *const okayp,
	 const int do_norm_secs)
{
	register const struct state *	sp;
	register int			dir;
	register int			i, j;
	register int			saved_seconds;
	register int_fast32_t			li;
	register time_t			lo;
	register time_t			hi;
	int_fast32_t				y;
	time_t				newt;
	time_t				t;
	struct tm			yourtm, mytm;

	*okayp = FALSE;
	yourtm = *tmp;
	if (do_norm_secs) {
		if (AADST_normalize_overflow(&yourtm.tm_min, &yourtm.tm_sec,
			SECSPERMIN))
				return WRONG;
	}
	if (AADST_normalize_overflow(&yourtm.tm_hour, &yourtm.tm_min, MINSPERHOUR))
		return WRONG;
	if (AADST_normalize_overflow(&yourtm.tm_mday, &yourtm.tm_hour, HOURSPERDAY))
		return WRONG;
	y = yourtm.tm_year;
	if (AADST_normalize_overflow32(&y, &yourtm.tm_mon, MONSPERYEAR))
		return WRONG;
	/*
	** Turn y into an actual year number for now.
	** It is converted back to an offset from TM_YEAR_BASE later.
	*/
	if (AADST_increment_overflow32(&y, TM_YEAR_BASE))
		return WRONG;
	while (yourtm.tm_mday <= 0) {
		if (AADST_increment_overflow32(&y, -1))
			return WRONG;
		li = y + (1 < yourtm.tm_mon);
		yourtm.tm_mday += year_lengths[isleap(li)];
	}
	while (yourtm.tm_mday > DAYSPERLYEAR) {
		li = y + (1 < yourtm.tm_mon);
		yourtm.tm_mday -= year_lengths[isleap(li)];
		if (AADST_increment_overflow32(&y, 1))
			return WRONG;
	}
	for ( ; ; ) {
		i = mon_lengths[isleap(y)][yourtm.tm_mon];
		if (yourtm.tm_mday <= i)
			break;
		yourtm.tm_mday -= i;
		if (++yourtm.tm_mon >= MONSPERYEAR) {
			yourtm.tm_mon = 0;
			if (AADST_increment_overflow32(&y, 1))
				return WRONG;
		}
	}
	if (AADST_increment_overflow32(&y, -TM_YEAR_BASE))
		return WRONG;
	yourtm.tm_year = y;
	if (yourtm.tm_year != y)
		return WRONG;
	if (yourtm.tm_sec >= 0 && yourtm.tm_sec < SECSPERMIN)
		saved_seconds = 0;
	else if (y + TM_YEAR_BASE < EPOCH_YEAR) {
		/*
		** We can't set tm_sec to 0, because that might push the
		** time below the minimum representable time.
		** Set tm_sec to 59 instead.
		** This assumes that the minimum representable time is
		** not in the same minute that a leap second was deleted from,
		** which is a safer assumption than using 58 would be.
		*/
		if (AADST_increment_overflow(&yourtm.tm_sec, 1 - SECSPERMIN))
			return WRONG;
		saved_seconds = yourtm.tm_sec;
		yourtm.tm_sec = SECSPERMIN - 1;
	} else {
		saved_seconds = yourtm.tm_sec;
		yourtm.tm_sec = 0;
	}
	/*
	** Do a binary search (this works whatever time_t's type is).
	*/
	if (!TYPE_SIGNED(time_t)) {
		lo = 0;
		hi = lo - 1;
	} else {
		lo = 1;
		for (i = 0; i < (int) TYPE_BIT(time_t) - 1; ++i)
			lo *= 2;
		hi = -(lo + 1);
	}
	for ( ; ; ) {
		t = lo / 2 + hi / 2;
		if (t < lo)
			t = lo;
		else if (t > hi)
			t = hi;
		if ((this->*funcp)(&t, offset, &mytm) == NULL) {
			/*
			** Assume that t is too extreme to be represented in
			** a struct tm; arrange things so that it is less
			** extreme on the next pass.
			*/
			dir = (t > 0) ? 1 : -1;
		} else	dir = AADST_tmcomp(&mytm, &yourtm);
		if (dir != 0) {
			if (t == lo) {
				if (t == std::numeric_limits<time_t>::max())
					return WRONG;
				++t;
				++lo;
			} else if (t == hi) {
				if (t == std::numeric_limits<time_t>::min())
					return WRONG;
				--t;
				--hi;
			}
			if (lo > hi)
				return WRONG;
			if (dir > 0)
				hi = t;
			else	lo = t;
			continue;
		}
		if (yourtm.tm_isdst < 0 || mytm.tm_isdst == yourtm.tm_isdst)
			break;
		/*
		** Right time, wrong type.
		** Hunt for right time, right type.
		** It's okay to guess wrong since the guess
		** gets checked.
		*/
		sp = (const struct state *)
			((funcp == &TAadstLocTime::AADST_localsub) ? lclptr : gmtptr);

		for (i = sp->typecnt - 1; i >= 0; --i) {
			if (sp->ttis[i].tt_isdst != yourtm.tm_isdst)
				continue;
			for (j = sp->typecnt - 1; j >= 0; --j) {
				if (sp->ttis[j].tt_isdst == yourtm.tm_isdst)
					continue;
				newt = t + sp->ttis[j].tt_gmtoff -
					sp->ttis[i].tt_gmtoff;
				if ((this->*funcp)(&newt, offset, &mytm) == NULL)
					continue;
				if (AADST_tmcomp(&mytm, &yourtm) != 0)
					continue;
				if (mytm.tm_isdst != yourtm.tm_isdst)
					continue;
				/*
				** We have a match.
				*/
				t = newt;
				goto label;
			}
		}
		return WRONG;
	}
label:
	newt = t + saved_seconds;
	if ((newt < t) != (saved_seconds < 0))
		return WRONG;
	t = newt;
	if ((this->*funcp)(&t, offset, tmp))
		*okayp = TRUE;
	return t;
}

time_t TAadstLocTime::AADST_time2(struct tm * const	tmp, struct tm * (TAadstLocTime::*funcp)(const time_t *, int_fast32_t, struct tm *),
      const int_fast32_t offset,
      int *const okayp)
{
	time_t	t;

	/*
	** First try without normalization of seconds
	** (in case tm_sec contains a value associated with a leap second).
	** If that fails, try with normalization of seconds.
	*/
	t = AADST_time2sub(tmp, funcp, offset, okayp, FALSE);
	return *okayp ? t : AADST_time2sub(tmp, funcp, offset, okayp, TRUE);
}

time_t TAadstLocTime::AADST_time1(struct tm *const tmp, struct tm *(TAadstLocTime::*funcp) (const time_t *, int_fast32_t, struct tm *), const int_fast32_t offset)
{
	register time_t			t;
	register const struct state *	sp;
	register int			samei, otheri;
	register int			sameind, otherind;
	register int			i;
	register int			nseen;
	int				seen[TZ_MAX_TYPES];
	int				types[TZ_MAX_TYPES];
	int				okay;

	if (tmp == NULL) {
		errno = EINVAL;
		return WRONG;
	}
	if (tmp->tm_isdst > 1)
		tmp->tm_isdst = 1;
	t = AADST_time2(tmp, funcp, offset, &okay);

	if (okay || tmp->tm_isdst < 0)
		return t;

	/*
	** We're supposed to assume that somebody took a time of one type
	** and did some math on it that yielded a "struct tm" that's bad.
	** We try to divine the type they started from and adjust to the
	** type they need.
	*/
	sp = (const struct state *) ((funcp == &TAadstLocTime::AADST_localsub) ?  lclptr : gmtptr);

	for (i = 0; i < sp->typecnt; ++i)
		seen[i] = FALSE;
	nseen = 0;
	for (i = sp->timecnt - 1; i >= 0; --i)
		if (!seen[sp->types[i]]) {
			seen[sp->types[i]] = TRUE;
			types[nseen++] = sp->types[i];
		}
	for (sameind = 0; sameind < nseen; ++sameind) {
		samei = types[sameind];
		if (sp->ttis[samei].tt_isdst != tmp->tm_isdst)
			continue;
		for (otherind = 0; otherind < nseen; ++otherind) {
			otheri = types[otherind];
			if (sp->ttis[otheri].tt_isdst == tmp->tm_isdst)
				continue;
			tmp->tm_sec += sp->ttis[otheri].tt_gmtoff -
					sp->ttis[samei].tt_gmtoff;
			tmp->tm_isdst = !tmp->tm_isdst;
			t = AADST_time2(tmp, funcp, offset, &okay);
			if (okay)
				return t;
			tmp->tm_sec -= sp->ttis[otheri].tt_gmtoff -
					sp->ttis[samei].tt_gmtoff;
			tmp->tm_isdst = !tmp->tm_isdst;
		}
	}
	return WRONG;
}

time_t TAadstLocTime::AADST_mktime(struct tm *const tmp, char* szName)
{
	AADST_tzset(szName);
	return AADST_time1(tmp, &TAadstLocTime::AADST_localsub, 0L);
}


TDstCore::TDstCore()
{
}

TDstCore::~TDstCore()
{
}

void TDstCore::getDstList(const char *szTimeZone, int cutloyear, int cuthiyear, std::list<TZdumpItem*> *plstItem)
{
	char **	pOldEnv;
	time_t t;
	time_t cutlotime;
	time_t cuthitime;
	time_t newt;
	struct tm *tmp;
	struct tm tm;
	struct tm *	newtmp;
	struct tm		newtm;
	char	buf[MAX_STRING_LENGTH];
	// Initialization
	pOldEnv = NULL;
	t = 0;
	cutlotime = yeartot(cutloyear);
	cuthitime = yeartot(cuthiyear);
	t = cutlotime;
	tmp = m_objDstTime.AADST_localtime(&t, szTimeZone);
	if (tmp != NULL) {
		tm = *tmp;
		(void) strncpy(buf, abbr(&tm), (sizeof buf) - 1);
	}

	for ( ; ; ) {
		newt = (t < INT_MAX - SECSPERDAY / 2 ? t + SECSPERDAY / 2	: INT_MAX);
		if (cuthitime <= newt)
			break;
		newtmp = m_objDstTime.AADST_localtime(&newt, szTimeZone);
		if (newtmp != NULL)
			newtm = *newtmp;
		if ((tmp == NULL || newtmp == NULL) ? (tmp != newtmp) :	(delta(&newtm, &tm) != (newt - t) ||
			newtm.tm_isdst != tm.tm_isdst ||
			strcmp(abbr(&newtm), buf) != 0)) {
				newt = hunt(t, newt, szTimeZone, plstItem);
				newtmp = m_objDstTime.AADST_localtime(&newt, szTimeZone);
				if (newtmp != NULL) {
					newtm = *newtmp;
				(void) strncpy(buf,	abbr(&newtm),	(sizeof buf) - 1);
				}
		}
		t = newt;
		tm = newtm;
		tmp = newtmp;
	}
}

const char* TDstCore::abbr(struct tm *tmp)
{
	char *	result;
	if (tmp->tm_isdst != 0 && tmp->tm_isdst != 1)
		return &TDstCore::szNullChar;
	result = m_objDstTime.m_tzname[tmp->tm_isdst];
	return (result == NULL) ? &TDstCore::szNullChar : result;
}

time_t TDstCore::yeartot(int y)
{
	int	myy, seconds;
	time_t t;

	myy = EPOCH_YEAR;
	t = 0;
	while (myy < y) {
		seconds = isleap(myy) ? SECSPERLYEAR : SECSPERNYEAR;
		myy += 1;
		if (t > INT_MAX - seconds)
			return INT_MAX;
		t += seconds;
	}
	return t;
}

int TDstCore::delta(struct tm * newp, struct tm *oldp)
{
	int	result;
	int tmy;

	if (newp->tm_year < oldp->tm_year)
		return -delta(oldp, newp);
	result = 0;
	for (tmy = oldp->tm_year; tmy < newp->tm_year; ++tmy)
		result += DAYSPERNYEAR + isleap_sum(tmy, TM_YEAR_BASE);
	result += newp->tm_yday - oldp->tm_yday;
	result *= HOURSPERDAY;
	result += newp->tm_hour - oldp->tm_hour;
	result *= MINSPERHOUR;
	result += newp->tm_min - oldp->tm_min;
	result *= SECSPERMIN;
	result += newp->tm_sec - oldp->tm_sec;
	return result;
}

time_t TDstCore::hunt(time_t lot, time_t hit, const char* szName, std::list<TZdumpItem*> *plstItem)
{
	time_t t;
	struct tm lotm;
	struct tm *lotmp;
	struct tm tm;
	struct tm *tmp;
	char loab[MAX_STRING_LENGTH];

	lotmp = m_objDstTime.AADST_localtime(&lot, szName);

	if (lotmp != NULL) {
		lotm = *lotmp;
		(void) strncpy(loab, abbr(&lotm), (sizeof loab) - 1);
	}
	for ( ; ; ) {
		time_t diff = hit - lot;
		if (diff < 2)
			break;
		t = lot;
		t += diff / 2;
		if (t <= lot)
			++t;
		else if (t >= hit)
			--t;
		tmp = m_objDstTime.AADST_localtime(&t, szName);
		if (tmp != NULL)
			tm = *tmp;
		if ((lotmp == NULL || tmp == NULL) ? (lotmp == tmp) :
			(delta(&tm, &lotm) == (t - lot) &&
			tm.tm_isdst == lotm.tm_isdst &&
			strcmp(abbr(&tm), loab) == 0)) {
				lot = t;
				lotm = tm;
				lotmp = tmp;
		} else	hit = t;
	}
	FillTimezoneObj(lot, szName, plstItem);
	FillTimezoneObj(hit, szName, plstItem);
	return hit;
}

void TDstCore::FillTimezoneObj(time_t t, const char* szName, std::list<TZdumpItem*> *plstItem)
{
	struct tm *	tmUtc;
	struct tm *	tmZone;
	TZdumpItem *pZdumpItem = NULL;
	// Set UTC time Update
	m_objDstTime.AADST_tzset(TDstCore::szNullChar);
	// Get UTC time
	tmUtc = m_objDstTime.AADST_gmtime(&t);
	// Get local time
	tmZone = m_objDstTime.AADST_localtime(&t, szName);
	//tmZone = AADST_localtime(&t);
	// Check UTC and timezone computing result
	//printf("%i/%i/%i  %i:%i:%i isdst=%i\n", lotmp->tm_mday, lotmp->tm_mon, lotmp->tm_year + 1900, lotmp->tm_hour ,lotmp->tm_min ,lotmp->tm_sec ,lotmp->tm_isdst);
	//printf("%i, %i\n", tmUtc, tmZone);
	if((tmUtc != NULL) && (tmZone != NULL)){
		// Create entry
		pZdumpItem = new TZdumpItem();
		// Push it in the internal list
		plstItem->push_back(pZdumpItem);
		// Set Utc Time
		pZdumpItem->setUtTime(tmUtc->tm_hour, tmUtc->tm_min, tmUtc->tm_sec);
		// Set Utc Date
		pZdumpItem->setUtDate(tmUtc->tm_mday, tmUtc->tm_mon + 1, tmUtc->tm_year + 1900);
		// Set Tz Time
		pZdumpItem->setTzTime(tmZone->tm_hour, tmZone->tm_min, tmZone->tm_sec);
		// Set Tz Date
		pZdumpItem->setTzDate(tmZone->tm_mday, tmZone->tm_mon + 1, tmZone->tm_year + 1900);
		// Set is DST
		pZdumpItem->setDst(tmZone->tm_isdst);
		// Set offset
		pZdumpItem->setGmtOffset(tmZone->tm_gmtoff);
	}
}

/*
 * MktrDate.h
 *
 *  Created on: Sep 1, 2010
 *      Author: xludesi
 */

#ifndef MKTRDATE_H_
#define MKTRDATE_H_

#include <time.h>
#include <iostream>

class MktrDate
{
public:
	// Constructors
    MktrDate();
    MktrDate(const time_t time);
    MktrDate(const MktrDate& orig);

    // Destructor
	virtual ~MktrDate();

    int year() const;
    int month() const;
    int day() const;
    int hour() const;
    int minute() const;
    void moveForward(unsigned long minutes);
    void moveBackword(unsigned long minutes);

    bool operator==(const MktrDate& right);
    bool operator!=(const MktrDate& right);
    bool operator<(const MktrDate& right);
    bool operator<=(const MktrDate& right);
    bool operator>(const MktrDate& right);
    bool operator>=(const MktrDate& right);
    MktrDate& operator=(const MktrDate& orig);

    std::string to_string();

    static MktrDate* create(const unsigned int year, const unsigned int month, const unsigned int day, const unsigned int hour, const unsigned int minute);

private:
    unsigned int _iyear;
    unsigned int _imonth;
    unsigned int _iday;
    unsigned int _ihour;
    unsigned int _iminute;
    int _century();

    MktrDate(const unsigned int year, const unsigned int month, const unsigned int day, const unsigned int hour, const unsigned int minute);
    static int _compareDates(const MktrDate& left, const MktrDate right);
};

#endif /* MKTRDATE_H_ */

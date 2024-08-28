/*********************************************************************************
 * This file is part of CUTE.
 *
 * CUTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CUTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with CUTE.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2007 Peter Sommerlad, Emanuel Graf
 *
 *********************************************************************************/
#ifndef ECLIPSE_LISTENER_H_
#define ECLIPSE_LISTENER_H_
#include "ostream_listener.h"
#include <iostream>
#include <iterator>
#include <algorithm>
namespace cute {

	class eclipse_listener
	{
	protected:
		struct blankToUnderscore{
            char operator()(char in){
			if (in == ' ') return '_';
			return in;
		}
        };
		std::string maskBlanks(const std::string &in) {
			std::string result;
			std::transform(in.begin(),in.end(),std::back_inserter(result),blankToUnderscore());
			return result;
		}
	public:
		int cnt_failure;
		int cnt_success;
		eclipse_listener() {}
		void start(test const &t){
			std::cout << std::endl << "#starting " <<t.name()<< std::endl;

		}

		void begin(suite const &t,char const *info){
			std::cout << std::endl << "#beginning " << info << " number of test " << t.size() << std::endl;
			cnt_failure = 0;
			cnt_success = 0;
		}
		void end(suite const &t, char const *info){
			  std::cout << std::endl << "#total test success: "<< cnt_success << " total test failure: "<< cnt_failure << std::endl;
			  std::cout << std::endl << "#ending " << info << std::endl;
		}
		void success(test const &t, char const *msg){
			std::cout << std::endl << "#success " <<  maskBlanks(t.name()) <<" " << msg<< std::endl;
			cnt_success++;
		}
		void failure(test const &t,test_failure const &e){
			std::cout << std::endl << "#failure " << maskBlanks(t.name()) << " " << e.filename << ":" << e.lineno << " " <<e.reason << std::endl;
			cnt_failure++;
		}
		void error(test const &t, char const *what){
			std::cout << std::endl << "#error " << maskBlanks(t.name()) << " " << what << std::endl;
		}
	};
}
#endif /*ECLIPSE_LISTENER_H_*/

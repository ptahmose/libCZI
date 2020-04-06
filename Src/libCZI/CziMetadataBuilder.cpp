//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017  Zeiss Microscopy GmbH
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// To obtain a commercial version please contact Zeiss Microscopy GmbH.
// 
//******************************************************************************
#include "stdafx.h"
#include "CziMetadataBuilder.h"
#include "utilities.h"
#include <regex>
#include <codecvt>
#include <iomanip>
#include "CziUtils.h"
#include "XmlNodeWrapper.h"

using namespace libCZI;
using namespace std;

//**************************************************************************************************

std::string libCZI::XmlDateTime::ToXmlString() const
{
	if (!this->IsValid())
	{
		throw std::invalid_argument("illegal date/time");
	}

	stringstream ss;
	ss << setfill('0') << setw(4) << this->year << '-' << setw(2) << this->mon << '-' << setw(2) << this->mday << 'T'
		<< setw(2) << this->hour << ':' << setw(2) << this->min << ':' << setw(2) << this->sec;
	if (this->isUTC == true)
	{
		ss << 'Z';
	}
	else if (this->HasTimeZoneOffset())
	{
		ss << ((this->offsetHours >= 0) ? "+" : "") << setw(2) << this->offsetHours << ':' << setw(2) << this->offsetMinutes;
	}

	return ss.str();
}

std::wstring libCZI::XmlDateTime::ToXmlWstring() const
{
	return Utilities::convertUtf8ToWchar_t(this->ToXmlString().c_str());
}

bool libCZI::XmlDateTime::IsValid() const
{
	return
		this->sec >= 0 && this->sec <= 60 &&
		this->min >= 0 && this->min <= 59 &&
		this->hour >= 0 && this->hour <= 23 &&
		this->year > -10000 && this->year < 10000 &&
		this->mon >= 1 && this->mon <= 12 &&
		this->mday >= 1 &&
		this->mday <= (this->mon == 2 ? (((this->year % 100 != 0 && this->year % 4 == 0) || (this->year % 400 == 0)) ? 29 : 28) :
			this->mon == 9 || this->mon == 4 || this->mon == 6 || this->mon == 11 ? 30 : 31);
}

/*static*/bool libCZI::XmlDateTime::TryParse(const wchar_t* szw, libCZI::XmlDateTime* ptrDateTime)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	return XmlDateTime::TryParse(utf8_conv.to_bytes(szw).c_str(), ptrDateTime);
}

/*static*/bool libCZI::XmlDateTime::TryParse(const char* sz, libCZI::XmlDateTime* ptrDateTime)
{
	// cf. https://www.oreilly.com/library/view/regular-expressions-cookbook/9781449327453/ch04s07.html
	//static const char* regexStr = "^(?<year>-?(?:[1-9][0-9]*)?[0-9]{4})-(?<month>1[0-2]|0[1-9])-(?<day>3[01]|0[1-9]|[12][0-9])T(?<hour>2[0-3]|[01][0-9]):(?<minute>[0-5][0-9]):(?<second>[0-5][0-9])(?<ms>\.[0-9]+)?(?<timezone>Z|[+-](?:2[0-3]|[01][0-9]):[0-5][0-9])?$";
	static const char* regexStr = R"((-?(?:[1-9][0-9]*)?[0-9]{4})-(1[0-2]|0[1-9])-(3[01]|0[1-9]|[12][0-9])T(2[0-3]|[01][0-9]):([0-5][0-9]):([0-5][0-9])(\.[0-9]+)?(Z|[+-](?:2[0-3]|[01][0-9]):[0-5][0-9])?)";

	auto str = Utilities::Trim(sz);
	regex dateTimeRegex(regexStr);
	smatch pieces_match;
	if (regex_match(str, pieces_match, dateTimeRegex) && pieces_match.size() == 9)
	{
		XmlDateTime dateTime;
		dateTime.Clear();

		if (pieces_match[1].matched)
		{
			dateTime.year = std::stoi(pieces_match[1]);
		}

		if (pieces_match[2].matched)
		{
			dateTime.mon = std::stoi(pieces_match[2]);
		}

		if (pieces_match[3].matched)
		{
			dateTime.mday = std::stoi(pieces_match[3]);
		}

		if (pieces_match[4].matched)
		{
			dateTime.hour = std::stoi(pieces_match[4]);
		}

		if (pieces_match[5].matched)
		{
			dateTime.min = std::stoi(pieces_match[5]);
		}

		if (pieces_match[6].matched)
		{
			dateTime.sec = std::stoi(pieces_match[6]);
		}

		// note that we skip the decimal places of "second"...

		if (pieces_match[8].matched)
		{
			auto str = pieces_match[8].str();
			char _1stChar = str[0];
			if (_1stChar == 'Z')
			{
				dateTime.isUTC = true;
			}
			else if ((_1stChar == '+' || _1stChar == '-') && str.length() >= 6)
			{
				dateTime.offsetHours = std::stoi(str.substr(0, 3));
				dateTime.offsetMinutes = std::stoi(str.substr(4, 2));
			}
		}

		if (dateTime.IsValid())
		{
			if (ptrDateTime != nullptr)
			{
				*ptrDateTime = dateTime;
			}

			return true;
		}
	}

	return false;
}

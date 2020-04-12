#pragma once

#include "inc_libCZI.h"

template<typename input_iterator>
void CalcHash(std::uint8_t* ptrHash, input_iterator begin, input_iterator end)
{
	memset(ptrHash, 0, 16);
	for (input_iterator it = begin; it != end; ++it)
	{
		std::uint8_t hash[16];
		libCZI::Utils::CalcMd5SumHash(it->c_str(), it->length(), hash, 16);
		for (uint8_t i = 0; i < 16; ++i)
		{
			*(ptrHash + i) ^= hash[i];
		}
	}
}


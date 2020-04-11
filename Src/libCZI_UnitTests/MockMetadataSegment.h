#pragma once

#include "inc_libCZI.h"

class MockMetadataSegment : public libCZI::IMetadataSegment
{
public:
	enum class Type
	{
		Data1,
		Data2,
		Data3,
		Data4,
		Data5,
		Data6,
		InvalidData
	};
private:
	std::string xmlData;
public:
	MockMetadataSegment(Type type = Type::Data1);
	virtual std::shared_ptr<const void> GetRawData(MemBlkType type, size_t* ptrSize);
	virtual void DangerousGetRawData(MemBlkType type, const void*& ptr, size_t& size) const;
};

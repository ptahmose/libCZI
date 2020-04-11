#include "pch.h"
#include "testImage.h"
#include "inc_libCZI.h"

using namespace libCZI;

static std::shared_ptr<IBitmapData> CreateTestImage()
{
	auto bm = CBitmapData<CHeapAllocator>::Create(PixelType::Bgr24, CTestImage::BGR24TESTIMAGE_WIDTH, CTestImage::BGR24TESTIMAGE_HEIGHT);
	ScopedBitmapLockerSP lck{ bm };
	CTestImage::CopyBgr24Image(lck.ptrDataRoi, bm->GetWidth(), bm->GetHeight(), lck.stride);
	return bm;
}

TEST(BitmapOperations, NNResize1)
{
	static const std::uint8_t expectedResult[16] = { 0xf2,0xa1,0x4b,0x86,0xa5,0x0e, 0xdb,0x95,0xed,0x8a,0x67,0x6c,0x69,0xab,0x2b,0x26 };

	auto srcBm = CreateTestImage();
	auto bm = CBitmapData<CHeapAllocator>::Create(srcBm->GetPixelType(), 100, 100);
	CBitmapOperations::NNResize(srcBm.get(), bm.get());
	std::uint8_t hash[16];
	CBitmapOperations::CalcMd5Sum(bm.get(), hash, sizeof(hash));
	int c = memcmp(expectedResult, hash, sizeof(hash));
	EXPECT_EQ(c, 0);
}

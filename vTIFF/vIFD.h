#pragma once
#include "Utils.h"
#include <string>
#include <vector>
class vIFD
{
	byte* p_Data;
	byte** imageData;
	int ByteOrder;
	int ImageWidth = 0;
	int ImageLength = 0;

	std::vector<int> BitsPerSample;
	int PixelBytes = 0;
	int Compression = 0;

	/// <summary>
	/// 0 = WhiteIsZero. For bilevel and grayscale images: 0 is imaged as white. The maximum value is imaged as black. This is the normal value for Compression=2.
	/// 1 = BlackIsZero. For bilevel and grayscale images: 0 is imaged as black. The maximum value is imaged as white. If this value is specified for Compression=2, the
	/// image should display and print reversed.
	/// </summary>
	int PhotometricInterpretation = 0;

	/// <summary>
	///For each strip, the byte offset of that strip
	/// </summary>
	std::vector<int> StripOffsets;//For each strip, the byte offset of that strip

   /// <summary>
   ///The number of rows in each strip (except possibly the last strip.)
   ///For example, if ImageLength is 24, and RowsPerStrip is 10, then there are 3
   ///strips, with 10 rows in the first strip, 10 rows in the second strip, and 4 rows in the
   ///third strip. (The data in the last strip is not padded with 6 extra rows of dummy data.)
   /// </summary>
	int RowsPerStrip = 0;

	/// <summary>
	///For each strip, the number of bytes in that strip after any compression.
	/// </summary>
	std::vector<int> StripByteCounts;
	float XResolution;
	float YResolution;
	int ResolutionUnit = 0;
	int Predictor = 0;
	std::vector<int> SampleFormat;
	std::string DateTime = "";
	std::string Software = "";

	int StripCount;

	int GetInt(int startPos, int Length);
	float GetRational(int startPos);
	float GetFloat(byte* b, int startPos);
	std::string GetString(int startPos, int Length);
	void GetIntArray(int startPos, int typeSize, int count, std::vector<int>& arr);

	void DecodeDE(int n);
	
	void GetDEValue(int TagIndex, int TypeIndex, int Count, int pdata);
	int (*pGetInt)(byte* pd, int startPos, int Length);
public:
	vIFD(byte* p_data, bool byteorder, int(*fun)(byte*, int, int));
	~vIFD();
	int Decode(int p);
	byte** GetImageData() {
		return imageData;
	}
	float* GetPixel(int x, int y);//每个通道占四个byte//32位float
	byte* GetPixelByte(int x, int y);//每个通道占一个byte//8位
	void PrintInfo();
	void DecodeStrips();
	int GetImageWidth() { return ImageWidth; }
	int GetImageLength() { return ImageLength; }
	int GetPass() { return BitsPerSample.size(); }
};


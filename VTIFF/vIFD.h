#pragma once
#include "Utils.h"
#include <string>
#include <vector>
enum vFormat;
class vIFD
{
	byte* p_Data;
	byte* imageData;
	int ByteOrder;
	int ImageWidth = 0;
	int ImageLength = 0;

	std::vector<int> BitsPerSample;
	int BytesPerSample =0;
	int ChannelCount = 0;
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
	int byteCountPerStripe;

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
	byte* GetImageData() {
		return imageData;
	}
	void* GetPixel(int x, int y);//每个通道占四个byte//32位float
	byte* GetPixelByte(int x, int y);//每个通道占一个byte//8位
	void PrintInfo();
	
	int GetImageWidth() { return ImageWidth; }
	int GetImageLength() { return ImageLength; }
	int GetPass() { return BitsPerSample.size(); }
	vFormat GetFormat();

	void DecodeImage();
private:
	template<class T>
	void DecodeStrips();

	template<class T>
	void DealPredictor();
};

enum vFormat
{
	VT_UNSIGNED_BYTE = 1,//8-bits
	VT_UNSIGNED_SHORT,//16-bits
	VT_UNSIGNED_INT,//32-bits

	VT_BYTE=4,  
	VT_SHORT,
	VT_INTE,
	
	VT_HALF_FLOAT=7,
	VT_FLOAT,//32-bits
	VT_DOUBLE,//64-bits


	VT_UNDEFINED
};
//Byte
//VT_BITMAP,// standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit, unsigned byte
//真彩色
//VT_RGB16, // 48-bit RGB image			: 3 x 16-bit
//VT_RGBA16,// 64-bit RGBA image		: 4 x 16-bit

//VT_RGBF,// 96-bit RGB float image	    : 3 x 32-bit IEEE floating point
//VT_RGBAF// 128-bit RGBA float image	: 4 x 32-bit IEEE floating point
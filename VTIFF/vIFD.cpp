#include "vIFD.h"
#include "Utils.h"
#include "CompressionLZW.h"
#include <iostream>



vIFD::vIFD(byte* p_data, bool byteorder, int(*fun)(byte* ,int, int) ) :
	p_Data(p_data),
	imageData(nullptr),
	ByteOrder(byteorder),
	/*BitsPerSample(nullptr),
	StripOffsets(nullptr),
	StripByteCounts(nullptr),
	SampleFormat(nullptr),*/
	pGetInt(fun)
{

}
vIFD::~vIFD()
{
	if (imageData != nullptr)
	{
		/*for (int i = 1; i < StripCount; i++)
		{
			delete[] imageData[i];
		}*/
		delete[] imageData;
	}
}
int vIFD::Decode(int Pos)
{
	int n = Pos;
	int DECount = pGetInt(p_Data, n, 2);
	n += 2;
	for (int i = 0; i < DECount; i++)
	{
		DecodeDE(n);
		n += 12;
	}
	/*也可以之后获取指定层的时候解码
	//已获得每条扫描线位置，大小，压缩方式和数据类型，接下来进行解码
	DecodeStrips();
	*/
	int pNext = pGetInt(p_Data, n, 4);
	return pNext;
}
void vIFD::DecodeDE(int Pos)
{
	int TagIndex = pGetInt(p_Data, Pos, 2);
	int TypeIndex = pGetInt(p_Data, Pos + 2, 2);
	int Count = pGetInt(p_Data, Pos + 4, 4);
	//Debug.Log("Tag: " + Tag(TagIndex) + " DataType: " + TypeArray[TypeIndex].name + " Count: " + Count);

	//先把找到数据的位置
	int pData = Pos + 8;
	int totalSize = TypeArray[TypeIndex] * Count;
	if (totalSize > 4)
		pData = GetInt(pData, 4);

	//再根据Tag把值读出并存起来
	GetDEValue(TagIndex, TypeIndex, Count, pData);
}
void vIFD::GetDEValue(int TagIndex, int TypeIndex, int Count, int pdata)
{
	int typesize = TypeArray[TypeIndex];
	switch (TagIndex)
	{
	case 254: break;//NewSubfileType
	case 255: break;//SubfileType
	case 256://ImageWidth
		ImageWidth = GetInt(pdata, typesize); break;
	case 257://ImageLength
		ImageLength = GetInt(pdata, typesize); break;
	case 258://BitsPerSample
		GetIntArray(pdata, typesize, Count, BitsPerSample); break;
	case 259: //Compression
		Compression = GetInt(pdata, typesize); break;
	case 262: //PhotometricInterpretation
		PhotometricInterpretation = GetInt(pdata, typesize); break;
	case 273://StripOffsets
		GetIntArray(pdata, typesize, Count, StripOffsets); StripCount = Count; break;
	case 274: break;//Orientation
	case 277: break;//SamplesPerPixel
	case 278://RowsPerStrip
		RowsPerStrip = GetInt(pdata, typesize); break;
	case 279://StripByteCounts
		GetIntArray(pdata, typesize, Count, StripByteCounts); break;
	case 282: //XResolution
		XResolution = GetRational(pdata); break;
	case 283://YResolution
		YResolution = GetRational(pdata); break;
	case 284: break;//PlanarConfig
	case 296://ResolutionUnit
		ResolutionUnit = GetInt(pdata, typesize); break;
	case 305://Software
		Software = GetString(pdata, typesize); break;
	case 306://DateTime
		DateTime = GetString(pdata, typesize); break;
	case 315: break;//Artist
	case 317: //Differencing Predictor
		Predictor = GetInt(pdata, typesize); break;
	case 320: break;//ColorDistributionTable
	case 338: break;//ExtraSamples
	case 339: //SampleFormat
		GetIntArray(pdata,typesize,Count, SampleFormat); break;
	default: break;
	}
}
void vIFD::DecodeStrips()
{
		int pStrip = 0;
		int size = 0;
		byteCountPerStripe = ImageWidth * RowsPerStrip * BitsPerSample.size()*BitsPerSample[0]/8;
		imageData =new byte[StripCount* byteCountPerStripe];
		
		if (Compression == 5)
		{
			Timer time("LZW Decode Time");
			CompressionLZW* lzw = new CompressionLZW();
			for (int i = 0; i < StripCount; i++)
			{
				pStrip = StripOffsets[i];
				size = StripByteCounts[i];
				//imageData[i] = new byte[byteCountPerStripe];
				lzw->Decode(p_Data,pStrip,size, &imageData[i*byteCountPerStripe]);
			}
			delete lzw;
		}
		
}
void vIFD::PrintInfo()
{
	/*std::cout << "ImageWidth: " << ImageWidth << std::endl;
	std::cout << "ImageLength: " << ImageLength << std::endl;*/
}
int vIFD::GetInt(int startPos, int Length)
{
	return pGetInt(p_Data, startPos, Length);
}
float vIFD::GetRational(int startPos)
{
	int A = GetInt(startPos, 4);
	int B = GetInt(startPos + 4, 4);
	return A / B;
}
std::string vIFD::GetString(int startPos, int Length)//II和MM对String没有影响
{
	std::string tmp = "";
	for (int i = 0; i < Length; i++)
		tmp += (char)p_Data[startPos];
	return tmp;
}
void vIFD::GetIntArray(int startPos,int typeSize,int count, std::vector<int>& arr)
{
	arr = std::vector<int>(count);
	//arr.resize(count);
	for (int i = 0; i < count; i++)
	{
		int v = GetInt(startPos + i * typeSize, typeSize);
		arr[i] = v;
	}
}
float vIFD::GetFloat(byte* b, int startPos)
{
	byte* byteTemp;
	if (ByteOrder)// "II")
	    byteTemp = new byte[4]{ b[startPos],b[startPos + 1],b[startPos + 2],b[startPos + 3] };
	else
	    byteTemp = new byte[4]{ b[startPos + 3],b[startPos + 2],b[startPos + 1],b[startPos] };
	
	return *reinterpret_cast<float*>(byteTemp);
}
float* vIFD::GetPixel(int x, int y)
{
	byte* pStripY = &imageData[y *byteCountPerStripe];
	float R = GetFloat(pStripY, x * PixelBytes);
	float G = GetFloat(pStripY, x * PixelBytes+4);
	float B = GetFloat(pStripY, x * PixelBytes+8);
	float A = GetFloat(pStripY, x * PixelBytes+12);
	float* color = new float[4]{ R,G,B,A };
	return color;
}
byte* vIFD::GetPixelByte(int x, int y)
{
	byte* pStripY = &imageData[y* byteCountPerStripe];
	byte R = pStripY[x];
	byte G = pStripY[x + 1];
	byte B = pStripY[x + 2];
	byte A = pStripY[x + 3];
	byte* color = new byte[4]{ R,G,B,A };
	return color;
}
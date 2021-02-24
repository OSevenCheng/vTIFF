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
	ChannelCount = BitsPerSample.size();
	if (ChannelCount == 0)
	{
		//错误
		return 0;
	}
	BytesPerSample = BitsPerSample[0]/8;
	int mo = BitsPerSample[0] % 8;
	if (BytesPerSample == 0 || mo != 0)
	{
		//不支持 通道宽度不为8bits的整数倍
		return 0;
	}
	bool isSame = true;
	if (ChannelCount > 1)
	{
		for (int i = 1; i < ChannelCount; i++)
			isSame = isSame && (BitsPerSample[i] == BitsPerSample[i - 1]);
	}
	if (!isSame)
	{
		//不支持 各通道不一致的情况
		return 0;
	}
	if(BytesPerSample)
	if (SampleFormat.size() == 0)
	{
		SampleFormat.push_back(1);//默认为1
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
		GetIntArray(pdata,typesize,Count, SampleFormat); break;//默认为无符号整型
	default: break;
	}
}
void vIFD::DecodeImage()
{
	int BitsCount = BitsPerSample[0];
	switch (BitsCount)//不管有无符号，都按有符号处理
	{
	case 8://byte
		DecodeStrips<char>();
		break;
	case 16://short
		DecodeStrips<short>();
		break;
	case 32:
		if (SampleFormat[0] == 3)//float
		{
			DecodeStrips<float>();
		}
		else//int
		{
			DecodeStrips<int>();
		}
		break;
	case 64:
		DecodeStrips<double>();
		break;
	default:
		break;
	}
	hasDecode = true;
}
template<class T>
void vIFD::DealPredictor()
{
	T* sData = reinterpret_cast<T*>(imageData);
	for (int j = 0; j < ImageLength; j++)
	{
		int before = j * ImageWidth * 3;
		for (int ch = 0; ch < BitsPerSample.size(); ch++)
		{
			char last = 0;
			for (int i = before + ch; i < before + ch + ImageWidth * 3; i += 3)
			{
				sData[i] += last;
				last = sData[i];
			}
		}
	}
}

template<class T>
void vIFD::DecodeStrips()
{
	int pStrip = 0;
	int size = 0;
	byteCountPerStripe = ImageWidth * RowsPerStrip * ChannelCount * BitsPerSample[0]/8;//解码后的宽度
	imageData = new byte[StripCount* byteCountPerStripe];
	if (Compression == 1)//No compression
	{
		for (int i = 0; i < StripCount; i++)
		{
			pStrip = StripOffsets[i];
			size = StripByteCounts[i];
			//imageData[i * byteCountPerStripe] = p_Data[pStrip];
			memcpy(&imageData[i * byteCountPerStripe], &p_Data[pStrip], size);
		}
	}
	else if (Compression == 5)//LZW compression
	{
		//Timer time("LZW Decode Time");
		CompressionLZW* lzw = new CompressionLZW();
		for (int i = 0; i < StripCount; i++)
		{
			pStrip = StripOffsets[i];
			size = StripByteCounts[i];

			lzw->Decode(p_Data,pStrip,size, &imageData[i*byteCountPerStripe]);
				
		}
		delete lzw;
		if (Predictor == 2)
		{
			DealPredictor<T>();
		}
	}
		
}

void* vIFD::GetPixel(int x, int y)
{
	int f = SampleFormat[0];
	int b = BitsPerSample[0] / 8;
	int c = BitsPerSample.size();
	void* color = nullptr;
	auto func_Char = [](byte* data,int x,int y, int c,int w) {
		char* sData = reinterpret_cast<char*>(data);
		return (void*)&sData[x + y * w * c];
	};
	auto func_Short = [](byte* data, int x, int y, int c, int w) {
		short* sData = reinterpret_cast<short*>(data);
		return (void*)&sData[x + y * w * c];
	};
	auto func_Int = [](byte* data, int x, int y, int c, int w) {
		int* sData = reinterpret_cast<int*>(data);
		return (void*)&sData[x + y * w * c];
	};
	void* (*func_ptr[3])(byte * data, int x, int y ,int c, int w)
		= { func_Char, func_Short, func_Int };
	return func_ptr[b - 1](imageData, x, y, c, ImageWidth);
}
//float* vIFD::GetPixel(int x, int y)
//{
//	float* color = nullptr;
//	if (GetFormat() == vFormat::VT_FLOAT && GetPass() == 4)
//	{
//		byte* pStripY = &imageData[y * byteCountPerStripe];
//		float R = GetFloat(pStripY, x * PixelBytes);
//		float G = GetFloat(pStripY, x * PixelBytes + 4);
//		float B = GetFloat(pStripY, x * PixelBytes + 8);
//		float A = GetFloat(pStripY, x * PixelBytes + 12);
//		color = new float[4]{ R,G,B,A };
//	}
//	else if (GetFormat() == vFormat::VT_UNSIGNED_BYTE && GetPass() == 3)
//	{
//		byte* pStripY = &imageData[y * byteCountPerStripe];
//		float R = (float)pGetInt(pStripY, x * PixelBytes, 1) / 255.0;
//		float G = (float)pGetInt(pStripY, x * PixelBytes + 1, 1) / 255.0;
//		float B = (float)pGetInt(pStripY, x * PixelBytes + 2, 1) / 255.0;
//		color = new float[3]{ R,G,B };
//	}
//	return color;
//}
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

vFormat vIFD::GetFormat()
{
	/*int f = SampleFormat.size() == 0 ? VT_UNSIGNED_BYTE : (SampleFormat[0]*3-2);
	int f2 = BitsPerSample[0] == 8
	if(BitsPerSample[0] == 8)
		return vFormat::*/
	int f = SampleFormat.size() == 0 ? 1 : SampleFormat[0];
	int b = BitsPerSample[0];
	vFormat format = VT_UNDEFINED;
	if (f == 1)
	{
		if (b == 8)//无符号整型
		{
			format = VT_UNSIGNED_BYTE;
		}
		else if (b == 16)
		{
			format = VT_UNSIGNED_SHORT;//16-bits
		}
		else if (b == 32)
		{
			format = VT_UNSIGNED_INT;//32-bits
		}
	}
	else if (f == 2)//有符号整型
	{
		if (b == 8)
		{
			format = VT_BYTE;
		}
		else if (b == 16)
		{
			format = VT_SHORT;
		}
		else if (b == 32)
		{
			format = VT_INTE;
		}
	}
	else if (f == 3)
	{
		if (b == 16)
		{
			format = VT_HALF_FLOAT;
		}
		else if (b == 32)
		{
			format = VT_FLOAT; //32-bits
		}
		else if (b == 64)
		{
			format = VT_DOUBLE;//64-bits
		}
	}

	return format;
}
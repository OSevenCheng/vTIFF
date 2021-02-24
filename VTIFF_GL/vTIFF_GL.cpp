#include "vTIFF_GL.h"
vTIFF_GL::vTIFF_GL()
{

}
vTIFF_GL::vTIFF_GL(std::string path)
{
	//LoadFile(path);
}
vTIFF_GL::~vTIFF_GL() 
{

}
bool vTIFF_GL::LoadFile(std::string path)
{
	if (!Load(path))
	{
		return false;//文件不存在
	}
	SetFormat();
	return true;
}
void vTIFF_GL::SetFormat()
{
	int channelNum = Pass();
	vFormat f = Format();
	if (channelNum == 1)
	{

	}
	else if (channelNum == 3)
	{
		switch (f)
		{
		case vFormat::VT_UNSIGNED_BYTE:
			inFormat = GL_RGB8;//无符号归一化
			exFormat = GL_RGB;//归一化
			exType = GL_UNSIGNED_BYTE;
			break;
		case vFormat::VT_UNSIGNED_SHORT:
			inFormat = GL_RGB16;//无符号归一化
			exFormat = GL_RGB;//归一化
			exType = GL_UNSIGNED_SHORT;
			break;
		case vFormat::VT_UNSIGNED_INT:
			inFormat = GL_RGB32UI;//无符号不归一化
			exFormat = GL_RGB_INTEGER;//不归一化
			exType = GL_UNSIGNED_INT;
			break;
		case vFormat::VT_BYTE:
			inFormat = GL_RGB8_SNORM;//有符号归一化
			exFormat = GL_RGB;//归一化
			exType = GL_UNSIGNED_BYTE;
			break;
		case vFormat::VT_SHORT:
			inFormat = GL_RGB16_SNORM;//有符号归一化
			exFormat = GL_RGB;//归一化
			exType = GL_UNSIGNED_SHORT;
			break;
		case vFormat::VT_INTE:
			inFormat = GL_RGB32I;//有符号不归一化
			exFormat = GL_RGB_INTEGER;//不归一化
			exType = GL_UNSIGNED_INT;
			break;
		case vFormat::VT_HALF_FLOAT:
			inFormat = GL_RGB16F;//有符号归一化
			exFormat = GL_RGB;//归一化
			exType = GL_HALF_FLOAT;
			break;
		case vFormat::VT_FLOAT:
			inFormat = GL_RGB32F;//有符号归一化
			exFormat = GL_RGB;//归一化
			exType = GL_FLOAT;
			break;
		case vFormat::VT_DOUBLE:
			inFormat = GL_RGB32F;//最大32
			exFormat = GL_RGB;//归一化
			exType = GL_DOUBLE;
			break;
		}
	}
	else if (channelNum == 4)
	{
		switch (f)
		{
		case vFormat::VT_UNSIGNED_BYTE:
			inFormat = GL_RGBA8;//无符号归一化
			exFormat = GL_RGBA;//归一化
			exType = GL_UNSIGNED_BYTE;
			break;
		case vFormat::VT_UNSIGNED_SHORT:
			inFormat = GL_RGBA16;//无符号归一化
			exFormat = GL_RGBA;//归一化
			exType = GL_UNSIGNED_SHORT;
			break;
		case vFormat::VT_UNSIGNED_INT:
			inFormat = GL_RGBA32UI;//无符号不归一化
			exFormat = GL_RGBA_INTEGER;//不归一化
			exType = GL_UNSIGNED_INT;
			break;
		case vFormat::VT_BYTE:
			inFormat = GL_RGBA8_SNORM;//有符号归一化
			exFormat = GL_RGBA;//归一化
			exType = GL_UNSIGNED_BYTE;
			break;
		case vFormat::VT_SHORT:
			inFormat = GL_RGBA16_SNORM;//有符号归一化
			exFormat = GL_RGBA;//归一化
			exType = GL_UNSIGNED_SHORT;
			break;
		case vFormat::VT_INTE:
			inFormat = GL_RGBA32I;//有符号不归一化
			exFormat = GL_RGBA_INTEGER;//不归一化
			exType = GL_UNSIGNED_INT;
			break;
		case vFormat::VT_HALF_FLOAT:
			inFormat = GL_RGBA16F;//有符号归一化
			exFormat = GL_RGBA;//归一化
			exType = GL_HALF_FLOAT;
			break;
		case vFormat::VT_FLOAT:
			inFormat = GL_RGBA32F;//有符号归一化
			exFormat = GL_RGBA;//归一化
			exType = GL_FLOAT;
			break;
		case vFormat::VT_DOUBLE:
			inFormat = GL_RGBA32F;//最大32
			exFormat = GL_RGBA;//归一化
			exType = GL_DOUBLE;
			break;
		}
	}
}
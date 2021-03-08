#pragma once
#include"glew.h"
#include<GL/GL.h>
#include<GL/GLU.h>
#include<vTIFF.h>
class vTIFF_GL:private vTIFF
{
private:
	GLint   inFormat;//推荐使用的内部格式
	GLenum  exFormat;
	GLenum  exType;
	void* pData;
	void SetFormat();
public:
	GLint InFormat() { return inFormat; }
	GLsizei Wid() { return Width(); }
	GLsizei Hig() { return Height(); }
	GLsizei Lay() { return Layer(); }
	GLenum ExFormat() { return exFormat; }
	GLenum ExType() { return exType; }
	void* GetData(int i) { return GetLayer(i); }
	vTIFF_GL();
	vTIFF_GL(std::string path);
	~vTIFF_GL();
	bool LoadFile(std::string path);
	
};


#include<iostream>
#include"TIFF.h"
using namespace std;
int main()
{
	TIFF* tiff = new TIFF("D:\\Projects\\Unity\\Streamlines\\Assets\\Streamlines\\Data\\1.tif");
	
	//获取某一层的数据。返回二维字节数组
	byte** layer0 = tiff->GetLayer(0);

	//获取某一层的某个像素的数据。
	//for (int x = 0; x < 1440; x++)
	//{
		float* color = tiff->GetPixel(720, 360);
		cout << color[0] << "	" << color[1] << "	" << color[2] << "	" << color[3] << endl;
	//}
	
	return 0;
}
// cxi.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
CxImage mask(CxImage , BOOL = true);
int isMaskable(CxImage);
int	doList(int);
TCHAR* FindExtension(const TCHAR *);

int main(int argc, _TCHAR* argv[])
{
	int NegativeCfg=1;
	cout << "GALアウトローtool: ●masker 0.91(@fans)\nCxImage version: 6.0.0"  << endl;
	cout << "该目录下的文件将处理后置于OutPng文件夹中█" <<endl<<endl;
	cout << "反色设置(1.自动检测;2.反色;3.不反色):";
	CreateDirectory("OutPng",NULL);

	cin >> NegativeCfg;
	doList(NegativeCfg);

	cout << "task finished!";
	system("pause >nul");
}


/*************************
*       遮罩函数          *
*************************/
CxImage mask(CxImage input_Image, BOOL Negative)
{
	if(!input_Image.IsValid())
		return 0;

	CxImage basic,alpha;
	RECT rect;
	long width=input_Image.GetWidth()/2,
		height=input_Image.GetHeight();

	rect.top = 0;rect.left = 0;
	rect.right = width;
	rect.bottom = height;
	input_Image.Crop(rect,&basic);

	rect.top = 0;rect.left = width;
	rect.right = width*2;
	rect.bottom = height;
	input_Image.Crop(rect,&alpha);

	//在这里创建遮罩层
	if(!basic.AlphaIsValid())
		basic.AlphaCreate();           //创建透明通道
	if(Negative)
		alpha.Negative();              //遮罩层反色处理

	RGBQUAD rgb;
	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			rgb = alpha.GetPixelColor(x,y,0);
			basic.AlphaSet(x,y,rgb.rgbRed);
		}
	}
	return basic;
}

/*************************
*    判断是否含遮罩块      *
*************************/
int isMaskable(CxImage input_Image)
{
	//灰度255:白色
	//0:不含遮罩块 1:需要反色 2:不需要反色
	int grayC,grayR;
	if(!input_Image.IsValid())
		return 0;
	grayC=(int)input_Image.GetPixelGray(input_Image.GetWidth()/2,0);  //取中间点的灰度值
	grayR=(int)input_Image.GetPixelGray(input_Image.GetWidth()-1,0);  //取最右点的灰度值
	if((grayC==255)&&(grayR==255))
		return 1;
	if((grayC==0)&&(grayR==0))
		return 2;
	
	return 0;
}

TCHAR* FindExtension(const TCHAR * name)
{
	int len = _tcslen(name);
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return (TCHAR*)(name+i+1);
		}
	}
	return (TCHAR*)(name+len);
}

int	doList(int NegativeCfg)
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	string File,type;
	bool Negative;

	hSearch = FindFirstFile("*.*",&FileData);
	if(hSearch!=INVALID_HANDLE_VALUE)
	{
		do
		{
			if(FileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) continue;  //排除文件夹
			File = FileData.cFileName;
			type = FindExtension(File.data());                         //获取文件扩展名          //另一种方法:File.substr(File.find(".")+1);

			//处理文件
			if(!type.compare("bmp")||!type.compare("jpg")||!type.compare("png"))
			{
				CxImage image;
				if(image.Load(File.data(),CxImage::GetTypeIdFromName(type.data()))&&isMaskable(image))
				{
					int maska = isMaskable(image);
					if(NegativeCfg==1)
					{
						if(maska==1) Negative=true;
						else Negative=false;
					}
					else if(NegativeCfg==2)
						Negative=true;
					else if(NegativeCfg==3)
						Negative=false;
					else return 0;
					cout << "处理文件●" << File.data() << endl;
					File=File.substr(0,File.find("."));
					File+=".png";
					File.insert(0,"OutPng\\");

					image=mask(image,Negative);                          //遮罩处理
					image.Save(File.data(),CXIMAGE_FORMAT_PNG);
				}
			}
		}while(::FindNextFile(hSearch, &FileData));
		FindClose(hSearch);
	}
	return 1;
}

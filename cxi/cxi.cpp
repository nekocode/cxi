// cxi.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
CxImage mask(CxImage , BOOL = true);
int isMaskable(CxImage);
int	doList(int);
TCHAR* FindExtension(const TCHAR *);

int main(int argc, _TCHAR* argv[])
{
	int NegativeCfg=1;
	cout << "GAL�����ȥ�`tool: ��masker 0.91(@fans)\nCxImage version: 6.0.0"  << endl;
	cout << "��Ŀ¼�µ��ļ������������OutPng�ļ����Ш�" <<endl<<endl;
	cout << "��ɫ����(1.�Զ����;2.��ɫ;3.����ɫ):";
	CreateDirectory("OutPng",NULL);

	cin >> NegativeCfg;
	doList(NegativeCfg);

	cout << "task finished!";
	system("pause >nul");
}


/*************************
*       ���ֺ���          *
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

	//�����ﴴ�����ֲ�
	if(!basic.AlphaIsValid())
		basic.AlphaCreate();           //����͸��ͨ��
	if(Negative)
		alpha.Negative();              //���ֲ㷴ɫ����

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
*    �ж��Ƿ����ֿ�      *
*************************/
int isMaskable(CxImage input_Image)
{
	//�Ҷ�255:��ɫ
	//0:�������ֿ� 1:��Ҫ��ɫ 2:����Ҫ��ɫ
	int grayC,grayR;
	if(!input_Image.IsValid())
		return 0;
	grayC=(int)input_Image.GetPixelGray(input_Image.GetWidth()/2,0);  //ȡ�м��ĻҶ�ֵ
	grayR=(int)input_Image.GetPixelGray(input_Image.GetWidth()-1,0);  //ȡ���ҵ�ĻҶ�ֵ
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
			if(FileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) continue;  //�ų��ļ���
			File = FileData.cFileName;
			type = FindExtension(File.data());                         //��ȡ�ļ���չ��          //��һ�ַ���:File.substr(File.find(".")+1);

			//�����ļ�
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
					cout << "�����ļ���" << File.data() << endl;
					File=File.substr(0,File.find("."));
					File+=".png";
					File.insert(0,"OutPng\\");

					image=mask(image,Negative);                          //���ִ���
					image.Save(File.data(),CXIMAGE_FORMAT_PNG);
				}
			}
		}while(::FindNextFile(hSearch, &FileData));
		FindClose(hSearch);
	}
	return 1;
}

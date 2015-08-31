# README

批处理当前目录下所有 `带遮罩图的 24bit 图片` *（常见于比较旧的游戏资源图片）*  转换为 `带 alpha 通道的 32bit png 格式图片`

### kernel algorithm
```c++
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
```

![效果图](http://git.oschina.net/uploads/images/2015/0831/171346_f470c33d_8742.png "在这里输入图片标题")
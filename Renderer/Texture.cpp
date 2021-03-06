#include"Texture.h"
#include"ShapeRenderer.h"

#include<math.h>
//缓冲区
static DataBlock fileDataBlock;//图片文件数据缓冲
static Bitmap_32bit bitmap;//图像数据,用于传递给显卡或第三方库
Texture Texture::emptyTex;

typedef Texture::numType numType;

Texture::Texture():width(0),height(0){}
Texture::~Texture(){}
FontTexture::FontTexture():charCode(0){}

//创建纹理
void Texture::texImage2D(int width,int height,const void *pixels){
	if(!glIsTexture(tex2D)){//防止多次申请从而导致原tex2D值丢失
		glGenTextures(1,&tex2D);//申请纹理序号
	}
	glBindTexture(GL_TEXTURE_2D,tex2D);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
	this->width=width;
	this->height=height;
}
bool Texture::hasTexture()const{return glIsTexture(tex2D);}
void Texture::deleteTexture(){
	//glIsTexture(tex2D);在驱动有问题的情况下返回值会有问题,这里的处理方法是直接删除
	glDeleteTextures(1,&tex2D);
	tex2D=0;
}
//创建纹理(根据不同的类)
void Texture::texImage2D(const FileBMP &fileBmp){
	fileBmp.decodeTo(bitmap);
	texImage2D(bitmap);
}
void Texture::texImage2D(const FilePNG &filePng){
	filePng.decodeTo(bitmap);
	texImage2D(bitmap);
}
void Texture::texImage2D(const Pixmap<bool> &pixmap){
	bitmap.coordinateType=Bitmap_32bit::CoordinateType_Math;
	pixmap.toBitmap(bitmap);
	texImage2D(bitmap);
}
void Texture::texImage2D(const Pixmap<uint8> &pixmap){
	pixmap.toBitmap(bitmap);
	texImage2D(bitmap);
}
void Texture::texImage2D(const Bitmap_32bit &bitmap){
	texImage2D(bitmap.getWidth(),bitmap.getHeight(),bitmap.dataPointer);
}
//创建纹理(根据文件名)
void Texture::texImage2D_FileBMP(const string &filename){
	FileBMP fileBmp;
	fileBmp.set(fileDataBlock.loadFile(filename));
	fileBmp.parseData();
	texImage2D(fileBmp);
}
void Texture::texImage2D_FilePNG(const string &filename,WhenErrorString whenError){
	FilePNG filePng;
	filePng.set(fileDataBlock.loadFile(filename,whenError));
	filePng.parseData();
	texImage2D(filePng);
}
void Texture::texImage2D_FileName(const string &filename){
	//根据扩展名来识别
	auto pos=filename.find_last_of(".");
	auto suffix=filename.substr(pos);
	auto len=suffix.length();
	if(len!=4)return;//目前图片的扩展名包括点共4个
	for(decltype(len) i=0;i<len;++i){//转换成小写再比对
		suffix[i]=tolower(suffix[i]);
	}
	//开始比对
	if(suffix==".bmp"){
		texImage2D_FileBMP(filename);
	}else if(suffix==".png"){
		texImage2D_FilePNG(filename);
	}
}

static Point2D<numType> p2D_1_1=Point2D<numType>(1,1);

int Texture::getWidth()const{return width;}
int Texture::getHeight()const{return height;}
Point2D<int> Texture::size()const{return Point2D<int>(width,height);}
Point2D<numType> Texture::sizeF()const{return Point2D<numType>(width,height);}

Texture Texture::makeSolidTexture(int width, int height, const uint32 &u32){
	Texture tex;
	bitmap.newBitmap(width,height);
	bitmap.fillColor(u32);
	tex.texImage2D(bitmap);
	return tex;
}

void TextureCache::clearCache(){
	for(auto &item:*this){
		item.deleteTexture();
	}
	clear();
}
Texture TextureCache::getTexture(SizeType index)const{
	auto tex=data(index);
	return tex ? *tex : Texture();
}
void TextureCacheArray::clearCache(){
	for(auto &cache:*this){
		cache.clearCache();
	}
	clear();
}
Texture TextureCacheArray::getTexture(SizeType idxA,SizeType idxB)const{
	auto arr=data(idxA);
	return arr ? arr->getTexture(idxB) : Texture();
}
void TextureCache_String::clearCache(){
	for(auto &item:*this){
		item.value.deleteTexture();
	}
	clear();
}
Texture TextureCache_String::getTexture(const string &texName)const{
	auto tex=value(texName);
	return tex ? *tex : Texture();
}
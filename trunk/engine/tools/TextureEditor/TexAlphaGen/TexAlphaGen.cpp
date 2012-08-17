// TexAlphaGen.cpp : Defines the entry point for the console application.
//

#include <IL/il.h>
using namespace std;

/// A8R8G8B8 pixel
struct Pixel
{
	ILubyte B;
	ILubyte G;
	ILubyte R;
	ILubyte A;
};

void GenAlpha(const char* filename)
{
	// create image
	ILuint imageId;
	ilGenImages(1, &imageId);
	ilBindImage(imageId);
	// open file
	ILboolean success = ilLoadImage(filename);
	if (!success)
	{
		cout<<"failed to load file "<<filename<<endl;	
		return;
	}
	ILuint width = ilGetInteger(IL_IMAGE_WIDTH);
	ILuint height = ilGetInteger(IL_IMAGE_HEIGHT);
	ILubyte* data = ilGetData();
	Pixel* pixels = reinterpret_cast<Pixel*>(data);
	for (ILuint y = 0; y < height; y++)
	{
		for (ILuint x = 0; x < width; x++)
		{
			Pixel& pixel = pixels[y * width + x];
			pixel.A = max(pixel.R, max(pixel.G, pixel.B));
		}
	}
	// save to file
	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage(filename);
	// delete image
	ilDeleteImages(1, &imageId);
}

int main(int argc, char* argv[])
{
	if (1 == argc)
	{
		cout<<"please append a file name as a argument."<<endl;
		return 0;
	}
	ilInit();

	GenAlpha(argv[1]);

	ilShutDown();
	return 0;
}


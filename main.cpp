#include<iostream>
#include<cstring>

class Mask{
	private:
		int maskSize = 0;
		int** pixel;
		
	public:
		
		Mask()
		{
			pixel = new int*[13];
			for(int i = 0; i < 13; i++){ pixel[i] = new int[13]; memset(pixel[i], 0, 13); }
		}
		~Mask()
		{
			for(int i = 0; i < 13; i++){ delete [] pixel[i]; }
			delete [] pixel;
		}
		
		void setMaskSize(int size) { maskSize = size; }
		
		int getMaskPixel(int x, int y) { return pixel[x][y]; }
		
		int getMaskSize(){ return maskSize; }
		
		void setCrossMask()
		{
			int padding = (maskSize - 1)/2;
			
			for(int y = 0; y < maskSize; y++)
			{
				for(int x = 0; x < maskSize; x++)
				{
					if(x == padding || y == padding)
					{ pixel[x][y] = 1; }
				}
			}
		}
		
		void setFullMask()
		{	
			for(int y = 0; y < maskSize; y++)
			{
				for(int x = 0; x < maskSize; x++)
				{
					pixel[x][y] = 1;
				}
			}
		}
		
};

class Image
{	
	private:
		int** pixels;
		int imageX, imageY;
		
		bool checkIfTouch(int x, int y) const
		{
			if(pixels[x][y] == 1 || pixels[x-1][y] == 1 || pixels[x][y-1] == 1 || pixels[x+1][y] == 1 || pixels[x][y+1] == 1){ return true; }
			else { return false; }
		}
		
		//Por quest�es de otimiza��o, est� sendo usado o c�digo acima.
		/*bool checkIfTouch(int xIndex, int yIndex, Mask mask) const
		{
			for(int x = 0; x < mask.getMaskSize(); x++)
			{
				for(int y = 0; y < mask.getMaskSize() ; y++)
				{
					if(mask.getMaskPixel(x,y) && getPixel((xIndex-1) + x ,(yIndex-1) + y))
					return true;
				}
			}
			return false;
		}*/
		
		bool checkIfInside(int x, int y) const
		{
			if(pixels[x][y] == 1 && pixels[x][y-1] == 1 && pixels[x][y+1] == 1 && pixels[x-1][y] == 1 && pixels[x+1][y] == 1){ return true; }
			else { return false; }
		}
		
		//Por quest�es de otimiza��o, est� sendo usado o c�digo acima.
		/*bool checkIfInside(int xIndex, int yIndex, Mask mask) const
		{
			int maskSize = mask.getMaskSize();
			for(int x = 0; x < maskSize; x++)
			{
				for(int y = 0; y < maskSize ; y++)
				{
					if(mask.getMaskPixel(x,y) == 1 && getPixel((xIndex-1) + x ,(yIndex-1) + y) == 0){ return false; }
				}
			}
			return true;
		}*/
		
		int getAveragePixel(int x,int y) const
		{
			int outputPixel = 0;
			float averageCount = 0;
			
			for(int xIndex = x - 1; xIndex <= x + 1; xIndex++)
			{
				for(int yIndex = y; yIndex <= y + 1; yIndex++)
				{
					averageCount += getPixel(xIndex,yIndex);
				}
			}
			averageCount = averageCount/9.0;
			
			if(averageCount >= 0.2){ return 1; }
			else{ return 0; }
		}
		
		Image getDiff(Image img) const
		{
			{
				Image teste;
			}
			
			Image output;
			output.setImageSize(imageX, imageY);
			
			for(int x = 1; x < imageX; x++)
			{
				for(int y = 1; y < imageY; y++)
				{
					if(getPixel(x,y) == 1 && img.getPixel(x,y) == 1 ) { output.setPixel(x,y,0); }
					else { output.setPixel(x,y, getPixel(x,y)); }
				}
			}
			return output;
		}
		
		Image merge(Image img)
		{
			Image output;
			output.setImageSize(imageX, imageY);
			
			for(int x = 1; x < imageX - 1; x++)
			{
				for(int y = 1; y < imageY - 1; y++)
				{
					if(img.getPixel(x,y) == 1){ setPixel(x,y,1); }
				}
			}
		}
		
	public:
		//constructor
		Image()
		{
			pixels = new int*[4096];
			for(int i = 0; i < 4096; i++){ pixels[i] = new int[4096]; memset(pixels[i], 0, 4096);}
		}
		
		//copy constructor
		Image(const Image& i)
		: Image()
		{ 	
			*this = i;
		}
		
		//destructor
		~Image()
		{
			for(int i = 0; i < 4096; i++){ delete [] pixels[i]; }
			delete [] pixels;
		}
		
		Image& operator=(const Image& returned)
		{
			setImageSize(returned.imageX , returned.imageY);
			
			for(int x = 0; x < imageX; x++)
			{
				for(int y = 0; y < imageY; y++)
				{
					setPixel(x,y, returned.getPixel(x,y));
				}
			}
		}
		
		void setPixel(int x, int y , int value) { pixels[x][y] = value; }
		
		void setImageSize(int xSize, int ySize){ imageX = xSize; imageY = ySize; }
		
		int getPixel(int x, int y) const { return pixels[x][y]; }
		
		Image dilatateImage() const
		{
			Image output;
			output.setImageSize(imageX, imageY);
			
			Mask mask;
			mask.setMaskSize(3);
			mask.setCrossMask();
			
			for(int x = 1; x < imageX - 1; x++)
			{
				for(int y = 1; y < imageY - 1; y++)
				{	
					if(checkIfTouch(x,y)) { output.setPixel(x,y,1); }
					//Futura Organiza��o
					//if(checkIfTouch(x,y,mask)) { output.setPixel(x,y,1); }
					else{ output.setPixel(x,y,0); }
				}
			}
			
			return output;
		}
		
		Image erodeImage() const
		{
			Image output;
			output.setImageSize(imageX, imageY);
			
			Mask mask;
			mask.setMaskSize(3);
			mask.setCrossMask();
			
			float percentage = 0.0;
			
			for(int x = 1; x < imageX - 1; x++)
			{
				percentage += 100.0/imageX;
				for(int y = 1; y < imageY - 1; y++)
				{	
					if(checkIfInside(x,y)) { output.setPixel(x,y,1); }
					//Futura Organiza��o
					//if(checkIfInside(x,y,mask)) { output.setPixel(x,y,1); }
					else{ output.setPixel(x,y,0); }
				}
			}	
			return output;
		}
		
		Image getAverageImage() const
		{
			Image output;
			output.setImageSize(imageX, imageY);
			
			for(int x = 1; x < imageX - 1; x++)
			{
				for(int y = 1; y < imageY - 1; y++)
				{
					int average = getAveragePixel(x,y);
					output.setPixel(x,y,average);
				}
			}
			
			return output;
		}
		
		Image getInternalGradient() const
		{	
			//Futura Organiza��o
			//Mask gradientMask;
			//gradientMask.setMaskSize(3);
			//gradientMask.setFullMask();
			
			Image temp = erodeImage();
			Image output = getDiff(temp);
			//output.saveImage("Wtf is happening.pbm");
			
			return output;
		}
		
		Image getExternalGradient() const
		{
			//Futura organiza��o
			//Mask gradientMask;
			//gradientMask.setMaskSize(3);
			//gradientMask.setFullMask();
			
			Image temp = dilatateImage();
			Image output = temp.getDiff(temp.erodeImage());
			
			return output;
		}
		
		Image highlightWords()
		{
			Image output;
			output.setImageSize(imageX, imageY);
			
			bool lineFound = false;
			for(int x = 1; x < imageX - 1; x++)
			{
				for(int y = 1; y < imageY - 1; y++)
				{
					if(getPixel(x,y) == 1)
					{
						output.merge(foundWord(x,y));
					}
				}
			}
		}
		
		void saveImage(char* name)
		{
			{
				FILE* savingImg;
				
				savingImg = fopen( name , "w+");
				
				fprintf(savingImg , "P1\n#Feita por Willian Mota Oliveira, use GIMP para ver a foto.\n%d %d\n", imageX, imageY);
				
				int endlIndex = 0;
				for(int line = 0; line < imageY ; line++)
				{
					for(int column = 0; column < imageX; column++)
					{
						if(endlIndex == 69){ fprintf(savingImg, "%d\n", getPixel(column, line)); endlIndex = 0;}
						else{ fprintf(savingImg, "%d", getPixel(column, line)); endlIndex++;}
					}
					
				}
				
				fclose(savingImg);
				
				std::cout << "Saved " << name << " \n";
			}
		}
		
		void loadImage(char* name)
		{
			//Ler imagem em imagem.pbm
			FILE* file;
			file = fopen(name , "r");
			
			char header[5];
			fscanf(file, "%s", header);
			
			//Pular linha:
			char buffer[100];
			fgets(buffer, 100, file);
			char ignore[1024];
			fgets(ignore, sizeof(ignore), file);
			/////////////
		
			fscanf(file, "%d", &imageX);
			fscanf(file, "%d", &imageY);
			
			std::cout << "Resolucao: " << imageX << "x" << imageY << '\n';
			
			for(int line = 0; line < imageY ; line++)
			{
				for(int column = 0; column < imageX; column++)
				{
					char inputChar;
					fscanf(file, "%c", &inputChar);
					if(inputChar == '1'){ setPixel(column, line, 1); } else if(inputChar == '0'){ setPixel(column, line, 0); }
					else{ column--; }
				}
			}
			
			fclose(file);
		}
};

int main()
{
	char nome[50];
	scanf("%s", nome);
	
	Image input;
	input.loadImage(nome);
	
	Image filteredImage = input.erodeImage().dilatateImage().dilatateImage().erodeImage().dilatateImage();
	filteredImage.saveImage("Filtered.pbm");
	
	Image internalGradient = filteredImage.getInternalGradient();
	internalGradient.saveImage("InternalGradient.pbm");
	
	Image externalGradient = filteredImage.getExternalGradient();
	externalGradient.saveImage("ExternalGradient.pbm");
	
	return 0;
}


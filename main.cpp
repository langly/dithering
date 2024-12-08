/*
* Simple Program to play around with dithering. 
*
* Get the cimg files from https://cimg.eu
*
* My webpage here:
*  https://langly.org
*
*/
#define cimg_use_jpeg
#include "CImg.h"
#include <iostream>
#include <cassert>

using namespace cimg_library;
using namespace std;

#define R_WEIGHT 0.299
#define G_WEIGHT 0.587
#define B_WEIGHT 0.114

int main(int argc, char **argv){

	if ( argc < 2 ) {
		printf("Usage: ./main filename\n");
		return -1;
	}


	CImg<float> image(argv[1]);

	image.resize(800,480);
	CImg<float> grey(image._width, image._height,1,1);
	CImg<float> outp(image._width, image._height,1,1);
	CImg<float> bw5(image._width, image._height,1,1);
	CImg<float> bw2(image._width, image._height,1,1);
	CImg<float> bw7(image._width, image._height,1,1);

	cout << image._width << "x" << image._height << endl;

	size_t size = image._width*image._height;
	float *data	= new float[size];

	// Set the accumulated error in the dithered image to 0
	for ( int i = 0; i < size; i++){
		data[i] = 0.0;
	}

	for ( int y = 0; y < image._height-1; y++){
		for ( int x = 0; x < image._width-1; x++){
			unsigned pos = ((y * image._width) + x);
			float r,g,b;

			// The data is laid out in memory in the following way
			// { All R values, All G values, All B values }
			// instead of a more traditional layout of interleaving the R,G,B channels.
			r = image._data[size*0 + pos] / 255.0;
			g = image._data[size*1 + pos] / 255.0;
			b = image._data[size*2 + pos] / 255.0;

			// Convert to greyscale
			float old = r * R_WEIGHT +
				g * G_WEIGHT +
				b * B_WEIGHT;


			bw2._data[pos] = old > .25 ? 1.0 : 0;
			bw5._data[pos] = old > .50 ? 1.0 : 0;
			bw7._data[pos] = old > .75 ? 1.0 : 0;


			float val = old + data[pos] + 0.5 >= 1.0 ? 1.0 : 0.0;

            // FS -- Uncomment the floats you don't want to use.
            float divisor = 16.0;
            // Jarvis
            divisor = 48.0;
            // Atkinson
            divisor = 8.0;
            float error = (old - val) / divisor;

            // cout << ":" << old << "," << val << "," << error << endl;

			grey._data[pos] = old * 255.0;

			//
			data[pos] = val;

			outp._data[pos] = val*256.0;

			// Now bleed the error to the others.
            // Floyd Steinberg
			// data[ (((y+0) * image._width) + (x+1)) ] += (error * 7.0);
			// data[ (((y+1) * image._width) + (x+1)) ] += (error * 1.0);
			// data[ (((y+1) * image._width) + (x  )) ] += (error * 5.0);
			// data[ (((y+1) * image._width) + (x-1)) ] += (error * 3.0);

            // Jarvis, Judice and Ninke
			// data[ (((y+0) * image._width) + (x+1)) ] += (error * 7.0);
			// data[ (((y+0) * image._width) + (x+2)) ] += (error * 5.0);

			// data[ (((y+1) * image._width) + (x-2)) ] += (error * 3.0);
			// data[ (((y+1) * image._width) + (x-1)) ] += (error * 5.0);
			// data[ (((y+1) * image._width) + (x  )) ] += (error * 7.0);
			// data[ (((y+1) * image._width) + (x+1)) ] += (error * 5.0);
			// data[ (((y+1) * image._width) + (x+2)) ] += (error * 3.0);

			// data[ (((y+2) * image._width) + (x-2)) ] += (error * 1.0);
			// data[ (((y+2) * image._width) + (x-1)) ] += (error * 3.0);
			// data[ (((y+2) * image._width) + (x  )) ] += (error * 5.0);
			// data[ (((y+2) * image._width) + (x+1)) ] += (error * 3.0);
			// data[ (((y+2) * image._width) + (x+2)) ] += (error * 1.0);

            // Atkinson
			data[ (((y+0) * image._width) + (x+1)) ] += (error );
			data[ (((y+0) * image._width) + (x+2)) ] += (error );

			data[ (((y+1) * image._width) + (x-1)) ] += (error );
			data[ (((y+1) * image._width) + (x  )) ] += (error );
			data[ (((y+1) * image._width) + (x+1)) ] += (error );

			data[ (((y+2) * image._width) + (x  )) ] += (error );

		}
	}

    CImgDisplay main_display(image), grey_display(grey), 
		bw2_display(bw2),
		bw5_display(bw5),
		bw7_display(bw7),
		outp_display(outp);

    while(!main_display.is_closed()){
        main_display.wait();
    };

    return 0;
}

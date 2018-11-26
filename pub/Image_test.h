#pragma once
#include <stdio.h>

typedef enum ColorType
{
	COLOR_PACKED_YUV444 = 0,
	COLOR_YUV444 = 1,
	COLOR_YUV422,
	COLOR_UYVY,
	COLOR_YUYV,
	COLOR_YUV420,
	COLOR_NV12,
	COLOR_NV21,
	COLOR_YUV420_10LE,
	COLOR_YUV420_10BE,
	COLOR_RGB32,
	COLOR_RGB24,
	COLOR_RGB16
}COLORTYPE;

#define clip(var) ((var>=255)?255:(var<=0)?0:var)

void SaveBmp(const char *filename, unsigned char *rgbbuf, int width, int height);
void yuv2rgb(unsigned char* yuv, unsigned char* rgba, ColorType colorType, unsigned int pitch, unsigned int width, unsigned int height);

inline void SaveBmp(const char *filename, unsigned char *rgbbuf, int width, int height)
{
	int i, j, k;
	unsigned char* tmp;
	BITMAPFILEHEADER fheader;
	BITMAPINFOHEADER header;

	memset(&fheader, 0, sizeof(BITMAPFILEHEADER));
	fheader.bfType = 'B' | 'M' << 8;
	fheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	fheader.bfSize = fheader.bfOffBits + width*height * 3;

	memset(&header, 0, sizeof(BITMAPINFOHEADER));
	header.biSize = sizeof(BITMAPINFOHEADER);

	header.biWidth = width; 
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 24;
	header.biCompression = BI_RGB;

	FILE *file = fopen(filename, "wb");
	if (!file)
	{
		printf("Could not write file\n");
		return;
	}
	tmp = new unsigned char[width*height * 3];
	for (j = 0, k = height - 1; j < height; j++, k--) {
		for (i = 0; i < width; i++) {
			tmp[(j*width + i) * 3 + 2] = rgbbuf[(k*width + i) * 3];
			tmp[(j*width + i) * 3 + 1] = rgbbuf[(k*width + i) * 3 + 1];
			tmp[(j*width + i) * 3] = rgbbuf[(k*width + i) * 3 + 2];
		}
	}

	/*Write headers*/
	fwrite(&fheader, sizeof(BITMAPFILEHEADER), 1, file);
	fwrite(&header, sizeof(BITMAPINFOHEADER), 1, file);

	fwrite(tmp, width*height * 3, 1, file);
	fclose(file);
	delete tmp;
}

inline void yuv2rgb(unsigned char* yuv, unsigned char* rgba, ColorType colorType, unsigned int pitch, unsigned int width, unsigned int height)
{

	int j, i;
	int c, d, e;

	int stride_uv;

	int r, g, b;

	int t_width = width;

	unsigned char* line = rgba;
	unsigned char* cur;
	
	unsigned char* y = yuv;
	unsigned char* misc = yuv + pitch*height;

	short* rgb16;

	if (colorType == COLOR_PACKED_YUV444) {
// 		for (j = 0; j < height; j++) {
// 			cur = line;
// 			for (i = 0; i < width; i++) {
// 				c = misc[(j*width + i) * 3] - 16;
// 				d = misc[(j*width + i) * 3 + 1] - 128;
// 				e = misc[(j*width + i) * 3 + 2] - 128;
// 
// 				(*cur) = clip((298 * c + 409 * e + 128) >> 8); cur++;
// 				(*cur) = clip((298 * c - 100 * d - 208 * e + 128) >> 8); cur++;
// 				(*cur) = clip((298 * c + 516 * d + 128) >> 8); cur += 2;
// 			}
// 			line += t_width << 2;
// 		}
	}
	else if (colorType == COLOR_YUV444) {
// 		for (j = 0; j < height; j++) {
// 			cur = line;
// 			for (i = 0; i < width; i++) {
// 				c = y[j*width + i] - 16;
// 				d = u[j*width + i] - 128;
// 				e = v[j*width + i] - 128;
// 
// 				(*cur) = clip((298 * c + 409 * e + 128) >> 8); cur++;
// 				(*cur) = clip((298 * c - 100 * d - 208 * e + 128) >> 8); cur++;
// 				(*cur) = clip((298 * c + 516 * d + 128) >> 8); cur += 2;
// 			}
// 			line += t_width << 2;
// 		}
	}
	else if (colorType == COLOR_YUV422) {
// 		stride_uv = (width + 1) >> 1;
// 
// 		for (j = 0; j < height; j++) {
// 			cur = line;
// 			for (i = 0; i < width; i++) {
// 				c = y[j*width + i] - 16;
// 				d = u[j*stride_uv + (i >> 1)] - 128;
// 				e = v[j*stride_uv + (i >> 1)] - 128;
// 
// 				(*cur) = clip((298 * c + 409 * e + 128) >> 8); cur++;
// 				(*cur) = clip((298 * c - 100 * d - 208 * e + 128) >> 8); cur++;
// 				(*cur) = clip((298 * c + 516 * d + 128) >> 8); cur += 2;
// 			}
// 			line += t_width << 2;
// 		}
	}

	else if (colorType == COLOR_UYVY) {
		unsigned char* t = misc;
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < pitch; i += 2) {
				c = *(t + 1) - 16;    // Y1
				d = *(t + 0) - 128;   // U
				e = *(t + 2) - 128;   // V

				(*cur) = clip((298 * c + 409 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c - 100 * d - 208 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c + 516 * d + 128) >> 8); cur += 2;

				c = *(t + 3) - 16;    // Y2
				(*cur) = clip((298 * c + 409 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c - 100 * d - 208 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c + 516 * d + 128) >> 8); cur += 2;

				t += 4;
			}
			line += t_width << 2;
		}
	}

	else if (colorType == COLOR_YUYV) {
		unsigned char* t = misc;
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < pitch; i += 2) {
				c = *(t + 0) - 16;    // Y1
				d = *(t + 1) - 128;   // U
				e = *(t + 3) - 128;   // V

				(*cur) = clip((298 * c + 409 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c - 100 * d - 208 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c + 516 * d + 128) >> 8); cur += 2;

				c = *(t + 2) - 16;    // Y2
				(*cur) = clip((298 * c + 409 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c - 100 * d - 208 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c + 516 * d + 128) >> 8); cur += 2;

				t += 4;
			}
			line += t_width << 2;
		}
	}

	else if (colorType == COLOR_YUV420 || colorType == COLOR_NV12 || colorType == COLOR_NV21) {
		stride_uv = (pitch + 1) >> 1;

		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < t_width; i++) {
				c = y[j*pitch + i] - 16;

				if (colorType == COLOR_YUV420)
				{
// 					d = u[(j >> 1)*stride_uv + (i >> 1)] - 128;
// 					e = v[(j >> 1)*stride_uv + (i >> 1)] - 128;
				}
				else if (colorType == COLOR_NV12)
				{
					d = misc[(j >> 1)*pitch + (i >> 1 << 1)] - 128;
					e = misc[(j >> 1)*pitch + (i >> 1 << 1) + 1] - 128;
				}
				else // if (m_color == NV21)
				{
					d = misc[(j >> 1)*pitch + (i >> 1 << 1) + 1] - 128;
					e = misc[(j >> 1)*pitch + (i >> 1 << 1)] - 128;
				}

				(*cur) = clip((298 * c + 409 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c - 100 * d - 208 * e + 128) >> 8); cur++;
				(*cur) = clip((298 * c + 516 * d + 128) >> 8); cur++;
			}
			line += t_width * 3;
		}
	}

	else if (colorType == COLOR_YUV420_10LE || colorType == COLOR_YUV420_10BE) {
// 		for (j = 0; j < height; j++) {
// 			cur = line;
// 			for (i = 0; i < width; i++) {
// 
// 				if (colorType == COLOR_YUV420_10BE)
// 				{
// 					c = (y[j*width * 2 + i * 2] << 8) | y[j*width * 2 + i * 2 + 1];
// 					d = (u[(j >> 1)*width + (i >> 1 << 1)] << 8) | u[(j >> 1)*width + (i >> 1 << 1) + 1];
// 					e = (v[(j >> 1)*width + (i >> 1 << 1)] << 8) | v[(j >> 1)*width + (i >> 1 << 1) + 1];
// 				}
// 				else
// 				{
// 					c = (y[j*width * 2 + i * 2 + 1] << 8) | y[j*width * 2 + i * 2];
// 					d = (u[(j >> 1)*width + (i >> 1 << 1) + 1] << 8) | u[(j >> 1)*width + (i >> 1 << 1)];
// 					e = (v[(j >> 1)*width + (i >> 1 << 1) + 1] << 8) | v[(j >> 1)*width + (i >> 1 << 1)];
// 				}
// 
// 				c = c - (16 << 2);
// 				d = d - (128 << 2);
// 				e = e - (128 << 2);
// 
// 				(*cur) = clip((298 * c + 409 * e + (128 << 2)) >> 10); cur++;
// 				(*cur) = clip((298 * c - 100 * d - 208 * e + (128 << 2)) >> 10); cur++;
// 				(*cur) = clip((298 * c + 516 * d + (128 << 2)) >> 10); cur += 2;
// 			}
// 			line += t_width << 2;
// 		}
	}

	else if (colorType == COLOR_RGB32 || colorType == COLOR_RGB24 || colorType == COLOR_RGB16) {
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < pitch; i++) {
				if (colorType == COLOR_RGB32) {
					r = misc[(j*pitch + i) * 4];
					g = misc[(j*pitch + i) * 4 + 1];
					b = misc[(j*pitch + i) * 4 + 2];
				}
				else if (colorType == COLOR_RGB24) {
					r = misc[(j*pitch + i) * 3];
					g = misc[(j*pitch + i) * 3 + 1];
					b = misc[(j*pitch + i) * 3 + 2];
				}
				else {
					rgb16 = (short*)misc;

					r = ((rgb16[j*pitch + i] >> 11) & 0x1F) << 3;
					g = ((rgb16[j*pitch + i] >> 5) & 0x3F) << 2;
					b = ((rgb16[j*pitch + i]) & 0x1F) << 3;
				}

				(*cur) = r; cur++;
				(*cur) = g; cur++;
				(*cur) = b; cur += 2;
			}
			line += t_width << 2;
		}
	}

	else { // YYY
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < pitch; i++) {
				(*cur) = y[j*pitch + i]; cur++;
				(*cur) = y[j*pitch + i]; cur++;
				(*cur) = y[j*pitch + i]; cur += 2;
			}
			line += t_width << 2;
		}
	}
}
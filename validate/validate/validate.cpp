// validate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "capture.h"
#include <stdio.h>

#define CHECK_RETURN(ret) \
if (ret != 0) \
{ \
    printf("ERROR: failed call! ret = %d\n", ret); \
    goto _finish; \
}

struct RGBA
{
    char r;
    char g;
    char b;
    char a;
};

void saveAsBMP(BYTE *pData, INT rowPitch, INT height, const char *fileName)
{
    BITMAPINFOHEADER bmInfo = {};
    bmInfo.biSize = sizeof(BITMAPINFOHEADER);
    bmInfo.biWidth = rowPitch / 4;
    bmInfo.biHeight = -height;
    bmInfo.biPlanes = 1;
    bmInfo.biBitCount = 32;
    bmInfo.biCompression = BI_RGB;
    bmInfo.biSizeImage = 0;
    bmInfo.biXPelsPerMeter = 0;
    bmInfo.biYPelsPerMeter = 0;
    bmInfo.biClrUsed = 0;
    bmInfo.biClrImportant = 0;

    BITMAPFILEHEADER bmfHeader = {};
    DWORD dwSizeofImage = rowPitch * height;
    DWORD dwSizeofDIB = dwSizeofImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = dwSizeofDIB;
    bmfHeader.bfType = 0x4D42;

    FILE *f;
    fopen_s(&f, fileName, "wb");

    size_t dwBytesWritten = 0;
    dwBytesWritten += fwrite(&bmfHeader, sizeof(BITMAPFILEHEADER), 1, f);
    dwBytesWritten += fwrite(&bmInfo, sizeof(BITMAPINFOHEADER), 1, f);
    dwBytesWritten += fwrite(pData, 1, dwSizeofImage, f);

    fclose(f);
}

bool calcBin(char block[16][16])
{
    double sum = 0.0;
    double avg = 0.0;
    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            sum += block[y][x] ;
        }
    }
    avg = sum / 256;
    return (avg >128) ? true : false;
}

int parseFrameID(FrameProp* prop)
{
    char* pBuf = nullptr;
    RGBA rgb[16][256]; // 16 16x16 RGBA block
    char Y[16][256] = {};
    for (int y = 0; y < 16; y++)
    {
        pBuf = (char*)prop->pData + y * prop->pitch;
        for (int x = 0; x < 256; x++)
        {
            double val;
            char r, g, b;
            r = pBuf[x * 4];
            g = pBuf[x*4+1];
            b = pBuf[x*4+2];
            rgb[y][x].r = r;
            rgb[y][x].g = g;
            rgb[y][x].b = b;
            rgb[y][x].a = pBuf[x*4+3];
            val = (0.299*r + 0.587*g + 0.114*b);
            Y[y][x] = (val >= 255) ? 255 : ((val <=0)? 0: (BYTE)(val));
        }
    }

    int index = 0;
    bool bin = false;
    for (int n = 0; n < 16; n++)
    {
        char block[16][16];
        for (int y = 0; y < 16; y++)
        {
            pBuf = &Y[0][0] + 16 * n + 256 * y;
            for (int x = 0; x < 16; x++)
            {
                block[y][x] = *(pBuf + x);
            }
        }

        bin = calcBin(block);
        index = index | bin << n;
    }

    return index;
}

int main()
{
    int ret = 0;
    CaptureDesktop* pCapture = new CaptureDesktop;
    FrameProp prop = {};
    int id = 0;

    ret = pCapture->initialize();
    CHECK_RETURN(ret);

    ret = pCapture->getFrame(&prop);
    CHECK_RETURN(ret);

    saveAsBMP(prop.pData, prop.pitch, prop.height, "capture.bmp");

    id = parseFrameID(&prop);
    printf("frameID = %d\n", id);

_finish:
    pCapture->destroy();
    return ret;
}


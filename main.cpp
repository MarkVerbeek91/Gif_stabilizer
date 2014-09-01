#include <stdio.h>
#include <stdlib.h>

#include "bmp.hpp"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage:resize infile1 infile2 outfile\n");
        return 1;
    }

    // remember filenames and factor
    char* infile1 = argv[1];
    char* infile2 = argv[2];
    char* outfile = argv[3];

    // open input file
    FILE* inptr1 = fopen(infile1, "r");
    if (inptr1 == NULL)
    {
        printf("Could not open %s.\n", infile1);
        return 3;
    }

    // open input file
    FILE* inptr2 = fopen(infile2, "r");
    if (inptr2 == NULL)
    {
        fclose(inptr1);
        printf("Could not open %s.\n", infile2);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr1);
        fclose(inptr2);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf1;
    fread(&bf1, sizeof(BITMAPFILEHEADER)-2, 1, inptr1);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi1;
    fread(&bi1, sizeof(BITMAPINFOHEADER), 1, inptr1);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0 //|| bf.bfOffBits != 54
    if (bf1.bfType != 0x4d42 || bi1.biSize != 40 ||
            bi1.biBitCount != 24 || bi1.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr1);
        fclose(inptr2);
        fprintf(stderr, "Unsupported file format infile one.\n");
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf2;
    fread(&bf2, sizeof(BITMAPFILEHEADER)-2, 1, inptr2);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi2;
    fread(&bi2, sizeof(BITMAPINFOHEADER), 1, inptr2);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0 //|| bf.bfOffBits != 54
    if (bf2.bfType != 0x4d42 || bi2.biSize != 40 ||
            bi2.biBitCount != 24 || bi2.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr1);
        fclose(inptr2);
        fprintf(stderr, "Unsupported file format infile two.\n");
        return 4;
    }

    // construct bitmap headers for the outfile
    BITMAPFILEHEADER out_bf;
    BITMAPINFOHEADER out_bi;
    out_bf = bf1;
    out_bi = bi1;

    int white_border = 2;

    // update the header file.
    out_bi.biWidth = bi1.biWidth + 2 * white_border;
    out_bi.biHeight = bi1.biHeight + 2 * white_border;

    // determine padding for scanlines
    int in_padding =  (4 - (bi1.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int out_padding =  (4 - (out_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    out_bf.bfSize = 54 + out_bi.biWidth * abs(out_bi.biHeight) * 3 + abs(out_bi.biHeight) *  out_padding;
    out_bi.biSizeImage = ((((out_bi.biWidth * out_bi.biBitCount) + 31) & ~31) / 8) * abs(out_bi.biHeight);

    // write outfile's BITMAPFILEHEADER
    fwrite(&out_bf, sizeof(BITMAPFILEHEADER)-2, 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&out_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // writing to a 3D array
    RGBTRIPLE image1[bi1.biHeight][bi1.biWidth]; // orgineel
    RGBTRIPLE image2[bi2.biHeight][bi2.biWidth]; // te machten plaat
//    RGBTRIPLE image3[bi1.biHeight + 2 * white_border][bi1.biWidth + 2 * white_border]; // output file.

    for (int i = 0, biHeight = abs(bi1.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi1.biWidth; j++)
        {
            // read RGB triple from infile
            fread(&image1[i][j], sizeof(RGBTRIPLE), 1, inptr1);
            fread(&image2[i][j], sizeof(RGBTRIPLE), 1, inptr2);
        }

        // skip over padding in infile
        fseek(inptr1, in_padding, SEEK_CUR);
        fseek(inptr2, in_padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr1);
    fclose(inptr2);

    // now making the comparison between the input files
/*
    // location
    int x_loc, y_loc;

    int error_count = 0, min_error = -1;

    for ( int q = 0; q < 10; q++) // find location in height
    {
        for (int i = 0, biHeight = abs(bi1.biHeight); i < biHeight-q; i++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi1.biWidth; j++)
            {
                printf("\t%d: \t%d, ",image1[i][j].rgbtGreen,image2[i+q][j].rgbtGreen);

                if (image1[i][j].rgbtGreen != image2[i+q][j].rgbtGreen)
                    error_count++;

            }
            printf("\n");
        }
        printf("error between file: %d\n", error_count);

        if (error_count == 0)
        {
            x_loc = q;
            break;
        }

        error_count = 0;
    }
*/

    RGBTRIPLE white;
    white.rgbtGreen = 255;
    white.rgbtRed   = 255;
    white.rgbtBlue  = 255;

    // building the output file:
    for (int i = 0, biHeight = abs(out_bi.biHeight); i < biHeight; i++)
    {
        // make the first 50 koloms white
        if ( i < white_border || i > white_border + bi2.biHeight)
        {
            for (int j = 0; j < out_bi.biWidth; j++)
            {
                fwrite(&white, sizeof(RGBTRIPLE), 1, outptr);
                printf("\tG: 0, R: 0, B: 0");
            }
        }
        else
        {
            // write the white space left.
            for (int j = 0; j < white_border; j++)
            {
                fwrite(&white, sizeof(RGBTRIPLE), 1, outptr);
                printf("\tG: 0, R: 0, B: 0");
            }


            // write the image to outfile
            for (int j = 0; j < bi2.biWidth; j++)
            {
                fwrite(&image1[i][j], sizeof(RGBTRIPLE), 1, outptr);
                printf("\tG: %d, R: %d, B: %d",image1[i][j].rgbtGreen, image1[i][j].rgbtRed, image1[i][j].rgbtBlue);
            }

            // write the white space right.
            for (int j = 0; j < white_border; j++)
            {
                fwrite(&white, sizeof(RGBTRIPLE), 1, outptr);
                printf("\tG: 0, R: 0, B: 0");
            }

        }

        // then add it to outfile
        for (int k = 0; k <out_padding; k++)
            fputc(0x00, outptr);

        printf("\n");

    }





    // close outfile
    fclose(outptr);



    // that's all folks
    return 0;
}

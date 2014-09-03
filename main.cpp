#include <stdio.h>
#include <stdlib.h>

#include "bmp.hpp"


int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage:resize object_file frame_file frame_out_file\n");
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
    fread(&bf1, 14, 1, inptr1);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi1;
    fread(&bi1, sizeof(BITMAPINFOHEADER), 1, inptr1);

    printf("Bitmap file header:\n");
    printf("Type: \t\t%d \nSize: \t\t%d \nReserved1: \t%d \nReserved2: \t%d \nOffBits: \t%d\n", bf1.bfType, bf1.bfSize, bf1.bfReserved1, bf1.bfReserved2, bf1.bfOffBits);

    printf("Bitmap info header:\n");
    printf("Size: \t\t%d\nWidth: \t\t%d\nHeight: \t%d\nPlanes: \t%d\nBitCount: \t%d\nCompression: \t%d\nSizeImage: \t%d\nXPlesPerMeter: \t%d\nYPelsPerMeter: \t%d\nClrUSed: \t%d\nClrImportant \t%d\n",bi1.biSize,bi1.biWidth,bi1.biHeight,bi1.biPlanes,bi1.biBitCount,bi1.biCompression,bi1.biSizeImage,bi1.biXPelsPerMeter,bi1.biYPelsPerMeter,bi1.biClrUsed,bi1.biClrImportant);

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


    printf("Bitmap file header:\n");
    printf("Type: \t\t%d \nSize: \t\t%d \nReserved2: \t%d \nReserved2: \t%d \nOffBits: \t%d\n", bf2.bfType, bf2.bfSize, bf2.bfReserved2, bf2.bfReserved2, bf2.bfOffBits);

    printf("Bitmap info header:\n");
    printf("Size: \t\t%d\nWidth: \t\t%d\nHeight: \t%d\nPlanes: \t%d\nBitCount: \t%d\nCompression: \t%d\nSizeImage: \t%d\nXPlesPerMeter: \t%d\nYPelsPerMeter: \t%d\nClrUSed: \t%d\nClrImportant \t%d\n",bi2.biSize,bi2.biWidth,bi2.biHeight,bi2.biPlanes,bi2.biBitCount,bi2.biCompression,bi2.biSizeImage,bi2.biXPelsPerMeter,bi2.biYPelsPerMeter,bi2.biClrUsed,bi2.biClrImportant);


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

    // determine padding for scanlines
    int in_padding1 =  (4 - (bi1.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int in_padding2 =  (4 - (bi2.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // writing to a 3D array
    RGBTRIPLE image1[bi1.biHeight][bi1.biWidth]; // object image
    RGBTRIPLE image2[bi2.biHeight][bi2.biWidth]; // objective image
//    RGBTRIPLE image3[bi1.biHeight + 2 * white_border][bi1.biWidth + 2 * white_border]; // output file.

    for (int i = abs(bi1.biHeight)-1; i >= 0; i--)
    {
        // iterate over pixels in scanline
        for (int j = bi1.biWidth-1; j >= 0; j--)
        {
            // read RGB triple from infile
            fread(&image1[i][j], sizeof(RGBTRIPLE), 1, inptr1);

        }

        // skip over padding in infile
        fseek(inptr1, in_padding1, SEEK_CUR);
    }

    for (int i = abs(bi2.biHeight)-1; i >= 0; i--)
    {
        // iterate over pixels in scanline
        for (int j = bi2.biWidth-1; j >= 0; j--)
        {
            // read RGB triple from infile
            fread(&image2[i][j], sizeof(RGBTRIPLE), 1, inptr2);
        }

        // skip over padding in infile
        fseek(inptr1, in_padding2, SEEK_CUR);
    }

    // close infile
    fclose(inptr1);
    fclose(inptr2);

    printf("target file:\n");
    for (int i = 0; i < abs(bi1.biHeight); i++)
    {
        for (int j = 0; j < bi1.biWidth; j++)
        {
            if (image1[i][j].rgbtGreen == 28)
                printf("X");
            else
                printf(" ");
        }
        printf("\n");
    }

    printf("object file:\n");
    for (int i = 0; i < abs(bi2.biHeight); i++)
    {
        for (int j = 0; j < bi2.biWidth; j++)
        {
            if (image2[i][j].rgbtGreen == 28)
                printf("X");
            else
                printf(" ");
        }
        printf("\n");
    }


    /** The next section makes a comparison between the input files. Several
        overlaps of the first input file over the second input file are tried.
        First part checks of the first frame has overlap on the up and left
        corners. The second part checks the lower and right part.
    */

    int x_loc, y_loc;
    int error_count = 0, min_error = -1;

    int bi1Height = abs(bi1.biHeight);
    int bi1Width = bi1.biWidth;
    int max_x = abs(bi2.biHeight) - bi1Height;
    int max_y = bi2.biWidth - bi1Width;

    for ( int x = 0; x < max_x; x++)
    {
       // find the off sect in y-cords. because I know the input.
        for ( int y = 0; y < max_y; y++)
        {
            for (int i = 0; i < bi1Height; i++)
            {
                // iterate over pixels in scanline
                for (int j = 0; j < bi1Width; j++)
                {
                    if (image1[i][j].rgbtGreen != image2[x+i][y+j].rgbtGreen)
                        error_count++;

                //  printf("\t  %d::%d, ",image1[i][j].rgbtGreen,image2[i+q][j].rgbtGreen);

                }
         // printf("\n");
            }
            printf("error between file: %d\n", error_count);

            if (error_count == 0)
            {
                y_loc = y;
                x_loc = x;
                break;
            }

            error_count = 0;
        }
    }

    for ( int x = -9; x < 0; x++)
    {
       // find the off sect in y-cords. because I know the input.
        for ( int y = -9; y < 0; y++)
        {
            for (int i = y, biHeight = abs(bi1.biHeight); i < biHeight; i++)
            {
                // iterate over pixels in scanline
                for (int j = x; j < bi1.biWidth; j++)
                {
                    if (image1[i][j].rgbtGreen != image2[i][j].rgbtGreen)
                        error_count++;

                //  printf("\t  %d::%d, ",image1[i][j].rgbtGreen,image2[i+q][j].rgbtGreen);

                }
         // printf("\n");
            }
            printf("error between file: %d\n", error_count);

            if (error_count == 0)
            {
                y_loc = y;
                x_loc = x;
                break;
            }

            error_count = 0;
        }
    }

    printf("offset in y: %d", y_loc);

    /** This section build makes the header for the output file.

    */

    // construct bitmap headers for the outfile
    BITMAPFILEHEADER out_bf;
    BITMAPINFOHEADER out_bi;
    out_bf = bf1;
    out_bi = bi1;

    int white_border = 5;

    // update the header file.
    out_bi.biWidth = bi1.biWidth + 2 * white_border;
    out_bi.biHeight = bi1.biHeight + 2 * white_border;

    // determine padding for scanlines
    int out_padding =  (4 - (out_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    out_bf.bfSize = 54 + out_bi.biWidth * abs(out_bi.biHeight) * 3 + abs(out_bi.biHeight) *  out_padding;
    out_bi.biSizeImage = ((((out_bi.biWidth * out_bi.biBitCount) + 31) & ~31) / 8) * abs(out_bi.biHeight);

    // write outfile's BITMAPFILEHEADER
    fwrite(&out_bf, sizeof(BITMAPFILEHEADER)-2, 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&out_bi, sizeof(BITMAPINFOHEADER), 1, outptr);


    /** The next section writes input file 1 to output file. For this a white
        or black pixel is needed. After that the code loops through the rows
        of the output file and add according to the row number blank pixels or
        the image.
            Left and right of the image are also black pixels placed.

    */



    RGBTRIPLE white;
    white.rgbtGreen = 255;
    white.rgbtRed   = 255;
    white.rgbtBlue  = 255;

    RGBTRIPLE black;
    black.rgbtGreen = 0;
    black.rgbtRed   = 0;
    black.rgbtBlue  = 0;

    int input_col_num = 0;

    for (int i = 0, biHeight = abs(out_bi.biHeight); i < biHeight; i++)
    {
        // make the first coloms white
        if ( i < white_border + y_loc )
        {
            for (int j = 0; j < out_bi.biWidth; j++)
            {
                fwrite(&white, sizeof(RGBTRIPLE), 1, outptr);
            }
        }
        else if (i >= (white_border + bi2.biHeight + y_loc))
        {
            for (int j = 0; j < out_bi.biWidth; j++)
            {
                fwrite(&white, sizeof(RGBTRIPLE), 1, outptr);
            }
        }
        else
        {
            for (int j = 0; j < white_border; j++)
            {
                fwrite(&white, sizeof(RGBTRIPLE), 1, outptr);
            }

            // write the image to outfile
            for (int j = 0; j < bi2.biWidth; j++)
            {
                fwrite(&image1[input_col_num][j], sizeof(RGBTRIPLE), 1, outptr);
            }

            // write the white space right.
            for (int j = 0; j < white_border; j++)
            {
                fwrite(&white, sizeof(RGBTRIPLE), 1, outptr);
            }
            input_col_num++;

        }

        // then add it to outfile
        for (int k = 0; k <out_padding; k++)
            fputc(0x00, outptr);

    }
    // close outfile
    fclose(outptr);



    // that's all folks
    return 0;
}

#include <png.h>

void SaveBitmapAsPNG(unsigned char* buffer, const char* filename,int width,int height) {
    // Open the file for writing
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename);
        return;
    }

    // Initialize libpng structures
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fprintf(stderr, "Failed to create PNG write struct\n");
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        fprintf(stderr, "Failed to create PNG info struct\n");
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return;
    }

    // Set up error handling
    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Error during PNG creation\n");
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    // Set the file output
    png_init_io(png, fp); 
    // Set PNG metadata
    png_set_IHDR(png, info, width, height, 8 /* bit depth */, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Write header
    png_write_info(png, info);

    // Write the image row by row
    for (int y = 0; y < height; ++y) {
        png_write_row(png, buffer + (y * 1));
    }

    // Finish writing
    png_write_end(png, NULL);

    // Cleanup
    png_destroy_write_struct(&png, &info);
    fclose(fp);

    printf("Saved PNG file: %s\n", filename);
}

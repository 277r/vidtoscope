#include <stdio.h>
#include <string>
#include <png++/png.hpp>

// get difference grayscale, SCALED
png::rgb_pixel getdiffgss (png::rgb_pixel &a, png::rgb_pixel &c){
    int16_t r,g,b;
    r = a.red - c.red;
    g = a.green - c.green;
    b = a.blue - c.blue;

    r = abs(r);
    g = abs(g);
    b = abs(b);

    // three difference values, needs to result in one value.
    r = r+g+b;
    // expensive
    r /= 3;

    // dont scale

    // CLAMP
    if (r > 255){
        r = 255;
    }
    g = r;
    b = r;
    
    a.red = r;
    a.green = g;
    a.blue = b;
   

    return {r,g,b};

}

png::rgb_pixel comb4(png::rgb_pixel &d, png::rgb_pixel *s){
    d.red = 0;
    d.green = 0;
    d.blue = 0;

    for (int i = 0; i < 4; i++){
        d.red += s[i].red;
        d.green += s[i].green;
        d.blue += s[i].blue;
    }
}


int main(int argc, char *argv[])
{

    int height, width;
    int entrylist = 0;
    int *indexA = new int[207360000];
    int *indexB = new int[207360000];
    int *framemarkers = new int[207360000];
    // all are zero
    memset(framemarkers,0,207360000*sizeof(int));

    int k = std::stoi(argv[1]);
    char fname[20];
    for (int g = 1; g < k; g++){
        sprintf(fname, "%04d.png", g);
        printf(fname);
        png::image< png::rgb_pixel > image(fname);
        // image of same size
        png::image<png::rgb_pixel> buf(image.get_width(),image.get_height());

    // touching edges 
    png::rgb_pixel matrix[4];
    png::rgb_pixel diff[4];
        height = image.get_height();
        width = image.get_width();
    

    for (int i = 1; i < image.get_height() - 1; i++){
		for (int ii = 1; ii < image.get_width() - 1; ii++){

            // set difference valuesto original levels
            diff[0] = image[i][ii];
            diff[1] = image[i][ii];
            diff[2] = image[i][ii];
            diff[3] = image[i][ii];
 
            
            getdiffgss(diff[0],image[i-1][ii]);
            getdiffgss(diff[1],image[i+1][ii]);
            getdiffgss(diff[2],image[i][ii-1]);
            getdiffgss(diff[3],image[i][ii+1]);
            comb4(buf[i][ii],diff);
            if (buf[i][ii].red < 20){
                buf[i][ii].red = 0;
                buf[i][ii].green = 0;
                buf[i][ii].blue = 0;
                
            }   
            else {
                indexB[entrylist] = - i + (image.get_height() >> 1);
                indexA[entrylist] = ii - (image.get_width() >> 1);
                entrylist++;
            } 
            if (buf[i][ii].red > 120){

                buf[i][ii].red = 255;
                buf[i][ii].green = 255;
                buf[i][ii].blue = 255;

            }
        }
	}
    framemarkers[entrylist] = 1;
    }

    /*
    for (int i = 0; i < entrylist; i++){
        std::cout << framemarkers[i] << ", ";
        }
        */
   // calculate duration from frame entries.

    // %4d is limit anyway.
    // assume 30fps
    int *deltaTs = new int[10000];
    memset(deltaTs, 0, 10000*sizeof(int));
    int t0 = 0;
    int tinc = 0;
    int hits = 0;
    while (t0+tinc < entrylist){
        if (framemarkers[t0+tinc] == 1){
            deltaTs[hits] = tinc;
            hits++;
            t0 += tinc;
            tinc = 0;
        }
        tinc++;
    }
    // deltaTs now contains the relative data lengths.
    for (int i = 0; i < hits; i++){
        std::cout << deltaTs[i] << ", ";
        if (deltaTs[i] == 0){
            deltaTs[i]++;
        }    
    }



    uint32_t duration = hits / 30;
    uint32_t rate = 96000;  // Sample rate
    uint32_t frame_count = duration * rate;
    uint16_t chan_num = 2;  // Number of channels
    uint16_t bits = 16;  // Bit depth
    uint32_t length = frame_count*chan_num*bits / 8;
    int16_t byte;
    float sync_level = 1.0;
    // scales signal to use full 16bit resolution
    float multiplier = 32767 / sync_level;

    float *channel1 = (float *) malloc(frame_count * sizeof(float));
    float *channel2 = (float *) malloc(frame_count * sizeof(float));

    // amount of samples in a video frame
    int hitsize = rate / 30;
    int objT = 0;
    
    // for every hit, take the data inside and multiply by the time
    for (int i = 0; i < hits; i++){
        for (int j = 0; j < hitsize; j++){
            // channel1 data section of video frame time gets rotationally filled with data from indexA
            channel1[i*hitsize+j] =  indexA[objT+ (j % deltaTs[i])];   
            channel2[i*hitsize+j] =  indexB[objT+ (j % deltaTs[i])];   
            channel1[i*hitsize+j] /= height;
            channel2[i*hitsize+j] /= width;
        }
        objT += deltaTs[i];

    }

    // Writes data to wav file
    FILE *fp = fopen("video_signal.wav", "w");
    if (fp == NULL)
    {
        printf("Output file couldn't be opened!\n");
        return 4;
    }

    //// WAVE Header Data
    fwrite("RIFF", 1, 4, fp);
    uint32_t chunk_size = length + 44 - 8;
    fwrite(&chunk_size, 4, 1, fp);
    fwrite("WAVE", 1, 4, fp);
    fwrite("fmt ", 1, 4, fp);
    uint32_t subchunk1_size = 16;
    fwrite(&subchunk1_size, 4, 1, fp);
    uint16_t fmt_type = 1;  // 1 = PCM
    fwrite(&fmt_type, 2, 1, fp);
    fwrite(&chan_num, 2, 1, fp);
    fwrite(&rate, 4, 1, fp);
    // (Sample Rate * BitsPerSample * Channels) / 8
    uint32_t byte_rate = rate * bits * chan_num / 8;
    fwrite(&byte_rate, 4, 1, fp);
    uint16_t block_align = chan_num * bits / 8;
    fwrite(&block_align, 2, 1, fp);
    fwrite(&bits, 2, 1, fp);

    // Marks the start of the data
    fwrite("data", 1, 4, fp);
    fwrite(&length, 4, 1, fp);  // Data size
    for (uint32_t i = 0; i < frame_count; i++)
    {
        byte = (channel1[i] * multiplier);
        fwrite(&byte, 2, 1, fp);
        byte = (channel2[i] * multiplier);
        fwrite(&byte, 2, 1, fp);
    }

    fclose(fp);
    free(channel1);
    free(channel2);
    return 0;
}

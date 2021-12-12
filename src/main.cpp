#include <iostream>
#include <string>
#include "Display.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

int winWidth = 320;
int winHeight = 240;

struct AudioData
{
    Uint8* buf;
    Uint32 len;
};

float *floatStream;
int sLen;

void myAudioCallback(void *userData, Uint8 *streamIn, int streamInLen)
{
    AudioData* audio = (AudioData*)userData;

    // std::cout << streamLen << std::endl;
    int streamLen = streamInLen/2; // if len=10, it wants you to fill a buffer of ten 8-bit bytes, but “out” is an array of 16-bit ints

    if(audio->len==0)
    {
        return;
    }

	Uint32 length = (Uint32)streamInLen;

	if(length > audio->len){
		length = audio->len;
	} else {
		length = length;
	}

    // memcpy(streamIn, audio->buf, length);

    floatStream = new float[streamLen]();
    sLen = streamLen; // copy for draw

    //audio samples
	Sint16* samples = (Sint16*)audio->buf;
	float sampleIndex = 0;
	float factor = 1.0f/32768.0f;
	for(Uint32 i = 0; i < streamLen; i++)
	{
		floatStream[i] = (samples[(size_t)sampleIndex]) * factor;
		sampleIndex += 1;
        // std::cout << "float cpy " << floatStream[i] << std::endl;
	}


    //final copy to callback
    Sint16 *stream = (Sint16 *)streamIn;
    for (size_t i = 0; i < streamLen; i++)
    {
        float val = floatStream[i];

        if (val > 1.0f)
        {
            val = 1.0f;
        }
        else if (val < -1.0f)
        {
            val = -1.0f;
        }
    
        // std::cout << val << std::endl;

        stream[i] = (Sint16)(val * 32767); //out stream casted to 16bit integers
    }

    audio->buf = audio->buf + length;
    audio->len = audio->len - length;
    
}

int main()
{
    std::cout << "hello" << std::endl;

    SDL_Init(SDL_INIT_EVERYTHING);

    Display display;
    display.init("something", winWidth, winHeight, true);

    std::string image_path = "res/image.png";
    SDL_Surface *cat_surface = IMG_Load(image_path.c_str());
    SDL_Texture *cat_texture = SDL_CreateTextureFromSurface(display.renderer, cat_surface);
    SDL_Rect cat_dst = {0, 0, cat_surface->w, cat_surface->h};

	SDL_AudioSpec wavSpec;
	Uint8* wavBuf;
	Uint32 wavLength;

    //"res/AKWF_0014.wav"

	if(SDL_LoadWAV("res/song.wav", &wavSpec, &wavBuf, &wavLength) == NULL)
	{
		// TODO: Proper error handling
		std::cout << "Error: " << "filename" 
			<< " could not be loaded as an audio file" << std::endl;
		// throw fileName;
	}

    AudioData audio;
    audio.buf = wavBuf;
    audio.len = wavLength;

    // wavSpec.callback = myAudioCallback;
    // wavSpec.userdata = &audio;

    // std::cout << wavBuf << std::endl;
    // Sint16* samples = (Sint16*)wavBuf;
    
    // float factor = 1.0f/32768.0f;
    // for(Uint32 i = 0; i < 600; i+=2)
	// {
    //     float samp = samples[i] * factor;
    //     std::cout << i << " " << samp << std::endl;
    // }

    SDL_AudioSpec spec;
    // TODO: Don't hardcode these values!
    SDL_zero(spec);
    spec.freq = 44100;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = 512;
    spec.callback = myAudioCallback;
    spec.userdata = &audio;

    int count = SDL_GetNumAudioDevices(0);
    for (int i = 0; i < count; ++i)
    {
        printf("Audio device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));
    }

    int deviceId= SDL_OpenAudioDevice(SDL_GetAudioDeviceName(1, 0), 0, &spec, NULL, 0);
    
    if (deviceId == 0)
    {
        // TODO: Proper error handling
        std::cout << "error opening device" << std::endl;
        throw SDL_GetError();
    }

    // std::cout << "\nbefore!!!!! \n" << audio.buf << std::endl;
    // audio.buf = audio.buf + (Uint32)400;
    // std::cout << "\nafter!!!!!! \n" << audio.buf << std::endl;

    int len = 3;
    uint8_t *buffer = new uint8_t[len];
    buffer[0] = 5;
    buffer[1] = 2;
    buffer[2] = 9;
    buffer = buffer + (Uint32)1;

    for(int i = 0; i < len; i++)
    {
        std::cout << (int)buffer[i] << std::endl;
    }

    SDL_PauseAudioDevice(deviceId, 0);

    bool isRunning=true;
    while(isRunning)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        SDL_RenderClear(display.renderer);

        SDL_RenderCopy(display.renderer, cat_texture, NULL, &cat_dst); // &src is empty cuz it uses whole texture
    
        //sine wav in mono so 600 samps instead of 1200
        //the reft of the samples are garbage? not the other speaker
        for(Uint32 i = 0; i < sLen; i+=2)
        {
            float l = floatStream[i];
            float r = floatStream[i+1];
            int s = i/(winWidth/100);
            SDL_SetRenderDrawColor(display.renderer, 255, 0, 255, 255);
            SDL_RenderDrawPoint(display.renderer, s, (l*60)+50);
            
            SDL_SetRenderDrawColor(display.renderer, 0, 255, 0, 255);
            SDL_RenderDrawPoint(display.renderer, s, (r*60)+110);
        }
        
        SDL_SetRenderDrawColor(display.renderer, 0, 0, 0, 255);

        SDL_RenderPresent(display.renderer);

    }

    // SDL_PauseAudioDevice(deviceId, 1);

    SDL_FreeWAV(wavBuf);

    return 0;
}
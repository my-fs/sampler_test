#include <iostream>
#include <math.h>
#include "Display.h"
#include "RtMidi.h"

#include "midifile/Binasc.h"
#include "midifile/MidiEvent.h"
#include "midifile/MidiEventList.h"
#include "midifile/MidiMessage.h"
#include "midifile/Options.h"
#include "MidiFileWrapper.h"

int winWidth = 320;
int winHeight = 240;

struct AudioData
{
    Uint8 *buf;
    Uint32 len;
};

bool DSPisOn;

unsigned int bpm = 120;
unsigned int buffer_size = 512;
unsigned int sample_rate = 44100;

float phase = 0.0;
float frequency = 1.0;
float amp = 0.0;

float* floatStream;

unsigned int timePassedSamples;

MidiFileWrapper mfw;
unsigned int ticksPassed = 0;

float mspt;
float getSecondsPerTick(int bpm, int tpq)
{
    float ms = 60000 / (float)(bpm * tpq);
    return ms;
}

std::vector<smf::MidiEvent> getEventsBetweenTicks(uint start, uint end)
{
    uint index = 0;
    //for(int i = 0; i < )
    std::vector<smf::MidiEvent> events;
    return events;
};

float nextSampleSine()
{
    phase += (M_PI * 2.0 / sample_rate) * frequency;
    if (phase > M_PI * 2.0) phase -= M_PI * 2.0;
    return sinf(phase);
}

float midi2Freq(int n)
{
    float f = 1.0 * pow(2, ((float)n - 69) / 12.0);

    return f;
}

#include "SampleOsc.h"
#include "Synth.h"
PolySynth polysynth;
int previousTick=0;

void myAudioCallback(void *udata, Uint8 *stream, int len)
{
    PolySynth *ps = (PolySynth *)udata; // cast synth from void pointer
    Sint16 *str = (Sint16 *) stream;

    ps->generateSamples(floatStream,buffer_size);

    for (int i = 0; i < len / sizeof(Sint16) / 2; i++) // len/size(Sint16)/2 = buffer_size
    {
        //float smp = osc->getNextSample() * amp; //Change this function name to generate a different sound or create your own
        str[i * 2] = (Sint16)(floatStream[i*2] * 32767);
        str[i * 2 + 1] = (Sint16)(floatStream[i*2+1] *  32767);
    }

}

//void myAudioCallback(void *udata, Uint8 *stream, int len)
//{
//    SampleOsc *osc = (SampleOsc *)udata;
//    //PolySynth
//    //AudioData *audio = (AudioData *)udata;
//    //std::cout <<  len / sizeof(Sint16) / 2 << std::endl;
//    // final copy
//    unsigned int i;
//    Sint16 *str = (Sint16 *) stream;
//
//    osc->pitch = frequency;
//    osc->generateSamples(floatStream,buffer_size);
//
//    int buff_size = len / sizeof(Sint16) / 2;
//
//    for (i = 0; i < len / sizeof(Sint16) / 2; i++) // len/size(Sint16)/2 = buffer_size
//    {
//        //float smp = osc->getNextSample() * amp; //Change this function name to generate a different sound or create your own
//        str[i * 2] = (Sint16)(floatStream[i*2] * 32767);
//        str[i * 2 + 1] = (Sint16)(floatStream[i*2+1] *  32767);
//    }
//
//    timePassedSamples += len / sizeof(Sint16) / 2;
//    float ms_passed = ((float)timePassedSamples/sample_rate)*1000;
//    int ticks_passed = (int)(ms_passed/mspt);
//    //std::cout << "paased, seconds " << ms_passed/1000 << " ticks "<< ticks_passed << " previous " << ticks_passed - 4 << std::endl;
//
//}

int main()
{
    std::cout << "hello world!" << std::endl;

    SDL_Init(SDL_INIT_EVERYTHING);

    Display display;
    display.init("something", winWidth, winHeight, true);

    ////////////
    //
    SDL_AudioSpec wavSpec;
    Uint8 *wavBuf;
    Uint32 wavLength;

    //"res/AKWF_0014.wav"
    if (SDL_LoadWAV("res/test.wav", &wavSpec, &wavBuf, &wavLength) == NULL)
    {
        // TODO: Proper error handling
        std::cout << "Error: "
                  << "filename"
                  << " could not be loaded as an audio file" << std::endl;
        // throw fileName;
    }

    //SampleOsc oscillator;
    //oscillator.loadWAV("res/AKWF_aguitar_0036.wav");
    //std::cout << "end " << oscillator.length / sizeof(Sint16) << std::endl;

    floatStream = new float[buffer_size*2]();

    AudioData audio;
    audio.buf = wavBuf;
    audio.len = wavLength;

    SDL_AudioSpec spec;
    // TODO: Don't hardcode these values!
    SDL_zero(spec);
    spec.freq = sample_rate;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = buffer_size;
    spec.callback = myAudioCallback;
    spec.userdata = &polysynth;

    int count = SDL_GetNumAudioDevices(0);
    for (int i = 0; i < count; ++i)
    {
        printf("Audio device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));
    }

    int deviceId = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(1, 0), 0, &spec, NULL, 0);

    if (deviceId == 0)
    {
        // TODO: Proper error handling
        std::cout << "error opening device" << std::endl;
        throw SDL_GetError();
    }

    SDL_PauseAudioDevice(deviceId,1);

    std::cout << 512/(2*sizeof(Sint16)) << std::endl;
    //std::cout << sizeof(Sint16) << " " << sizeof(Uint8) << " " << sizeof(float) << std::endl;

    RtMidiIn *midiin;
    std::vector<unsigned char> message;

    midiin = new RtMidiIn();
    midiin->openPort(0);

    //MidiFileWrapper mfw;
    mfw.init("res/mytest.mid");
    mfw.printData();
    mspt = getSecondsPerTick(bpm, mfw.tpq);
    std::cout << "mspt" << mspt << std::endl;

    //smf::MidiEvent event = mfw.midifile[0][0];

    polysynth.init();
    bool isRunning = true;
    while (isRunning)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                isRunning = false;
            }
            switch (e.type)
            {
            case SDL_KEYDOWN:
                switch (e.key.keysym.scancode)
                {
                case SDL_SCANCODE_SPACE:
                    std::cout << "space" << std::endl;
                    SDL_PauseAudioDevice(deviceId,0);
                    break;
                }
            case SDL_KEYUP:
                switch (e.key.keysym.scancode)
                {
                case SDL_SCANCODE_SPACE:
                    break;
                }
            }
        }

        SDL_RenderClear(display.renderer);

        midiin->getMessage(&message);

        polysynth.sendMessage(message);
        if(polysynth.midiNotesOn.size()>0) {
            std::cout << "notes on: ";
            for(int i = 0; i < polysynth.midiNotesOn.size(); i++)
            {
                std::cout << (int)polysynth.midiNotesOn[i][1] << " ";
            }
            for(int i = 0; i < polysynth.currentVoiceSize; i++)
            {
                std::cout << "pitch " << polysynth.voices[i].pitch;
            }
            std::cout << std::endl;
            for(const auto& elem : polysynth.voiceNotePairs)
            {
                std::cout << "pairs " << elem.first << " " << elem.second;
            }
            std::cout << "\n";
        }


        // int nBytes = message.size();
        // if (nBytes > 0)
        // {
        //     for (int i = 0; i < nBytes; i++)
        //     {
        //         std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
        //     }
        //     std::cout << "freq " << midi2Freq((int)message[1]) << std::endl;
        //     // std::cout << "stamp = " << stamp << std::endl;
        //     if((int)message[0]==144) {
        //         amp = 0.8;
        //         oscillator.phaseReset=true;
        //         frequency = midi2Freq((int)message[1]);
        //     }
        //     if((int)message[0]==128) {
        //         amp = 0.0;
        //     }
        // }

        for(int i = 0; i < buffer_size; i++)
        {
            SDL_SetRenderDrawColor(display.renderer, 255, 0, 255, 255);
            SDL_RenderDrawPoint(display.renderer, i/2, (floatStream[i*2] * 50) + 50);
        }

        SDL_SetRenderDrawColor(display.renderer, 40, 50, 40, 255);

        SDL_RenderPresent(display.renderer);
    }

    return 0;
}

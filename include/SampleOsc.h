#pragma once

struct SampleOsc
{
    Uint8 *bufEnd;
    Uint8 *bufStart;
    Uint8 *bufPos;
    Uint32 length; // len (in bytes)
    float pitch=1.85;
    float *table;
    float phase=0;
    bool phaseReset = false;
    void loadWAV(const std::string &fileName)
    {
        // TODO: Handle streamFromFile
        SDL_AudioSpec wavSpec;
        Uint8 *wavStart;
        Uint32 wavLength;

        if (SDL_LoadWAV(fileName.c_str(), &wavSpec, &wavStart, &wavLength) == NULL)
        {
            // TODO: Proper error handling
            std::cout << "Error: " << fileName
                      << " could not be loaded as an audio file" << std::endl;
        }

        bufPos = wavStart;
        bufStart = wavStart;
        bufEnd = bufStart + wavLength; //byte offset
        length = wavLength;
        
        // load float array of samples (mono)
        table = new float[length/2]();
        Sint16* samples = (Sint16*)wavStart;
        float factor = 1.0f/32768.0f; // converting between S16bit->float, (1/highest signed 16 bit number)
        for(Uint32 i = 0; i < length/2; i++)
        {
            table[i] = samples[i] * factor;
        }
    }
    float getNextSample()
    {
        phase+=pitch;
        if((phase>(length/2)-1) || phaseReset)
        {
            phase = 0;
            phaseReset=false;
        }

        float left_step = table[(unsigned int)floor(phase)]; 
        float right_step = table[(unsigned int)ceil(phase)];

        float percentage = phase - floor(phase);
        float interpolatedSample = ((1.0 - percentage) * left_step) + (percentage * right_step);

        float samp = interpolatedSample * amp;
        
        return samp;
    }
    void generateSamples(float *stream, size_t len)
    {
        for(int i = 0; i < len; i++)
        {
            float samp = getNextSample();
            stream[i*2] = samp;
            stream[i*2+1] = samp;
        }
    }
};


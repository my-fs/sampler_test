#include <vector>
#include "SampleOsc.h"

struct voiceParam
{
    float volume;
    float frequency;
};

struct PolySynth
{
    unsigned int voiceCount = 8;
    unsigned int currentVoiceSize = 0;
    std::vector<SampleOsc> voices;
    std::vector<std::vector<unsigned char>> midiNotesOn; // list of midi notes that are currently on
    std::map<int, int> voiceNotePairs;  //voice index, note

    float* mixedStream;

    void init()
    {
        for(int i = 0; i < voiceCount; i++)
        {
            SampleOsc osc;
            osc.loadWAV("res/AKWF_birds_0003.wav");
            voices.push_back(osc);
        }
    }

    void sendMessage(std::vector<unsigned char> midiMessage)
    {
        int nBytes = midiMessage.size();

        if (nBytes > 0)
        {
            for (int i = 0; i < nBytes; i++)
            {
                std::cout << "Byte " << i << " = " << (int)midiMessage[i] << ", ";
            }
            std::cout << "freq " << midi2Freq((int)midiMessage[1]) << std::endl;
            // std::cout << "stamp = " << stamp << std::endl;

            if((int)midiMessage[0]==144) // note on
            {
                //amp = 0.8;
                //oscillator.phaseReset=true;
                //frequency = midi2Freq((int)midiMessage[1]);
                if(currentVoiceSize<voiceCount)
                {
                    midiNotesOn.push_back(midiMessage);
                    currentVoiceSize++;
                    voices[currentVoiceSize-1].phaseReset=true;
                    std::cout << "?reset??" << std::endl;
                    voices[currentVoiceSize-1].amplitude = 0.1;
                    voices[currentVoiceSize-1].note = (int)midiMessage[1];
                    voices[currentVoiceSize-1].pitch = midi2Freq((int)midiMessage[1]);
                    //voiceNotePairs.insert({currentVoiceSize,(int)midiMessage[1]});
                }
            }

            if((int)midiMessage[0]==128) // note off
            {
                //amp = 0.0;
                for(int i = 0; i < midiNotesOn.size(); i++)
                {
                    if((int)midiNotesOn[i][1]==(int)midiMessage[1])
                    {
                        std::cout << "erased! " << i << std::endl;
                        //voiceNotePairs.erase(currentVoiceSize);
                        midiNotesOn.erase(midiNotesOn.begin() + i); 
                        voices[currentVoiceSize-1].amplitude = 0.0;
                        currentVoiceSize--;
                    }
                }
            }
        }
    }

    void generateSamples(float *stream, int len)
    {
        mixedStream = new float[len*2]();
        for(int i = 0; i < midiNotesOn.size(); i++)
        {
            for(int j = 0; j < voices.size(); j++)
            {
                if((int)midiNotesOn[i][1]==voices[j].note)
                {
                   // voices[j].pitch = midi2Freq((int)midiNotesOn[i][1]);
                    voices[j].generateSamples(len);
                }
            }
        }
        for(int i = 0; i < currentVoiceSize; i++)
        {
            for(int j = 0; j < len; j++)
            {
               //std::cout << samp << std::endl
                mixedStream[j*2] += voices[i].stream[j*2];
                mixedStream[j*2+1] += voices[i].stream[j*2+1];
            }
        }
        for(int i = 0; i < len; i++)
        {
            stream[i*2] = mixedStream[i*2];
            stream[i*2+1] = mixedStream[i*2+1];
        }
    }

};

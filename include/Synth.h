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
    std::vector<SampleOsc> voices;
    std::vector<std::vector<unsigned char>> midiNotesOn; // list of midi notes that are currently on

    void init()
    {

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
                midiNotesOn.push_back(midiMessage);
            }
            if((int)midiMessage[0]==128) // note off 
            {
                //amp = 0.0;
                for(int i = 0; i < midiNotesOn.size(); i++)
                {
                    if((int)midiNotesOn[i][1]==(int)midiMessage[1])
                    {
                        //std::cout << "erased! " << i << std::endl;
                        midiNotesOn.erase(midiNotesOn.begin() + i);
                    }
                }
            }
        }
    }
};

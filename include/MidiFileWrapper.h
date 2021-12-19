#pragma once

#include "midifile/MidiFile.h"
#include "midifile/Options.h"
#include <string>

struct MidiFileWrapper
{
    smf::MidiFile midifile;
    smf::Options options;

    std::string filename;
    int trackCount;
    int tpq;

    // load midi wrapped midifile object
    void init(std::string p_filename)
    {

        filename = p_filename;

        // i feel like this whole exchange is silly
        // converting filename into fake "arguments"
        // turn string into const char array
        const char *file = filename.c_str();
        int argc = 1;                 // 2 would be ".play","midis/(filename)"
        char **argv = (char **)&file; // remove "const" into double pointer char

        options.process(argc, argv);

        // if (options.getArgCount() == 0) // dont check argcount
        //     midifile.read(std::cin);
        // else
        midifile.read(options.getArg(0));
        midifile.doTimeAnalysis();
        midifile.linkNotePairs();

        trackCount = midifile.getTrackCount();
        tpq = midifile.getTicksPerQuarterNote();

        std::cout << "TPQ: " << tpq << std::endl;
        std::cout << "TRACKS: " << trackCount << std::endl;
    }

    void printData()
    {
        std::cout << "count" << trackCount << std::endl;
        for (int track = 0; track < trackCount; track++)
        {
            // MidiTrack trackStruct;

            std::cout << "track: "<< track << std::endl;
            std::cout << "Tick\tSeconds\tDur\tMessage" << std::endl;

            for (int event = 0; event < midifile[track].size(); event++)
            {
                // std::cout << std::dec << midifile[track][event].tick;
                // std::cout << '\t' << std::dec << midifile[track][event].seconds;

                std::cout << std::dec << midifile[track][event].tick;
                std::cout << '\t' << std::dec << midifile[track][event].seconds;

                // Note note;
                // trackStruct.events.push_back(midifile[track][event]);

                std::cout << '\t';
                if (midifile[track][event].isNoteOn()){
                    std::cout << midifile[track][event].getDurationInSeconds();
                }
                std::cout << '\t';
                
                std::cout << (int)midifile[track][event][0] << " " << 
                (int)midifile[track][event][1] << " " << (int)midifile[track][event][2] << std::endl;

                // smf::MidiEventList mytrack = midifile[track];
                // trackStruct.eventlist = &midifile[track];

                // note.eventIndex = event;

                if (midifile[track][event].isTrackName())
                {   // check for track names
                    std::string content = midifile[track][event].getMetaContent();
                    if (!content.empty())
                    {
                        std::cout << "content: " << content << std::endl;
                        // trackStruct.name = content;
                    }
                }

                // note.start = midifile[track][event].tick;
//                std::cout << '\t';

                // if (midifile[track][event].isTempo())
                // {
                //     double mspqn = midifile[track][event].getTempoTPS(tpq);
                //     // std::cout << "mspqn: " << mspqn << std::endl;
                // }

                if (midifile[track][event].isLinked())
                {
                    if (midifile[track][event].isNoteOn())
                    {
                        //         note.duration = midifile[track][event].getTickDuration();
                        //         note.key = midifile[track][event][1];
                        //         note.startNote = &midifile[track][event];
                        //         note.endNote = midifile[track][event].getLinkedEvent();
                        //         // end note
                        //         //  std::cout << midifile[track][event].getLinkedEvent() << std::endl;
                        //std::cout << "is on" << std::endl;
                        //         trackStruct.notes.push_back(note);

                        //         // min max
                        //         trackStruct.minNote = std::min(trackStruct.minNote, note.key);
                        //         trackStruct.maxNote = std::max(trackStruct.maxNote, note.key);
                    }
                }
            }

            // trackVec.push_back(trackStruct);
        }
    }

};

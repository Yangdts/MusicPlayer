#pragma once
#include <JuceHeader.h>
#include "PlaylistComponent.h"


class DJAudioPlayer : public juce::AudioSource{
public:
    DJAudioPlayer(juce::AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(juce::URL audioURL);//Load audiofiles
    void setGain(double gain); //Set audio gain from 0 to 1.0
    void setSpeed(double ratio); //Set speed by ratio of 0 to 100
    void setPosition(double posInSecs); //Sets the position of audiofile 
    void setPositionRelative(double pos); //Set position of audiofile relative 

    void start(); //start playing audio
    void stop(); //stop playing audio

    double getPositionRelative();     //relative position of playhead

private:
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
};
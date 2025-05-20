#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "PlaylistComponent.h"

//==============================================================================
/*
*/
class DeckGUI  : public juce::Component,
                 public juce::Button::Listener,
                 public juce::Slider::Listener,
                 public juce::FileDragAndDropTarget,
                 public juce::Timer
{
public:
    DeckGUI(DJAudioPlayer* player,
            juce::AudioFormatManager & formatManagerToUse,
            juce::AudioThumbnailCache & cacheToUse);
    ~DeckGUI();

    void paint (juce::Graphics&) override;
    void resized() override;

    /** Button::Listener*/
    void buttonClicked(juce::Button*) override;

    void sliderValueChanged(juce::Slider* slider)override;

    bool isInterestedInFileDrag(const juce::StringArray &files)override;
    void filesDropped(const juce::StringArray &files, int x, int y)override;

    void timerCallback() override;
    void playlistToDeckGUI(); //function for playlist functions to deckGUI

private:

    juce::FileChooser fChooser{ "Select a file..." };
    juce::TextButton playButton{ "PLAY" };
    juce::TextButton stopButton{ "STOP" };
    juce::TextButton loadButton{ "LOAD" };

    juce::TextEditor trackTitle;
    juce::String atrackTitle;

    juce::Slider volSlider;
    juce::Slider speedSlider;
    juce::Slider posSlider;
    juce::Label volLabel;
    juce::Label speedLabel;
    juce::Label posLabel;

    DJAudioPlayer* player;
    
    WaveformDisplay waveformDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};

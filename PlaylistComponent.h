#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <ostream>

//==============================================================================
/*
*/
class PlaylistComponent  : public juce::Component,
                           public juce::TableListBoxModel,
                           public juce::Button::Listener,
                           public juce::TextEditor::Listener
{
public:
    PlaylistComponent();
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;
    void paintRowBackground(juce::Graphics &, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate) override;
    void buttonClicked(juce::Button* button)override;

    void textEditorTextChanged(juce::TextEditor& editor)override;
    void deleteTrack(); //Delete file from playlist
    void loadTrackToDeck1(); //load file from playlist to deck 1
    void loadTrackToDeck2(); //load file from playlist to deck 2
    juce::String trackToGUI; 
    std::string stdString;

private:

    juce::TableListBox tableComponent;

    juce::AudioFormatManager formatManager;
    juce::Array<juce::File> myFiles;
    std::vector<std::string> existingFiles;
    double duration;
    juce::String trackTitle;
    juce::Array<double> myTrackDuration;
    juce::Array<juce::String> trackTitles;
    juce::TextButton playlistLoadButton{ "Load into playlist library" };
    int duplicatedIndex = -1;
    juce::TextButton playlistDeleteButton;
    juce::TextButton playlistDeck1;
    juce::TextButton playlistDeck2;
    juce::TextEditor playlistSearch{ "Search for tracks..." };
    juce::String searchInput;

    double rowClicked;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};

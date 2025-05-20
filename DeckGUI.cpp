#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player, 
                 juce::AudioFormatManager& formatManagerToUse,
                 juce::AudioThumbnailCache& cacheToUse
                ): player(_player),
                   waveformDisplay(formatManagerToUse,cacheToUse)
{
    //Add and make visible all deckGUI buttons, labels and sliders
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(posLabel);
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(trackTitle);

    //Add listeners to buttons and sliders
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    //Set range to sliders
    volSlider.setRange(0.0, 1.0);
    speedSlider.setRange(0.0, 100.0);
    posSlider.setRange(0.0, 1.0);

    //Label texts for sliders.
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.attachToComponent(&volSlider, true);
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, true);
    posLabel.setText("Position", juce::dontSendNotification);
    posLabel.attachToComponent(&posSlider, true);

    //track title on top of GUI
    trackTitle.setReadOnly(true);
    trackTitle.setTextToShowWhenEmpty("No Track Loaded", juce::Colours::orange);

    //Timer for position relative/waveform display
    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    //g.drawText ("DeckGUI", getLocalBounds(),juce::Justification::centred, true);   // draw some placeholder text
}

void DeckGUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    auto sliderSpace = getWidth() / 9;
    double rowH = getHeight() / 9;
    trackTitle.setBounds(0, 0, getWidth(), rowH);
    playButton.setBounds(0, rowH, getWidth()/2, rowH * 2);
    stopButton.setBounds(getWidth()/2, rowH , getWidth() / 2, rowH * 2);
    volSlider.setBounds(sliderSpace, rowH * 3, getWidth(), rowH);
    speedSlider.setBounds(sliderSpace, rowH * 4, getWidth(), rowH);
    posSlider.setBounds(sliderSpace, rowH * 5, getWidth(), rowH);
    waveformDisplay.setBounds(0,rowH * 6,getWidth(),rowH*2);
    loadButton.setBounds(0, rowH * 8, getWidth(), rowH);

    trackTitle.setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    trackTitle.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
}

void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        std::cout << "Play button was clicked " << std::endl;
        player->start();

    }
    if (button == &stopButton)
    {
        std::cout << "Stop button was clicked " << std::endl;
        player->stop();
    }
    if (button == &loadButton)
    {
        juce::FileChooser chooser{ "Select a file..." };
        if (chooser.browseForFileToOpen())
        {
            player->loadURL(juce::URL{ chooser.getResult() });
            waveformDisplay.loadURL(juce::URL{ chooser.getResult() });
            trackTitle.setText(juce::URL{ chooser.getResult() }.getFileName(), juce::NotificationType::dontSendNotification);
        }
    }
}

void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }
    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag(const juce::StringArray &files)
{
    return true;
}
void DeckGUI::filesDropped(const juce::StringArray &files, int x, int y)
{
    if (files.size() == 1)
    {
        player->loadURL(juce::URL{ juce::File{files[0]} });
    }
}

void DeckGUI::timerCallback()
{
    //std::cout << "DeckGUI::timerCallback" << std::endl;
    waveformDisplay.setPositionRelative(player->getPositionRelative());
}

//Playlist to deckGUI
void DeckGUI::playlistToDeckGUI()
{
    std::ifstream textFile("Playlist.txt");
    std::string str;
    std::getline(textFile, str);
    std::string URL = "file:///" + str; //convert to file
    DBG(URL);
    juce::URL audioURL{ URL }; 
    player->loadURL(audioURL); //load to DJAudioPlayer
    waveformDisplay.loadURL(audioURL); //load to waveformDisplay
    DBG(audioURL.getFileName()); 

    atrackTitle = audioURL.getFileName(); //get filename to set tracktitle
    trackTitle.setText(atrackTitle, juce::NotificationType::dontSendNotification);
}
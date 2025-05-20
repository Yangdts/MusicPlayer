#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
    //Add and make visible both deckGUIs and playlist
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(playlistComponent);

    //register formats for audio file
    formatManager.registerBasicFormats();

    //timer for timercallback function below
    startTimer(500);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{

    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    deckGUI1.setBounds(0,0,getWidth() / 2, getHeight()/2);
    deckGUI2.setBounds(getWidth()/2, 0, getWidth() / 2, getHeight()/2);
    
    playlistComponent.setBounds(0, getHeight() / 2, getWidth(), getHeight() / 2);
}

//function to check if user has loaded audio file from playlist into a deckGUI player
void MainComponent::timerCallback()
{
    std::ifstream file("Playlist.txt");
    std::string str;
    std::vector<std::string> lines;
    while (std::getline(file, str))
    {
        lines.push_back(str);
    }

    if (std::stoi(lines[1]) == 1)//Deck 1
    {
        deckGUI1.playlistToDeckGUI();
        std::ofstream textFile("Playlist.txt");
        textFile << "" << std::endl << "0" << std::endl;
        textFile.close();
    }
    else if (std::stoi(lines[1]) == 2)//Deck 2
    {
        deckGUI2.playlistToDeckGUI();
        std::ofstream textFile("Playlist.txt");
        textFile << "" << std::endl << "0" << std::endl;
        textFile.close();
    }
}
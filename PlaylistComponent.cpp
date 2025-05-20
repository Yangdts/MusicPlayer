#include <JuceHeader.h>
#include <string>
#include "PlaylistComponent.h"
#include <time.h>

//==============================================================================
PlaylistComponent::PlaylistComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    //Column headers
    tableComponent.getHeader().addColumn("Track title", 1, 300);
    tableComponent.getHeader().addColumn("Duration", 2, 100);
    tableComponent.getHeader().addColumn("Load to", 3, 150);
    tableComponent.getHeader().addColumn("Load to", 4, 150);
    tableComponent.getHeader().addColumn("Delete", 5, 100);


    tableComponent.setModel(this);
    addAndMakeVisible(tableComponent);
    addAndMakeVisible(playlistLoadButton);
    addAndMakeVisible(playlistSearch);
    playlistLoadButton.addListener(this);
    playlistSearch.addListener(this);

    formatManager.registerBasicFormats();

    //create a textfile and add placeholder "0" which will be replaced later
    std::ofstream textFile("Playlist.txt");
    textFile << stdString << std::endl << "0" << std::endl;
    textFile.close();

    //create textfile to store audiofile urls so the playlist persists
    std::ifstream pHtextFile("PlaylistHistory.txt");
    std::string str;
    std::vector<std::string> lines;
    while (std::getline(pHtextFile, str))
    {
        lines.push_back(str);
    }

    for (int i = 0; i < lines.size(); ++i)
    {
        existingFiles.push_back(lines[i]);
    }

    // if there are existing files, for persisting playlist
    if (existingFiles.size() != 0)
    {
        for (int i = 0; i < existingFiles.size(); ++i) //add into myfiles
        {
            myFiles.add(juce::File{ existingFiles[i] });
        }

        for (int i = 0; i < myFiles.size(); ++i)
        {
            trackTitle = juce::URL::removeEscapeChars(juce::URL{ myFiles[i] }.getFileName()); 
            trackTitles.add(trackTitle); 
            juce::AudioFormatReader* reader = formatManager.createReaderFor(myFiles[i]); //create reader
            duration = reader->lengthInSamples / reader->sampleRate; 
            myTrackDuration.add(duration);
        }

        tableComponent.updateContent(); //update table component to dispaly filename and duration
    }

}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
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
    g.drawText ("PlaylistComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    double rowH = getHeight() / 7;
    playlistLoadButton.setBounds(0, 0, getWidth() / 2, rowH);
    playlistSearch.setBounds(getWidth() / 2, 0, getWidth() / 2, rowH);

    tableComponent.setBounds(0, rowH, getWidth(), getHeight());

    //For header columns
    tableComponent.getHeader().setColumnWidth(1, (getWidth() / 8) * 3);
    tableComponent.getHeader().setColumnWidth(2, getWidth() / 8);
    tableComponent.getHeader().setColumnWidth(3, (getWidth() / 8) * 1.5);
    tableComponent.getHeader().setColumnWidth(4, (getWidth() / 8) * 1.5);
    tableComponent.getHeader().setColumnWidth(5, getWidth() / 8);

    tableComponent.setColour(juce::ListBox::backgroundColourId, juce::Colours::cornflowerblue);

    //For searchBar
    playlistSearch.setTextToShowWhenEmpty("Search for tracks...", juce::Colours::orange);
    playlistSearch.setFont(20.0f);

}

int PlaylistComponent::getNumRows()
{
    return trackTitles.size();
}
void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::orange);
    }
    else
    {
        g.fillAll(juce::Colours::darkgrey);
    }
}
void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (columnId == 1) //track titles
    {
        g.drawText(trackTitles[rowNumber], 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
    if (columnId == 2) //Duration
    {
        std::time_t seconds(myTrackDuration[rowNumber]);
        tm* p = gmtime(&seconds);
        std::string trackLength;
        std::string hours;
        std::string mins;
        std::string secs;
        if (p->tm_hour < 10)
        {
            hours = "0" + std::to_string(p->tm_hour);
        }
        else 
        {
            hours = std::to_string(p->tm_hour);
        }
        if (p->tm_min < 10)
        {
            mins = "0" + std::to_string(p->tm_min);
        }
        else
        {
            mins = std::to_string(p->tm_min);
        }
        if (p->tm_sec < 10)
        {
            secs = "0" + std::to_string(p->tm_sec);
        }
        else
        {
            secs = std::to_string(p->tm_sec);
        }

        trackLength = hours + ":" + mins + ":" + secs;
        g.drawText(trackLength, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (columnId == 3)//load track to deck 1
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "Deck 1" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
            btn->onClick = [this] {loadTrackToDeck1(); }; //load track to deck 1 when button is clicked
        }
    }
    if (columnId == 4)//load track to deck 2
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "Deck 2" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
            btn->onClick = [this] {loadTrackToDeck2(); }; //load track to deck 1 when button is clicked
        }
    }
    if (columnId == 5)//Delete track button
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "Delete" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
            btn->onClick = [this] {deleteTrack(); }; //load track to deck 1 when button is clicked
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    if (button == &playlistLoadButton)
    {
        juce::FileChooser choosers{ "Select for file(s) to add to the playlist library...",juce::File(),"*.mp3;*wav;*.aiff" };
        if (choosers.browseForMultipleFilesToOpen())
        {
            myFiles = choosers.getResults();
            for (int i = 0; i < myFiles.size(); ++i)
            {
                if (existingFiles.size() != 0 || i == 0)
                {
                    for (int j = 0; j < existingFiles.size(); ++j)
                    {
                        if (myFiles[i].getFullPathName().toStdString() == existingFiles[j])
                        {
                            DBG("There are duplicates");

                            juce::AlertWindow alertDuplicate("Duplicate file added", "Duplicated file" + juce::URL::removeEscapeChars(juce::URL{ myFiles[i] }.getFileName()) + "has not been added", juce::MessageBoxIconType::InfoIcon);
                            alertDuplicate.addButton("Continue", 0);
                            alertDuplicate.setUsingNativeTitleBar(true);

                            int result = alertDuplicate.runModalLoop();

                            duplicatedIndex = i; //Storing duplicated index
                            break;
                        }
                    }
                    if (i != duplicatedIndex)
                    {
                        trackTitle = juce::URL::removeEscapeChars(juce::URL{ myFiles[i] }.getFileName());
                        trackTitles.add(trackTitle);
                        juce::AudioFormatReader* reader = formatManager.createReaderFor(myFiles[i]);
                        duration = reader->lengthInSamples / reader->sampleRate;
                        myTrackDuration.add(duration);
                        DBG(trackTitles[i]);
                        DBG(duration);
                        existingFiles.push_back(myFiles[i].getFullPathName().toStdString());
                    }
                }
            }
            std::ofstream pHtextFile{ "PlaylistHistory.txt" };

            for (int i = 0; i < existingFiles.size(); ++i)
            {
                pHtextFile << existingFiles[i] << std::endl;
            }
            pHtextFile.close();
            tableComponent.updateContent();
        }
    }

    rowClicked = button->getComponentID().getDoubleValue();// get row clicked 
}

void PlaylistComponent::textEditorTextChanged(juce::TextEditor& editor)
{
    searchInput = playlistSearch.getText();

    for (int i = 0; i < trackTitles.size(); ++i) 
    {
        if (trackTitles[i].containsWholeWord(searchInput) && searchInput != "")
        {
            tableComponent.selectRow(i, false, true);
        }
    }
}
void PlaylistComponent::deleteTrack()
{
    DBG(rowClicked);
    for (int i = 0; i < myFiles.size(); ++i)
    {
        if (rowClicked == i)
        {
            juce::AlertWindow confirmDelete("Delete File", "Are you sure you want to delete this file?", juce::MessageBoxIconType::QuestionIcon);
            confirmDelete.setUsingNativeTitleBar(true);
            confirmDelete.addButton("Delete", true);
            confirmDelete.addButton("Cancel", false);

            bool result = confirmDelete.runModalLoop();

            if (result == true)
            {
                DBG("deletefile");
                trackTitles.remove(i);
                existingFiles.erase(existingFiles.begin() + i);
            }
            else
            {
                confirmDelete.exitModalState(true);
            }
        }
    }
    std::ofstream pHtextFile("PlaylistHistory.txt");
    for (int i = 0; i < existingFiles.size(); ++i)
    {
        pHtextFile << existingFiles[i] << std::endl;
    }
    pHtextFile.close();
    tableComponent.updateContent();
}

void PlaylistComponent::loadTrackToDeck1()
{
    for (int i = 0; i < myFiles.size(); ++i)
    {
        juce::AudioFormatReader* reader = formatManager.createReaderFor(myFiles[i]);
        if (rowClicked == i)
        {
            DBG("loadtodeck1");
            DBG(myFiles[i].getFileName());
            DBG(reader->getFormatName());
            trackToGUI = myFiles[i].getFullPathName();
            stdString = trackToGUI.toStdString();
            std::replace(stdString.begin(), stdString.end(), '\\', '/');
            DBG(stdString);

            std::ofstream textFile("playlist.txt");
            textFile << stdString << std::endl << "1" << std::endl;
            textFile.close();
        }
    }
}

void PlaylistComponent::loadTrackToDeck2()
{
    for (int i = 0; i < myFiles.size(); ++i)
    {
        juce::AudioFormatReader* reader = formatManager.createReaderFor(myFiles[i]);
        if (rowClicked == i)
        {
            DBG("loadtodeck2");
            DBG(myFiles[i].getFileName());
            DBG(reader->getFormatName());
            trackToGUI = myFiles[i].getFullPathName();
            stdString = trackToGUI.toStdString();
            std::replace(stdString.begin(), stdString.end(), '\\', '/');
            DBG(stdString);

            std::ofstream textFile("playlist.txt");
            textFile << stdString << std::endl << "2" << std::endl;
            textFile.close();
        }
    }
}
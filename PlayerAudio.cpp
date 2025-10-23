#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() {
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    // Important: Clear the transport source before destroying readerSource
    transportSource.setSource(nullptr);
    readerSource.reset();
}
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);

	// if you've reached to the end of the song and looping is enabled, restart playback

    if (isLooping && transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds()) {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}
void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}
bool PlayerAudio::LoadFile(const juce::File& file) {

    // Stop and clear the transport source first
    transportSource.stop();
    transportSource.setSource(nullptr);
    // Reset the reader source (this will delete the reader)
    readerSource.reset();
    metadataText = "";
    metadata.clear();
    metadataText = "Name: " + juce::String(file.getFileName()) + "\nSize: " +juce::String(file.getSize() / 1024) +" KB\n";
    if (auto* reader = formatManager.createReaderFor(file))
    {
        // Create new reader source
        
        metadataText += juce::String::formatted("Sample Rate: %d HZ\nChannels: %d\nBits Depth: %d\nDuration: %.2f\n",
            reader->sampleRate,
            reader->numChannels,
            reader->bitsPerSample,
            reader->lengthInSamples / reader->sampleRate);

        metadata = reader->metadataValues;
        if (metadata.size() > 0) {
            juce::String title = metadata.getValue("Title", "").toRawUTF8();
            juce::String artist = metadata.getValue("Artist", "").toRawUTF8();
            juce::String album = metadata.getValue("Album", "").toRawUTF8();
            juce::String genre = metadata.getValue("Genre", "").toRawUTF8();
            juce::String year = metadata.getValue("Year", "").toRawUTF8();
            juce::String commet = metadata.getValue("Commet", "").toRawUTF8();
            if (title.isNotEmpty()) metadataText += "Title: " + title + "\n";
            if (artist.isNotEmpty()) metadataText += "Artist: " + artist + "\n";
            if (album.isNotEmpty()) metadataText += "Album: " + album + "\n";
            if (genre.isNotEmpty()) metadataText += "Genre: " + genre + "\n";
            if (year.isNotEmpty()) metadataText += "Year: " + year + "\n";
            if (commet.isNotEmpty()) metadataText += "Commet: " + commet + "\n";
        }

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        // Connect the new source
        transportSource.setSource(readerSource.get(),
            0,
            nullptr,
            reader->sampleRate);

        return true;
    }
    return false;
}



void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}
double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}
double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::setGainFromGUI(float gain)
{
    if (!isMuted)
    {
        previousVolume = gain;
    }
    setGain(gain);
}

void PlayerAudio::toggleMute()
{
    if (isMuted)
    {
        setGain(previousVolume);
        isMuted = false;
    }
    else
    {
        setGain(0.0f);
        isMuted = true;
    }
}
void PlayerAudio::togglePlayer() {
    if (transportSource.isPlaying()) {
        transportSource.stop();
        previousPosition = transportSource.getCurrentPosition();
        isPlaying = false;
    }
    else {
        transportSource.setPosition(previousPosition);
        transportSource.start();
        isPlaying = true;
    }
}
bool PlayerAudio::getPlayerState() {
    return isPlaying;
}
void PlayerAudio::setPreviousPosition(float pos) {
    previousPosition = pos;
}
void PlayerAudio::setPlayerState(bool state) {
    isPlaying = state;
}

void PlayerAudio::toggleLooping()
{
    isLooping = !isLooping;
}

bool PlayerAudio::getLoopState() const
{
    return isLooping;
}
juce::String PlayerAudio::getMeta() {
    return metadataText;
}


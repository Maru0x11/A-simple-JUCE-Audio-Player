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
bool PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);

    // if you've reached to the end of the song and looping is enabled, restart playback

    if (transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds()-1) {
        if (isLooping) {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
        else {
            setPosition(0.0f);
            previousPosition = 0.0f;
            if (transportSource.getLengthInSeconds() > 0) return true;
            else return false;
        }

    }
    return false;
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

    if (auto* reader = formatManager.createReaderFor(file))
    {
        // Create new reader source
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
    if (readerSource != nullptr) {
        transportSource.setPosition(pos);
    }
}
double PlayerAudio::getPosition() const
{   
    if (readerSource != nullptr) {
		return transportSource.getCurrentPosition();
    }
    return 0.0;
}
double PlayerAudio::getLength() const
{
   if (readerSource != nullptr) {
       return transportSource.getLengthInSeconds();
   }
   return 0.0;
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
    if (isPlaying) {
        isPlaying = false;
        transportSource.stop();
        previousPosition = transportSource.getCurrentPosition();
        transportSource.setPosition(0.0);
    }
    else {
        isPlaying = true;
        transportSource.setPosition(previousPosition);
        transportSource.start();
    }
}
bool PlayerAudio::getPlayerState() {
    return isPlaying;
}
void PlayerAudio::setPlayerState(bool state) {
    isPlaying = state;
}
void PlayerAudio::setPreviousPosition(float pos) {
    previousPosition = pos;
}
void PlayerAudio::toggleLooping()
{
    isLooping = !isLooping;
}

bool PlayerAudio::getLoopState() const
{
    return isLooping;
}
void PlayerAudio::addToPlaylist(const juce::File& file) {
    filesNames.add(juce::String(file.getFileName()));
    playlistfiles.add(file);
}
juce::StringArray PlayerAudio::getFilesNames() {
    return filesNames;
}
juce::File PlayerAudio::getFile(int rowNumber) {
    return playlistfiles[rowNumber];
}
void PlayerAudio::deleteTrack(int rowNumber) {
    filesNames.remove(rowNumber);
    playlistfiles.remove(rowNumber);
}
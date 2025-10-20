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
void PlayerAudio::play()
{
    transportSource.start();
}
void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
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

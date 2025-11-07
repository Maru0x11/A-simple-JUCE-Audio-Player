#include "PlayerAudio.h"
//****************************************
#include <taglib/tag.h>
#include <taglib/fileref.h>
//****************************************
PlayerAudio::PlayerAudio() : resamplingSource(&transportSource, false)
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
    readerSource.reset();
}
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resamplingSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
bool PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    resamplingSource.getNextAudioBlock(bufferToFill);

    if (transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds() - 1) {
        if (isLooping) {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
        else {
            if (transportSource.getLengthInSeconds() > 0) return true;
            else return false;
        }

    }
    return false;
}
void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    resamplingSource.releaseResources();
}
bool PlayerAudio::LoadFile(const juce::File& file) {

    transportSource.stop();
    transportSource.setSource(nullptr);

    readerSource.reset();

    if (auto* reader = formatManager.createReaderFor(file))
    {
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

        transportSource.setSource(readerSource.get(),
            0,
            nullptr,
            reader->sampleRate);

        setPlaybackSpeed(1.0f);
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
    }
    else {
        isPlaying = true;
        if (transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds() - 1) {
            transportSource.setPosition(0.0f);
        }
        else {
            transportSource.setPosition(transportSource.getCurrentPosition());
        }
        transportSource.start();
    }
}
bool PlayerAudio::getPlayerState() {
    return isPlaying;
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
void PlayerAudio::addToPlaylist(const juce::File& file) {
    if (!playlistfiles.contains(file)) {
        filesNames.add(juce::String(file.getFileName()));
        playlistfiles.add(file);
    }
}
juce::StringArray PlayerAudio::getFilesNames() {
    return filesNames;
}
juce::File PlayerAudio::getFile(int rowNumber) {
    currentRowNumber = rowNumber;
    return playlistfiles[rowNumber];
}
void PlayerAudio::deleteTrack(int rowNumber) {
    filesNames.remove(rowNumber);
    playlistfiles.remove(rowNumber);
}
int PlayerAudio::getCurrentRowNumber() {
    return currentRowNumber;
}
int PlayerAudio::getPlaylistSize() {
    return filesNames.size();
}
void PlayerAudio::setPlaybackSpeed(float speed)
{
    currentSpeed = speed;
    if (readerSource != nullptr) {
        resamplingSource.setResamplingRatio(speed);
    }
}

float PlayerAudio::getPlaybackSpeed() const
{
    return currentSpeed;
}

juce::AudioFormatManager* PlayerAudio::getFormatManager()
{
    return &formatManager;
}
//***********************************************************************
void PlayerAudio::readMetadata(const juce::File& file) {
    metadata = "";

    TagLib::FileName path = file.getFullPathName().toRawUTF8();
    TagLib::FileRef fileRef(path);
    if (!fileRef.isNull() && fileRef.tag()) {
        juce::String title = fileRef.tag()->title().toCString();
        juce::String artist = fileRef.tag()->artist().toCString();
        juce::String album = fileRef.tag()->album().toCString();
        juce::String genre = fileRef.tag()->genre().toCString();
        juce::String year = juce::String(fileRef.tag()->year());
        juce::String commet = fileRef.tag()->comment().toCString();
        juce::String track = juce::String(fileRef.tag()->track());
        if (title.isNotEmpty()) metadata += "Title: " + title + "\n";
        if (artist.isNotEmpty()) metadata += "Artist: " + artist + "\n";
        if (album.isNotEmpty()) metadata += "Album: " + album + "\n";
        if (genre.isNotEmpty()) metadata += "Genre: " + genre + "\n";
        if (year.isNotEmpty()) metadata += "Year: " + year + "\n";
        if (commet.isNotEmpty()) metadata += "Commet: " + commet + "\n";
        if (track.isNotEmpty()) metadata += "Track: " + track + "\n";
    }
    else {
        metadata += "Name: " + juce::String(file.getFileName());
    }
}
juce::String PlayerAudio::getMetadata() {
    return metadata;
}
//***********************************************************************

#pragma once
#include <JuceHeader.h>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    bool getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool LoadFile(const juce::File& file);
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void toggleMute();
    void setGainFromGUI(float gain);
    void togglePlayer();
    bool getPlayerState();
    void setPlayerState(bool state);
    void toggleLooping();
    bool getLoopState() const;
    void addToPlaylist(const juce::File& file);
    juce::StringArray getFilesNames();
    juce::File getFile(int rowNumber);
    void deleteTrack(int rowNumber);
    int getCurrentRowNumber();
    int getPlaylistSize();
    //***********************************
    void readMetadata(const juce::File& file);
    juce::String getMetadata();
    //***********************************

    void setPlaybackSpeed(float speed);
    float getPlaybackSpeed() const;
    juce::AudioFormatManager* getFormatManager();

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    juce::ResamplingAudioSource resamplingSource;
    float currentSpeed = 1.0f;
    float previousVolume = 0.5f;
    bool isMuted = false;
    bool isPlaying = false;
    bool isLooping = false;
    juce::Array<juce::String> filesNames;
    juce::Array<juce::File> playlistfiles;
    //*************************************
    juce::String metadata;
    //*************************************
    int currentRowNumber=-1;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};

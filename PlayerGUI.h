#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::TableListBoxModel,
    public juce::Timer
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    juce::ShapeButton createShapeButton(const juce::String& name);
    juce::Path CreateButtonShape(const juce::String& name);
    void changePlayer();
    //======================================
    int getNumRows()override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)override;
    void paintCell(juce::Graphics& g, int rowNumber, int columId, int width, int height, bool rowIsSelected)override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentTpUpdate) override;
    void LoadFile();
private:
    PlayerAudio playerAudio;
    // GUI elements
    juce::TextButton loadButton{ "Load" };
    juce::Slider volumeslider;
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop" };
    juce::ShapeButton playPauseButton, goToStartButton, goToEndButton;
    //==================================
    juce::TableListBox PlaylistBox;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::TextButton addToPlaylist{ "Add" };
    //==================================
    juce::Slider positionSlider;
    juce::Label positionLabel;
    juce::Label durationLabel;

    // Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};


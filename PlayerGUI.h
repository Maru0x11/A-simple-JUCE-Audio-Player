#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
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

private:
    PlayerAudio playerAudio;
    // GUI elements
    juce::TextButton loadButton{ "Load" };
    juce::Slider volumeslider;
    juce::TextButton muteButton{ "Mute" };
	juce::TextButton loopButton{ "Loop" };
    juce::ShapeButton playPauseButton, goToStartButton, goToEndButton;
    juce::TextEditor metadataTextEditor;
    juce::String metadataText;
    juce::Viewport metadataViewPort;
    std::unique_ptr<juce::FileChooser> fileChooser;
    // Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};


#include "PlayerGUI.h"
PlayerGUI::PlayerGUI() {
	for (auto* btn : { &loadButton, &playButton, &stopButton, &muteButton }) 
	{
		addAndMakeVisible(btn);
		btn->addListener(this);
	}

	volumeslider.setRange(0.0, 1.0, 0.01);
	volumeslider.setValue(0.5);
	volumeslider.addListener(this);
	addAndMakeVisible(volumeslider);
}

PlayerGUI::~PlayerGUI() {}
void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	playerAudio.getNextAudioBlock(bufferToFill);
}
void PlayerGUI::releaseResources()
{
	playerAudio.releaseResources();
}
void PlayerGUI::resized() {
	int y = 10;
	loadButton.setBounds(18, y, 80, 30);
	playButton.setBounds(100, y, 80, 30);
	stopButton.setBounds(198, y, 88, 30);
    muteButton.setBounds(296, y, 80, 30); 
	volumeslider.setBounds(10, 60, getWidth() - 20, 30);
}
void PlayerGUI::buttonClicked(juce::Button* button)
{
	if (button == &loadButton)
	{
		fileChooser = std::make_unique<juce::FileChooser>(
			"Select an audio file...", juce::File{}, "*.wav;*.mp3");

		fileChooser->launchAsync(
			juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
			[this](const juce::FileChooser& fc)
		{
			auto file = fc.getResult();
			if (file.existsAsFile())
				playerAudio.LoadFile(file);
		});
	}
	else if (button == &playButton)
	{
		playerAudio.play();
	}
	else if (button == &stopButton)
	{
		playerAudio.stop();
	}
    else if (button == &muteButton)
    {
        playerAudio.toggleMute();
    }
}
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
	if (slider == &volumeslider)
		playerAudio.setGainFromGUI((float)volumeslider.getValue()); 
}

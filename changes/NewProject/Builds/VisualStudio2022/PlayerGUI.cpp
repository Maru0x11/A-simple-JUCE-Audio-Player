#include "PlayerGUI.h"

PlayerGUI::PlayerGUI() :
	playPauseButton(createShapeButton("playPauseButton")),
	goToStartButton(createShapeButton("goToStartButton")),
	goToEndButton(createShapeButton("goToEndButton"))
{
	std::array<juce::Button*, 6> buttons = {
		&loadButton, &muteButton,
		&playPauseButton, &goToStartButton, &goToEndButton,
		&loopButton
	};

	for (auto* btn : buttons)
	{
		addAndMakeVisible(btn);
		btn->addListener(this);
	}

	volumeslider.setRange(0.0, 1.0, 0.01);
	volumeslider.setValue(0.5);
	volumeslider.addListener(this);
	addAndMakeVisible(volumeslider);

	playPauseButton.setShape(CreateButtonShape("play"), true, true, true);
	goToStartButton.setShape(CreateButtonShape("goToStart"), true, true, true);
	goToEndButton.setShape(CreateButtonShape("goToEnd"), true, true, true);

	loopButton.setButtonText("Loop: OFF");

	playPauseButton.addListener(this);
	goToStartButton.addListener(this);
	goToEndButton.addListener(this);

}

PlayerGUI::~PlayerGUI() {}
void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (playerAudio.getNextAudioBlock(bufferToFill)) {
		juce::MessageManager::callSync([this]() {
			buttonClicked(&playPauseButton);
			});
	}
}
void PlayerGUI::releaseResources()
{
	playerAudio.releaseResources();
}
void PlayerGUI::resized() {
	int y = 10;
	loadButton.setBounds(y, y, 80, 30);
	muteButton.setBounds(y, 50, 80, 30);
	volumeslider.setBounds(100, 60, getWidth() - 20, 30);
	loopButton.setBounds(y, 90, 80, 30);

	playPauseButton.setBounds(getWidth() / 2 - 15, y, 30, 30);
	goToStartButton.setBounds(getWidth() / 2 - 55, y + 5, 20, 20);
	goToEndButton.setBounds(getWidth() / 2 + 35, y + 5, 20, 20);

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
				{
					playerAudio.LoadFile(file);
				}
			});
	}
	else if (button == &muteButton)
	{
		playerAudio.toggleMute();
	}
	else if (button == &playPauseButton) {
		playerAudio.togglePlayer();
		changePlayer();
	}
	else if (button == &goToStartButton) {
		playerAudio.setPosition(0.0f);
		playerAudio.setPreviousPosition(0.0f);
	}
	else if (button == &goToEndButton) {
		playerAudio.setPosition(playerAudio.getLength());
		playerAudio.setPreviousPosition(0.0f);
		playPauseButton.setShape(CreateButtonShape("play"), true, true, true);
		playPauseButton.setBounds(getWidth() / 2 - 15, 10, 30, 30);
		playerAudio.setPlayerState(false);
	}
	else if (button == &loopButton) {

		playerAudio.toggleLooping();

		if (playerAudio.getLoopState()) {
			loopButton.setButtonText("Loop: ON");
		}
		else {
			loopButton.setButtonText("Loop: OFF");
		}

	}
}
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
	if (slider == &volumeslider)
		playerAudio.setGainFromGUI((float)volumeslider.getValue());
}
juce::ShapeButton PlayerGUI::createShapeButton(const juce::String& name) {
	return juce::ShapeButton(
		name, juce::Colours::seashell,
		juce::Colours::black,
		juce::Colours::bisque);
}
juce::Path PlayerGUI::CreateButtonShape(const juce::String& name) {
	juce::Path buttonShape;
	if (name == "pause") {
		buttonShape.addRectangle(0.2f, 0.2f, 0.15f, 0.6f);
		buttonShape.addRectangle(0.6f, 0.2f, 0.15f, 0.6f);
	}
	else if (name == "play") {
		buttonShape.addTriangle(0.2f, 0.2f, 0.2f, 0.8f, 0.8f, 0.5f);
	}
	else if (name == "goToStart") {
		buttonShape.addRectangle(0.2f, 0.2f, 0.15f, 0.6f);
		buttonShape.addTriangle(0.5f, 0.5f, 0.8f, 0.8f, 0.8f, 0.2f);
	}
	else if (name == "goToEnd") {
		buttonShape.addTriangle(0.2f, 0.2f, 0.5f, 0.5f, 0.2f, 0.8f);
		buttonShape.addRectangle(0.65f, 0.2f, 0.15f, 0.6f);
	}
	return buttonShape;
}
void PlayerGUI::changePlayer() {
	if (playerAudio.getPlayerState()) {
		playPauseButton.setShape(CreateButtonShape("pause"), true, true, true);
		playPauseButton.setBounds(getWidth() / 2 - 15, 10, 30, 30);

	}
	else {
		playPauseButton.setShape(CreateButtonShape("play"), true, true, true);
		playPauseButton.setBounds(getWidth() / 2 - 15, 10, 30, 30);
	}
}
#include "PlayerGUI.h"

PlayerGUI::PlayerGUI() :
	playPauseButton(createShapeButton("playPauseButton")),
	goToStartButton(createShapeButton("goToStartButton")),
	goToEndButton(createShapeButton("goToEndButton")),
	thumbnail(512, *playerAudio.getFormatManager(), thumbnailCache)
{
	customLookAndFeel.setColour(juce::Label::textColourId, juce::Colours::deepskyblue);
	customLookAndFeel.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
	customLookAndFeel.setColour(juce::Slider::thumbColourId, juce::Colours::blueviolet);
	customLookAndFeel.setColour(juce::Slider::trackColourId, juce::Colours::blueviolet);
	customLookAndFeel.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::deepskyblue);
	customLookAndFeel.setColour(juce::TableListBox::textColourId, juce::Colours::darkolivegreen);
	customLookAndFeel.setColour(juce::TableListBox::backgroundColourId, juce::Colours::black);
	customLookAndFeel.setColour(juce::TableHeaderComponent::textColourId, juce::Colours::darkviolet);
	customLookAndFeel.setColour(juce::TableHeaderComponent::backgroundColourId, juce::Colours::transparentBlack);
	customLookAndFeel.setColour(juce::TableHeaderComponent::outlineColourId, juce::Colours::white);
	//customLookAndFeel.setColour(juce::ScrollBar::backgroundColourId, juce::Colours::black);
	customLookAndFeel.setColour(juce::ScrollBar::thumbColourId, juce::Colours::black);
	setLookAndFeel(&customLookAndFeel);

	std::array<juce::Button*, 12> buttons = {  
		&loadButton, &muteButton,
		&playPauseButton, &goToStartButton, &goToEndButton,
		&loopButton,&addToPlaylist, &setMarkerAButton,&setMarkerBButton,&clearMarkersButton,
		&backward10sButton,&forward10sButton 
	};

	for (auto* btn : buttons)
	{
		addAndMakeVisible(btn);
		btn->addListener(this);
	}

	volumeslider.setRange(0.0, 1.0, 0.01);
	volumeslider.setValue(0.5);
	volumeslider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60 / 2, 20);
	volumeslider.addListener(this);
	volumeLabel.setText("Volume:", juce::dontSendNotification);
	volumeLabel.attachToComponent(&volumeslider, true);
	addAndMakeVisible(volumeslider);

	speedSlider.setRange(0.25, 4.0, 0.05);
	speedSlider.setValue(1.0);
	speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
	speedSlider.addListener(this);
	addAndMakeVisible(speedSlider);

	speedLabel.setText("Speed:", juce::dontSendNotification);
	speedLabel.attachToComponent(&speedSlider, true);

	playPauseButton.setShape(CreateButtonShape("play"), true, true, true);
	goToStartButton.setShape(CreateButtonShape("goToStart"), true, true, true);
	goToEndButton.setShape(CreateButtonShape("goToEnd"), true, true, true);

	loopButton.setButtonText("Loop: OFF");

	playPauseButton.addListener(this);
	goToStartButton.addListener(this);
	goToEndButton.addListener(this);
	//=========================================
	PlaylistBox.setModel(this);
	addAndMakeVisible(PlaylistBox);
	PlaylistBox.getHeader().addColumn("Track Title", 1, 250);
	PlaylistBox.getHeader().addColumn("Load", 3, 20);
	PlaylistBox.getHeader().addColumn("Delete", 4, 20);
	PlaylistBox.getHeader().setStretchToFitActive(true);
	PlaylistBox.getHeader().setPopupMenuActive(false);
	PlaylistBox.updateContent();

	//===========================================
	addAndMakeVisible(positionSlider);
	positionSlider.setRange(0.0, 1.0, 0.001);
	positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	positionSlider.addListener(this);

	addAndMakeVisible(positionLabel);
	positionLabel.setText("00:00", juce::dontSendNotification);
	positionLabel.setJustificationType(juce::Justification::centredLeft);

	addAndMakeVisible(durationLabel);
	durationLabel.setText("00:00", juce::dontSendNotification);
	durationLabel.setJustificationType(juce::Justification::centredRight);

	startTimer(100);

	//===========================================
	markerA = 0.0;
	markerB = 0.0;
	hasMarkerA = false;
	hasMarkerB = false;

	setMarkerAButton.setButtonText("Set A");
	setMarkerBButton.setButtonText("Set B");
	clearMarkersButton.setButtonText("Clear A-B");
	//***********************************************
	addAndMakeVisible(metadataTextEditor);
	addAndMakeVisible(metadataViewPort);
	//***********************************************	
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
	int w = 60; int h = 20;int mid = (getWidth()) / 2;
	loadButton.setBounds(0, getHeight()/3+10, w, h);
	muteButton.setBounds(mid/2 , getHeight() / 2 + 10, w, h);
	loopButton.setBounds(3*mid/2, getHeight() / 2 + 10, w, h);

	goToStartButton.setBounds(mid - 100, getHeight() / 2 + 10 + 5, 20, 20);
	backward10sButton.setBounds(mid - 70, getHeight() / 2 + 10 + 5, w/2, h);
	playPauseButton.setBounds(mid - 15 , getHeight() / 2 + 10, 30, 30);
	forward10sButton.setBounds(mid + 40, getHeight() / 2 + 10 + 5, w/2, h);
	goToEndButton.setBounds(mid + 80 , getHeight() / 2 + 10 + 5, 20, 20);

	// ===========================================
	int positionY = getHeight() / 2 + 30;
	int positionX = 0;

	positionLabel.setBounds(positionX, 15*getHeight()/24 -10, w, h);
	durationLabel.setBounds(getWidth()-w-5, 15 * getHeight()/ 24 - 10, w, h);
	positionSlider.setBounds(positionX, 15 * getHeight() / 24, getWidth() - positionX, 30);

	speedSlider.setBounds(mid + 100 , positionY, mid-100, 30);

	volumeslider.setBounds(50, positionY, mid-100, 30);

	addToPlaylist.setBounds(w+10, getHeight() / 3 + 10, w, h);
	PlaylistBox.setBounds(0,2*getHeight()/3+50, getWidth(), getHeight() / 3 -50);

	// ===========================================

	int markerY = 15 * getHeight() / 24+40;
	setMarkerAButton.setBounds(mid-70, markerY, w, h);
	setMarkerBButton.setBounds(mid, markerY, w, h);
	clearMarkersButton.setBounds(mid+70, markerY, w, h);
	//******************************************************************************
	metadataTextEditor.setColour(juce::TextEditor::textColourId, juce::Colours::deepskyblue);
	metadataTextEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
	metadataTextEditor.setMultiLine(true);
	metadataTextEditor.setReadOnly(true);
	metadataTextEditor.setFont(juce::Font("Courier New", 14.0f, juce::Font::bold));
	metadataTextEditor.setScrollbarsShown(true);
	metadataTextEditor.setCaretVisible(false);

	metadataTextEditor.setSize(getWidth() / 2, 120);

	metadataViewPort.setScrollBarsShown(true, true);

	metadataViewPort.setViewedComponent(&metadataTextEditor, false);
	metadataViewPort.setBounds(mid/2, getHeight() / 3 + 10, mid, 70);
	//****************************************************************************
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
					thumbnail.setSource(new juce::FileInputSource(file));
					//***************************************
					playerAudio.readMetadata(file);
					metadataText = playerAudio.getMetadata();
					metadataTextEditor.setText(metadataText);
					//**************************************
				}
			});
	}
	else if (button == &muteButton)
	{
		playerAudio.toggleMute();
	}
	else if (button == &playPauseButton) {
		playerAudio.togglePlayer();
		repaint();
	}
	else if (button == &goToStartButton) {
		playerAudio.setPosition(0.0f);
		playerAudio.setPreviousPosition(0.0f);
	}
	else if (button == &goToEndButton) {
		playerAudio.setPosition(playerAudio.getLength());
		playerAudio.setPreviousPosition(0.0f);
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
	else if (button == &backward10sButton) {
		double currentPos = playerAudio.getPosition();
		double newPos = juce::jmax(0.0, currentPos - 10.0);
		playerAudio.setPosition(newPos);
	}
	else if (button == &forward10sButton) {
		double currentPos = playerAudio.getPosition();
		double totalLength = playerAudio.getLength();
		double newPos = juce::jmin(totalLength, currentPos + 10.0);
		playerAudio.setPosition(newPos);
	}
	else if (button == &addToPlaylist) {
		fileChooser = std::make_unique<juce::FileChooser>(
			"Select an audio file...", juce::File{}, "*.wav;*.mp3");

		fileChooser->launchAsync(
			juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
			[this](const juce::FileChooser& fc)
			{
				auto file = fc.getResult();
				if (file.existsAsFile())
				{
					playerAudio.addToPlaylist(file);
					PlaylistBox.updateContent();
					PlaylistBox.repaint();
				}
			});

	}

	else if (button == &setMarkerAButton) {
		markerA = playerAudio.getPosition();
		hasMarkerA = true;
		repaint();
	}

	else if (button == &setMarkerBButton) {
		markerB = playerAudio.getPosition();
		hasMarkerB = true;
		repaint();
	}

	else if (button == &clearMarkersButton) {

		hasMarkerA = false;
		hasMarkerB = false;
		repaint();
	}

	else {
		juce::String buttonID = button->getComponentID();
		juce::StringArray IDParts = juce::StringArray::fromTokens(buttonID, ":", "");
		int rowNumber = IDParts[0].getIntValue();
		if (IDParts.size() == 2) {
			if (IDParts[1] == "delete") {
				playerAudio.deleteTrack(rowNumber);
				PlaylistBox.updateContent();
				PlaylistBox.repaint();
			}
			else if (IDParts[1] == "load") {
				juce::File file = playerAudio.getFile(rowNumber);
				playerAudio.LoadFile(file);
				thumbnail.setSource(new juce::FileInputSource(file));
				//***************************************
				playerAudio.readMetadata(file);
				metadataText = playerAudio.getMetadata();
				metadataTextEditor.setText(metadataText);
				//**************************************
			}
		}
	}
}
void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
	if (slider == &volumeslider)
		playerAudio.setGainFromGUI((float)volumeslider.getValue());
	else if (slider == &positionSlider) {

		double newPosition = positionSlider.getValue() * playerAudio.getLength();
		playerAudio.setPosition(newPosition);
	}
	else if (slider == &speedSlider) {
		playerAudio.setPlaybackSpeed((float)speedSlider.getValue());
	}
}
juce::ShapeButton PlayerGUI::createShapeButton(const juce::String& name) {
	return juce::ShapeButton(
		name, juce::Colours::blueviolet,
		juce::Colours::lightblue,
		juce::Colours::palevioletred);
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
//========================================================
int PlayerGUI::getNumRows() {
	juce::StringArray filesNames = playerAudio.getFilesNames();
	return filesNames.size();
}
void PlayerGUI::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
	if (rowIsSelected) {
		g.fillAll(juce::Colours::blueviolet);
	}
	else {
		g.fillAll(juce::Colours::black);
	}
}
void PlayerGUI::paintCell(juce::Graphics& g, int rowNumber, int columId, int width, int height, bool rowIsSelected) {
	juce::StringArray filesNames = playerAudio.getFilesNames();
	g.setColour(juce::Colours::white);
	g.drawText(filesNames[rowNumber],
		2, 0,
		width, height,
		juce::Justification::centredLeft,
		true);
}
juce::Component* PlayerGUI::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentTpUpdate) {
	if (columnId == 4) {
		if (existingComponentTpUpdate == nullptr) {
			juce::TextButton* d = new juce::TextButton("X");
			d->addListener(this);
			juce::String id{ std::to_string(rowNumber) + ":delete" };
			d->setComponentID(id);
			existingComponentTpUpdate = d;
		}
	}
	else if (columnId == 3) {
		if (existingComponentTpUpdate == nullptr) {
			juce::TextButton* load = new juce::TextButton("+");
			load->addListener(this);
			juce::String id{ std::to_string(rowNumber) + ":load" };
			load->setComponentID(id);
			existingComponentTpUpdate = load;
		}
	}
	return existingComponentTpUpdate;
}
//========================================================

juce::String PlayerGUI::formatTime(double timeInSeconds) {

	int totalSeconds = static_cast<int>(timeInSeconds);
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;
	return juce::String::formatted("%02d:%02d", minutes, seconds);
}

void PlayerGUI::timerCallback() {

	double currentPos = playerAudio.getPosition();
	double totalLength = playerAudio.getLength();

	if (hasMarkerA && hasMarkerB) {

		if (markerA > markerB) {

			double temp = markerA;
			markerA = markerB;
			markerB = temp;
		}

		if (currentPos >= markerB) {
			playerAudio.setPosition(markerA);
			currentPos = markerA;
		}
	}

	if (playerAudio.getLength() > 0) {

		positionSlider.setValue(currentPos / totalLength, juce::dontSendNotification);
		positionLabel.setText(formatTime(currentPos), juce::dontSendNotification);
		durationLabel.setText(formatTime(totalLength), juce::dontSendNotification);
	}
	repaint();
}

//========================================================

void PlayerGUI::paint(juce::Graphics& g) {

	Component::paint(g);

	if (thumbnail.getTotalLength() > 0.0)
	{
		auto area = getLocalBounds();
		auto waveformArea = juce::Rectangle<int>(0, 0, getWidth(),getHeight()/3);

		g.setColour(juce::Colours::lightblue);
		thumbnail.drawChannels(g, waveformArea, 0.0, thumbnail.getTotalLength(), 1.0f);

		double audioPosition = playerAudio.getPosition();
		double proportion = audioPosition / playerAudio.getLength();
		int playheadX = waveformArea.getX() + (int)(waveformArea.getWidth() * proportion);

		g.setColour(juce::Colours::red);
		g.drawLine(playheadX, waveformArea.getY(), playheadX, waveformArea.getBottom(), 3.0f);

		g.setColour(juce::Colours::black);
		g.drawRect(waveformArea, 1);
	}

	auto sliderBounds = positionSlider.getBounds();

	int sliderX = sliderBounds.getX();
	int sliderY = sliderBounds.getY();
	int sliderWidth = sliderBounds.getWidth();
	int sliderHeight = sliderBounds.getHeight();

	double totalLength = playerAudio.getLength();

	if (totalLength <= 0) {
		return;
	}

	if (hasMarkerA) {

		float markerAX = sliderX + (markerA / totalLength) * sliderWidth;

		g.setColour(juce::Colours::deepskyblue);
		g.drawLine(markerAX, sliderY, markerAX, sliderY + sliderHeight, 3.0f);

		juce::Path triangle;

		float triangleSize = 6.0f;

		triangle.startNewSubPath(markerAX, sliderY - 10);
		triangle.lineTo(markerAX - triangleSize, sliderY);

		triangle.lineTo(markerAX + triangleSize, sliderY);
		triangle.closeSubPath();

		g.setColour(juce::Colours::deepskyblue);
		g.fillPath(triangle);
	}

	if (hasMarkerB)
	{
		float markerBX = sliderX + (markerB / totalLength) * sliderWidth;

		g.setColour(juce::Colours::deepskyblue);

		g.drawLine(markerBX, sliderY, markerBX, sliderY + sliderHeight, 3.0f);

		juce::Path triangle;
		float triangleSize = 6.0f;

		triangle.startNewSubPath(markerBX, sliderY - 10);
		triangle.lineTo(markerBX - triangleSize, sliderY);
		triangle.lineTo(markerBX + triangleSize, sliderY);
		triangle.closeSubPath();

		g.setColour(juce::Colours::deepskyblue);
		g.fillPath(triangle);
	}
	if (playerAudio.getPlayerState()) {
		playPauseButton.setShape(CreateButtonShape("pause"), true, true, true);
		playPauseButton.setBounds((getWidth()/2) - 15, getHeight() / 2 + 10, 30, 30);
	}
	else {
		playPauseButton.setShape(CreateButtonShape("play"), true, true, true);
		playPauseButton.setBounds((getWidth() / 2) - 15, getHeight() / 2 + 10, 30, 30);
	}
}

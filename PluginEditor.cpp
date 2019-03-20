#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessorEditor::DelayAudioProcessorEditor (DelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Set size of plugin window
    setSize (500, 300);

	// Obtain reference to plugin parameters
	auto& params = processor.getParameters();


	// Declare parameter for Dry/Wet setting
	AudioParameterFloat* dryWetParameter = (AudioParameterFloat*)params.getUnchecked(0);
	
	// Congifure slider for user control of Dry/Wet parameter
	getLookAndFeel().setColour(Slider::thumbColourId, Colour(0xff46b2fb));
	mDryWetSlider.setBounds(50, 160, 100, 100);
	mDryWetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	mDryWetSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
	mDryWetSlider.setValue(*dryWetParameter);
	addAndMakeVisible(mDryWetSlider);

	// Define functions for slider to parameter communication
	mDryWetSlider.onValueChange = [this, dryWetParameter]{ *dryWetParameter = mDryWetSlider.getValue(); };
	mDryWetSlider.onDragStart = [dryWetParameter] {dryWetParameter->beginChangeGesture(); };
	mDryWetSlider.onDragEnd = [dryWetParameter] {dryWetParameter->endChangeGesture(); };

	// Configure label for Dry/Wet slider
	mDryWetLabel.setColour(Label::textColourId, Colour(0xff46b2fb));
	mDryWetLabel.setFont(Font("Ravie", 20.0f, Font::plain));
	mDryWetLabel.setBounds(50, 260, 100, 30);
	mDryWetLabel.setText("Dry/Wet", dontSendNotification);
	addAndMakeVisible(mDryWetLabel);


	// Declare parameter for Feedback setting
	AudioParameterFloat* feedbackParameter = (AudioParameterFloat*)params.getUnchecked(1);

	// Configure slider for user control of Feedback parameter
	mFeedbackSlider.setBounds(200, 160, 100, 100);
	mFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	mFeedbackSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
	mFeedbackSlider.setValue(*feedbackParameter);
	addAndMakeVisible(mFeedbackSlider);

	// Define functions for slider to parameter communication
	mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = mFeedbackSlider.getValue(); };
	mFeedbackSlider.onDragStart = [feedbackParameter] {feedbackParameter->beginChangeGesture(); };
	mFeedbackSlider.onDragEnd = [feedbackParameter] {feedbackParameter->endChangeGesture(); };

	// Configure label for Feedback slider
	mFeedbackLabel.setColour(Label::textColourId, Colour(0xff46b2fb));
	mFeedbackLabel.setFont(Font("Ravie", 20.0f, Font::plain));
	mFeedbackLabel.setBounds(200, 260, 100, 30);
	mFeedbackLabel.setText("Feedback", dontSendNotification);
	addAndMakeVisible(mFeedbackLabel);


	// Declare parameter for Delay Time setting
	AudioParameterFloat* delayTimeParameter = (AudioParameterFloat*)params.getUnchecked(2);

	// Configure slider for user control of Delay Time parameter
	mDelayTimeSlider.setBounds(350, 160, 100, 100);
	mDelayTimeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	mDelayTimeSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	mDelayTimeSlider.setRange(delayTimeParameter->range.start, delayTimeParameter->range.end);
	mDelayTimeSlider.setValue(*delayTimeParameter);
	addAndMakeVisible(mDelayTimeSlider);

	// Define functions for slider to parameter communication
	mDelayTimeSlider.onValueChange = [this, delayTimeParameter] { *delayTimeParameter = mDelayTimeSlider.getValue(); };
	mDelayTimeSlider.onDragStart = [delayTimeParameter] {delayTimeParameter->beginChangeGesture(); };
	mDelayTimeSlider.onDragEnd = [delayTimeParameter] {delayTimeParameter->endChangeGesture(); };

	// Configure label for Delay Time parameter
	mDelayTimeLabel.setColour(Label::textColourId, Colour(0xff46b2fb));
	mDelayTimeLabel.setFont(Font("Ravie", 20.0f, Font::plain));
	mDelayTimeLabel.setBounds(350, 260, 100, 30);
	mDelayTimeLabel.setText("Delay Time", dontSendNotification);
	addAndMakeVisible(mDelayTimeLabel);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
}

//==============================================================================
void DelayAudioProcessorEditor::paint (Graphics& g)
{
    // Fill background of GUI with light blue color
    g.fillAll (Colour(0xffbafbf6));

	// Fill array of colours to use for GUI painting
	pallette = { Colours::pink, Colours::violet, Colours::white };

	// Create ellipse template
	Ellipse ellipse = createEllipse();

	// Create reference to ellipse template
	Ellipse &ellipseRef = ellipse;

	// Define line thickness
	float lineThickness = 3.0f;

	// Cycle through colours while drawing ellipses
	for (int i = 0; i <= 11; i++)
	{
		for (int j = 0; j <= 3; j++)
		{
			// Set ellipse colour
			g.setColour(pallette[j]);

			// Draw ellipse to the screen
			g.drawEllipse(ellipse.xLocation, ellipse.yLocation, ellipse.width, ellipse.height, lineThickness);

			// Update ellipse size
			updateEllipseSize(ellipseRef);
		}
	}

	// Draw final ellipse to screen
	g.setColour(Colours::white);
	g.drawEllipse(ellipse.xLocation, ellipse.yLocation, ellipse.width, ellipse.height, lineThickness);

	// Draw background title text
	g.setColour(Colours::black);
	g.setFont(Font("Forte", 86.0f, Font::bold));
	g.drawText("Delay", 50, 30, 400, 100, Justification::centred, true);

	// Draw foreground title text
	g.setColour(Colours::white);
    g.setFont (Font("Forte", 80.0f, Font::bold));
	g.drawText("Delay", 50, 30, 400, 100, Justification::centred, true);
}

void DelayAudioProcessorEditor::resized()
{
}

DelayAudioProcessorEditor::Ellipse DelayAudioProcessorEditor::createEllipse()
{
	// Set initial Ellipse location based on GUI window size
	float xLocation = 20;
	float yLocation = 20;
	float width = 460;
	float height = 120;
	float opacity = 1.0f;

	return { xLocation, yLocation, width, height, opacity };
}

void DelayAudioProcessorEditor::updateEllipseSize(DelayAudioProcessorEditor::Ellipse& ellipseRef)
{
	// Update ellipse to be slightly smaller for the next drawing
	ellipseRef.xLocation += 5;
	ellipseRef.yLocation += 2.3;
	ellipseRef.width -= 10;
	ellipseRef.height -= 5;
}
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
	
	createSlider(mDryWetSlider, mDryWetLabel, dryWetParameter, "Dry/Wet", Rectangle<int>(50, 160, 100, 100));

	// Define functions for slider to parameter communication
	mDryWetSlider.onValueChange = [this, dryWetParameter]{ *dryWetParameter = mDryWetSlider.getValue(); };
	mDryWetSlider.onDragStart = [dryWetParameter] {dryWetParameter->beginChangeGesture(); };
	mDryWetSlider.onDragEnd = [dryWetParameter] {dryWetParameter->endChangeGesture(); };


	// Declare parameter for Feedback setting
	AudioParameterFloat* feedbackParameter = (AudioParameterFloat*)params.getUnchecked(1);

	createSlider(mFeedbackSlider, mFeedbackLabel, feedbackParameter, "Feedback", Rectangle<int>(200, 160, 100, 100));

	// Define functions for slider to parameter communication
	mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = mFeedbackSlider.getValue(); };
	mFeedbackSlider.onDragStart = [feedbackParameter] {feedbackParameter->beginChangeGesture(); };
	mFeedbackSlider.onDragEnd = [feedbackParameter] {feedbackParameter->endChangeGesture(); };


	// Declare parameter for Delay Time setting
	AudioParameterFloat* delayTimeParameter = (AudioParameterFloat*)params.getUnchecked(2);

	createSlider(mDelayTimeSlider, mDelayTimeLabel, delayTimeParameter, "Delay Time", Rectangle<int>(350, 160, 100, 100));

	// Define functions for slider to parameter communication
	mDelayTimeSlider.onValueChange = [this, delayTimeParameter] { *delayTimeParameter = mDelayTimeSlider.getValue(); };
	mDelayTimeSlider.onDragStart = [delayTimeParameter] {delayTimeParameter->beginChangeGesture(); };
	mDelayTimeSlider.onDragEnd = [delayTimeParameter] {delayTimeParameter->endChangeGesture(); };
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
}

//==============================================================================
void DelayAudioProcessorEditor::paint (Graphics& g)
{
    // Fill background of GUI with light blue color
    g.fillAll (Colour(0xffbafbf6));

	Array<Colour> pallette = { Colours::pink, Colours::violet, Colours::white };

	Ellipse ellipse = createEllipse();

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
			updateEllipseSize(ellipse);
		}
	}

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

//=========================Self-created functions===============================
void DelayAudioProcessorEditor::createSlider(Slider& slider, Label& label, AudioParameterFloat* parameter, const String& name, Rectangle<int> bounds)
{
	// Configure slider
	getLookAndFeel().setColour(Slider::thumbColourId, Colour(0xff46b2fb));
	slider.setBounds(bounds);
	slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	slider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	slider.setRange(parameter->range.start, parameter->range.end);
	slider.setValue(*parameter);
	addAndMakeVisible(slider);

	// Adjust bounds for label
	bounds.setY(bounds.getY() + 100);
	bounds.setHeight(bounds.getHeight() - 70);

	// Configure label for slider
	label.setColour(Label::textColourId, Colour(0xff46b2fb));
	label.setFont(Font("Ravie", 20.0f, Font::plain));
	label.setBounds(bounds);
	label.setText(name, dontSendNotification);
	addAndMakeVisible(label);
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

void DelayAudioProcessorEditor::updateEllipseSize(Ellipse& ellipse)
{
	// Update ellipse to be slightly smaller for the next drawing
	ellipse.xLocation += 5;
	ellipse.yLocation += 2.3;
	ellipse.width -= 10;
	ellipse.height -= 5;
}
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
class DelayAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DelayAudioProcessorEditor (DelayAudioProcessor&);
    ~DelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	//=========================Self-created functions===============================
	void createSlider(Slider& slider, Label& label, AudioParameterFloat* parameter, const String& name, Rectangle<int> bounds);

	struct Ellipse
	{
		float xLocation, yLocation, width, height, opacity;
	};
	Ellipse createEllipse();
	void updateEllipseSize(Ellipse& ellipse);

private:
    // Reference to the processor object that created this editor
    DelayAudioProcessor& processor;

	// Sliders for GUI
	Slider mFeedbackSlider, mDryWetSlider, mDelayTimeSlider;

	// Labels corresponding to sliders
	Label mFeedbackLabel, mDryWetLabel, mDelayTimeLabel;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};

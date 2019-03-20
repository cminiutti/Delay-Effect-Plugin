/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DelayAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DelayAudioProcessorEditor (DelayAudioProcessor&);
    ~DelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	//==============================================================================
	struct Ellipse
	{
		float xLocation;
		float yLocation;
		float width;
		float height;
		float opacity;
	};
	Ellipse createEllipse();
	void updateEllipseSize(Ellipse& ellipseRef);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayAudioProcessor& processor;

	// Declare sliders for GUI
	Slider mFeedbackSlider;
	Slider mDryWetSlider;
	Slider mDelayTimeSlider;

	// Declare labels corresponding to sliders
	Label mFeedbackLabel;
	Label mDryWetLabel;
	Label mDelayTimeLabel;

	// Declare array of colours for drawing ellipses
	Array<Colour> pallette;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};

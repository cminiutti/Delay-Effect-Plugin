/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#define MAX_DELAY_TIME 2

//==============================================================================
/**
*/
class DelayAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    DelayAudioProcessor();
    ~DelayAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	//==============================================================================
	float linInterpolate(float inSampleX, float inSampleY, float inFloatPhase);
	float smoothParameterChanges(float smoothedVariable, AudioParameterFloat* pluginParameter);
	float calculateDelayReadHead();

	//==============================================================================
	struct readHeads
	{
		int current, next;
		float fraction;
	};
	readHeads getReadHeads();

private:

	// L/R delay buffers
	float* mCircularBufferLeft;
	float* mCircularBufferRight;

	// Size of delay buffers
	int mCircularBufferLength;

	// Delay buffer write and read heads
	int mCircularBufferWriteHead;
	float mDelayReadHead;

	// Variable for delay in units of samples
	float mDelayTimeInSamples;

	// Feedback values applied to delay
	float mFeedbackLeft, mFeedbackRight;

	// Dry/Wet value for output buffer
	float mDryWet;

	// Variable for smoothing of delay time parameter setting
	float mTimeSmoothed;

	// Plugin parameters
	AudioParameterFloat* mTimeParameter;
	AudioParameterFloat* mDryWetParameter;
	AudioParameterFloat* mFeedbackParameter;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessor)
};

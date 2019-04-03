#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	// Add parameters to plugin
	addParameter(mDryWetParameter = new AudioParameterFloat("dryWet", "Dry Wet", 0.0, 1.0, 0.5));
	addParameter(mFeedbackParameter = new AudioParameterFloat("feedback", "Feedback", 0.0, 0.98, 0.5));
	addParameter(mTimeParameter = new AudioParameterFloat("delayTime", "Delay Time", 0.0, MAX_DELAY_TIME, 1.0));

	// Initialize circular buffers to nullptr
	mCircularBufferLeft = nullptr;
	mCircularBufferRight = nullptr;
	
	// Initialize member variables to zero
	mCircularBufferWriteHead = 0;
	mCircularBufferLength = 0;
	mDelayTimeInSamples = 0;
	mDelayReadHead = 0;
	mFeedbackLeft = 0;
	mFeedbackRight = 0;
	mDryWet = 0;
	mTimeSmoothed = 0;
}

DelayAudioProcessor::~DelayAudioProcessor()
{
	// Destroy circular buffers
	if (mCircularBufferLeft != nullptr)
	{
		delete[] mCircularBufferLeft;
		mCircularBufferLeft == nullptr;
	}

	if (mCircularBufferRight != nullptr)
	{
		delete[] mCircularBufferRight;
		mCircularBufferRight == nullptr;
	}
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	// Convert delay time parameter from time units in seconds to samples
	mDelayTimeInSamples = sampleRate * *mTimeParameter;

	// Set size of circular delay buffer in samples
	mCircularBufferLength = sampleRate * MAX_DELAY_TIME;

	// Destroy left circular buffer
	if (mCircularBufferLeft != nullptr)
	{
		delete[] mCircularBufferLeft;
		mCircularBufferLeft == nullptr;
	}

	// Create new delay buffer for left channel
	if (mCircularBufferLeft == nullptr)
	{
		mCircularBufferLeft = new float[(int)mCircularBufferLength];
	}
	
	// Clear any garbage data from left delay buffer
	zeromem(mCircularBufferLeft, mCircularBufferLength * sizeof(float));

	// Destroy right circular buffer
	if (mCircularBufferRight != nullptr)
	{
		delete[] mCircularBufferRight;
		mCircularBufferRight == nullptr;
	}

	// Create new delay buffer for right channel
	if (mCircularBufferRight == nullptr)
	{
		mCircularBufferRight = new float[(int)mCircularBufferLength];
	}

	// Clear any garbage data from right delay buffer
	zeromem(mCircularBufferRight, mCircularBufferLength * sizeof(float));

	// Initialize write head to beginning of delay buffer
	mCircularBufferWriteHead = 0;

	// Initialize smoothed time variable to current time parameter value
	mTimeSmoothed = *mTimeParameter;
}

void DelayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	// Get system I/O
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any garbage data from output buffers
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	// Retrieve write pointers to left and right channel buffers
	float* leftChannel = buffer.getWritePointer(0);
	float* rightChannel = buffer.getWritePointer(1);

	for (int i = 0; i < buffer.getNumSamples(); i++)
	{
		// Write current sample and any feedback to delay buffers at current write head
		mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
		mCircularBufferRight[mCircularBufferWriteHead] = rightChannel[i] + mFeedbackRight;

		// Perform smoothing on delay time parameter to account for quick knob movements
		mTimeSmoothed = smoothParameterChanges(mTimeSmoothed, mTimeParameter);

		// Convert delay parameter from time units of seconds to samples
		mDelayReadHead = calculateDelayReadHead();

		// Get samples for performing linear interpolation
		auto readHead = getReadHeads();

		// Interpolate between samples in delay buffer and assign values to L/R delay samples
		auto delay_sample_left = linInterpolate(mCircularBufferLeft[readHead.current], mCircularBufferLeft[readHead.next], readHead.fraction);
		auto delay_sample_right = linInterpolate(mCircularBufferRight[readHead.current], mCircularBufferRight[readHead.next], readHead.fraction);

		// Store delayed samples as feedback
		mFeedbackLeft = delay_sample_left * *mFeedbackParameter;
		mFeedbackRight = delay_sample_right * *mFeedbackParameter;

		updateWriteHead();

		// Set output buffer with wet/dry mix of input sample and delayed sample
		buffer.setSample(0, i, buffer.getSample(0, i) * (1 - *mDryWetParameter) + delay_sample_left * (*mDryWetParameter));
		buffer.setSample(1, i, buffer.getSample(1, i) * (1 - *mDryWetParameter) + delay_sample_right * (*mDryWetParameter));
	}
}

//========================Self-created functions================================
// Linearly interpolate between two samples if desired delay is fractional
float DelayAudioProcessor::linInterpolate(float inSampleX, float inSampleY, float inFloatPhase)
{
	// Return linear interpolation between two samples
	return (1 - inFloatPhase) * inSampleX + inFloatPhase * inSampleY;
}

// This function prevents parameter values from jumping to fast during processing and causing audible artifacts
float DelayAudioProcessor::smoothParameterChanges(float smoothedVariable, AudioParameterFloat* pluginParameter)
{
	return smoothedVariable = smoothedVariable - 0.0001*(smoothedVariable - *pluginParameter);
}

// Determine current read head for delay buffers
float DelayAudioProcessor::calculateDelayReadHead()
{
	// Convert delay parameter from time units of seconds to samples
	mDelayTimeInSamples = getSampleRate() * mTimeSmoothed;

	// Calculate delay read head
	mDelayReadHead = mCircularBufferWriteHead - mDelayTimeInSamples;

	if (mDelayReadHead < 0)
	{
		mDelayReadHead += mCircularBufferLength;
	}

	return mDelayReadHead;
}

// Retrieve read heads prior to interpolation
DelayAudioProcessor::readHeads DelayAudioProcessor::getReadHeads()
{
	// Get samples for performing linear interpolation
	auto current = (int)mDelayReadHead;
	auto next = current + 1;
	auto fraction = mDelayReadHead - current;

	if (next >= mCircularBufferLength)
	{
		next -= mCircularBufferLength;
	}

	return { current, next, fraction };
}

void DelayAudioProcessor::updateWriteHead()
{
	mCircularBufferWriteHead++;

	if (mCircularBufferWriteHead >= mCircularBufferLength)
	{
		mCircularBufferWriteHead = 0;
	}
}

//==============================================================================
void DelayAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	std::unique_ptr<XmlElement> xml(new XmlElement("Delay"));

	xml->setAttribute("DryWet", *mDryWetParameter);
	xml->setAttribute("Feedback", *mFeedbackParameter);
	xml->setAttribute("Time", *mTimeParameter);

	copyXmlToBinary(*xml, destData);
}

void DelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

	if (xml.get() != nullptr && xml->hasTagName("Delay"))
	{
		*mDryWetParameter = xml->getDoubleAttribute("DryWet");
		*mFeedbackParameter = xml->getDoubleAttribute("Feedback");
		*mTimeParameter = xml->getDoubleAttribute("Time");
	}
}

const String DelayAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool DelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
	return 1;
}

int DelayAudioProcessor::getCurrentProgram()
{
	return 0;
}

void DelayAudioProcessor::setCurrentProgram(int index)
{
}

const String DelayAudioProcessor::getProgramName(int index)
{
	return {};
}

void DelayAudioProcessor::changeProgramName(int index, const String& newName)
{
}

bool DelayAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
	return new DelayAudioProcessorEditor(*this);
}

void DelayAudioProcessor::releaseResources()
{
}

// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new DelayAudioProcessor();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif
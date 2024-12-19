/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

juce::AudioProcessorValueTreeState::ParameterLayout IKReverbAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "size",
        "Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "damping",
        "Damping",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "type",
        "Type",
        juce::StringArray {"ROOM", "HALL", "PLATE", "SPRING", "SHIMMER"},
        0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "predelay",
        "Pre-Delay",
        juce::NormalisableRange<float>(0.0f, 500.0f, 1.0f),
        0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "mix",
        "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modulation",
        "Modulation",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowcut",
        "Low Cut",
        juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f, 0.3f),
        20.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highcut",
        "High Cut",
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f, 0.3f),
        20000.0f));

    return layout;
}

//==============================================================================
IKReverbAudioProcessor::IKReverbAudioProcessor()
    : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = 0.33f;
    reverbParams.dryLevel = 0.67f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);
}

IKReverbAudioProcessor::~IKReverbAudioProcessor()
{
}

//==============================================================================
const juce::String IKReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool IKReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool IKReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool IKReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double IKReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int IKReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int IKReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void IKReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String IKReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void IKReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void IKReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    reverb.prepare(spec);
    preDelay.prepare(spec);
    
    // Initialize filters
    lowCutFilter.prepare(spec);
    highCutFilter.prepare(spec);
    
    // Initialize filter coefficients
    auto lowCutFreq = apvts.getRawParameterValue("lowcut")->load();
    auto highCutFreq = apvts.getRawParameterValue("highcut")->load();
    
    *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, lowCutFreq);
    *highCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, highCutFreq);
    
    // Initialize reverb parameters
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = 0.33f;
    reverbParams.dryLevel = 0.67f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);
}

void IKReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool IKReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void IKReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Update reverb parameters
    auto* sizeParam = apvts.getRawParameterValue("size");
    auto* dampingParam = apvts.getRawParameterValue("damping");
    auto* mixParam = apvts.getRawParameterValue("mix");
    auto* typeParam = apvts.getRawParameterValue("type");
    auto* predelayParam = apvts.getRawParameterValue("predelay");
    auto* modulationParam = apvts.getRawParameterValue("modulation");
    auto* lowCutParam = apvts.getRawParameterValue("lowcut");
    auto* highCutParam = apvts.getRawParameterValue("highcut");

    // Update filter coefficients
    *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, lowCutParam->load());
    *highCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, highCutParam->load());

    // Create audio block for filter processing
    juce::dsp::AudioBlock<float> filterBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> filterContext(filterBlock);
    
    // Apply filters
    lowCutFilter.process(filterContext);
    highCutFilter.process(filterContext);

    // Update reverb parameters based on type
    float baseSize = sizeParam->load();
    float baseDamping = dampingParam->load();
    float modulation = modulationParam->load();
    
    switch (static_cast<int>(typeParam->load()))
    {
        case 0: // Room
            reverbParams.roomSize = baseSize * 0.4f;
            reverbParams.damping = baseDamping * 0.9f;
            reverbParams.width = 0.5f;
            break;
            
        case 1: // Hall
            reverbParams.roomSize = baseSize * 1.0f;
            reverbParams.damping = baseDamping * 0.2f;
            reverbParams.width = 1.0f;
            break;
            
        case 2: // Plate
            reverbParams.roomSize = baseSize * 0.6f;
            reverbParams.damping = baseDamping * 0.1f;
            reverbParams.width = 0.75f;
            break;
            
        case 3: // Spring
            reverbParams.roomSize = baseSize * 0.3f;
            reverbParams.damping = baseDamping * 0.4f;
            reverbParams.width = 0.3f;
            break;
            
        case 4: // Shimmer
            reverbParams.roomSize = baseSize * 0.8f;
            reverbParams.damping = baseDamping * 0.3f;
            reverbParams.width = 1.0f;
            break;
    }
    
    reverbParams.wetLevel = mixParam->load();
    reverbParams.dryLevel = 1.0f - mixParam->load();
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);

    // Pre-delay time in samples
    int predelayTimeSamples = static_cast<int>((predelayParam->load() / 1000.0f) * currentSampleRate);
    preDelay.setDelay(predelayTimeSamples);
    
    // Create temporary buffers for processing
    juce::AudioBuffer<float> wetBuffer(totalNumInputChannels, buffer.getNumSamples());
    wetBuffer.clear();

    // Copy input to wet buffer
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        wetBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
    }

    // Apply pre-delay if needed
    if (predelayTimeSamples > 0)
    {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = wetBuffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                channelData[sample] = preDelay.popSample(channel, channelData[sample]);
            }
        }
    }

    // Process reverb
    juce::dsp::AudioBlock<float> reverbBlock(wetBuffer);
    juce::dsp::ProcessContextReplacing<float> reverbContext(reverbBlock);
    reverb.process(reverbContext);

    // Apply modulation
    if (modulation > 0.0f)
    {
        float modDepth = modulation * 0.002f; // Subtle modulation
        float modSpeed = 3.0f; // 3 Hz modulation rate

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = wetBuffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float modPhase = std::sin(2.0f * juce::MathConstants<float>::pi * shimmerPhase);
                channelData[sample] *= (1.0f + modPhase * modDepth);
                shimmerPhase += modSpeed / currentSampleRate;
                if (shimmerPhase >= 1.0f)
                    shimmerPhase -= 1.0f;
            }
        }
    }

    // Mix dry and wet signals
    float wetMix = mixParam->load();
    float dryMix = 1.0f - wetMix;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* dryData = buffer.getWritePointer(channel);
        auto* wetData = wetBuffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            dryData[sample] = dryData[sample] * dryMix + wetData[sample] * wetMix;
        }
    }
}

float IKReverbAudioProcessor::processShimmer(float input, float amount)
{
    // Simple ring modulation for shimmer effect
    shimmerPhase += 440.0f / currentSampleRate; // 440 Hz modulation
    if (shimmerPhase >= 1.0f)
        shimmerPhase -= 1.0f;
    
    return input * std::sin(2.0f * juce::MathConstants<float>::pi * shimmerPhase) * amount;
}

//==============================================================================
bool IKReverbAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* IKReverbAudioProcessor::createEditor()
{
    return new IKReverbAudioProcessorEditor (*this);
}

//==============================================================================
void IKReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void IKReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IKReverbAudioProcessor();
}

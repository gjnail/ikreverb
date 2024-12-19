/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

juce::AudioProcessorValueTreeState::ParameterLayout IKDistortionAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterBool>(
        "ghostMode", 
        "Ghost Mode", 
        false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drive",
        "Drive",
        juce::NormalisableRange<float>(1.0f, 25.0f, 0.1f),
        1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "output",
        "Output",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "type",
        "Type",
        juce::StringArray {"Soft", "Hard", "Tube", "Foldback", "Sine"},
        0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "tone",
        "Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "mix",
        "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "destroy",
        "Destroy",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f));

    return layout;
}

//==============================================================================
IKDistortionAudioProcessor::IKDistortionAudioProcessor()
    : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    bandpassFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
}

IKDistortionAudioProcessor::~IKDistortionAudioProcessor()
{
}

//==============================================================================
const juce::String IKDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool IKDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool IKDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool IKDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double IKDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int IKDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int IKDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void IKDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String IKDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void IKDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void IKDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    bandpassFilter.prepare(spec);
    bandpassFilter.setResonance(ghostBandpassQ);
    bandpassFilter.setCutoffFrequency(ghostBandpassFreq);
}

void IKDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool IKDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

float IKDistortionAudioProcessor::processDistortion(float input, DistortionType type)
{
    switch (type)
    {
        case DistortionType::Soft:
            return std::tanh(input);
            
        case DistortionType::Hard:
            return input > 1.0f ? 1.0f : (input < -1.0f ? -1.0f : input);
            
        case DistortionType::Tube:
        {
            // Asymmetric tube-style distortion
            float x = input;
            if (x > 0)
                return 1.0f - std::exp(-x);
            else
                return -1.0f + std::exp(x);
        }
            
        case DistortionType::Foldback:
        {
            // Foldback distortion
            float threshold = 0.5f;
            float x = input;
            while (std::abs(x) > threshold) {
                x = std::abs(x > 0 ? threshold - (x - threshold) 
                                  : -(threshold - (-x - threshold)));
            }
            return x;
        }
            
        case DistortionType::Sine:
            // Sine waveshaping
            return std::sin(input * juce::MathConstants<float>::pi * 0.5f);
            
        default:
            return input;
    }
}

float IKDistortionAudioProcessor::processDestroy(float input, float amount)
{
    if (amount <= 0.0f) return input;
    
    // Super aggressive input gain
    float inputGain = 1.0f + (amount * 12.0f); // Even higher input gain
    float boostedInput = input * inputGain;
    
    // Keep original low end with massive emphasis
    float lowEnd = std::tanh(boostedInput * 0.8f) * (1.0f + amount * 2.5f);
    
    // Fixed resonant peak frequency for maximum impact
    float frequency = 150.0f + (amount * amount) * 200.0f;  // Nonlinear frequency scaling
    float q = 15.0f + amount * 35.0f; // Fixed high resonance that increases with amount
    
    // Calculate filter coefficients
    float omega = 2.0f * juce::MathConstants<float>::pi * frequency / (float)currentSampleRate;
    float alpha = std::sin(omega) / (2.0f * q);
    
    float b0 = alpha;
    float b1 = 0.0f;
    float b2 = -alpha;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * std::cos(omega);
    float a2 = 1.0f - alpha;
    
    // Normalize coefficients
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
    
    // Apply the resonant filter
    float resonant = b0 * boostedInput + b1 * destroyFilterL[0] + b2 * destroyFilterL[1]
                              - a1 * destroyFilterL[2] - a2 * destroyFilterL[3];
    
    // Update filter state
    destroyFilterL[1] = destroyFilterL[0];
    destroyFilterL[0] = boostedInput;
    destroyFilterL[3] = destroyFilterL[2];
    destroyFilterL[2] = resonant;
    
    // Extreme multi-stage distortion
    float resonantGain = 1.0f + amount * 15.0f; // Much more gain
    resonant *= resonantGain;
    
    // First stage - hard clip with pre-emphasis
    resonant *= 1.0f + (amount * 2.0f);
    resonant = std::tanh(resonant * 2.5f);
    
    // Second stage - aggressive waveshaping
    resonant = std::sin(resonant * juce::MathConstants<float>::pi * 0.5f);
    
    // Third stage - fold back distortion
    float threshold = 0.6f;
    while (std::abs(resonant) > threshold) {
        resonant = threshold - (std::abs(resonant) - threshold);
    }
    
    // Mix signals with phase alignment
    float lowEndMix = 0.6f + (amount * 0.4f); // 60-100% low end
    float resonantMix = amount * 1.5f; // More aggressive resonance mix
    
    float output = (lowEnd * lowEndMix) + (resonant * resonantMix);
    
    // Final saturation stages
    output *= 1.0f + amount * 3.0f; // More output gain
    output = std::tanh(output * 2.0f); // First clip
    output *= 1.0f + amount * 1.0f; // Additional gain
    output = std::tanh(output); // Final clip
    
    return output;
}

void IKDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Get all parameters
    auto* ghostModeParam = apvts.getRawParameterValue("ghostMode");
    auto* driveParam = apvts.getRawParameterValue("drive");
    auto* outputParam = apvts.getRawParameterValue("output");
    auto* typeParam = apvts.getRawParameterValue("type");
    auto* toneParam = apvts.getRawParameterValue("tone");
    auto* mixParam = apvts.getRawParameterValue("mix");
    auto* destroyParam = apvts.getRawParameterValue("destroy");
    
    ghostModeEnabled = ghostModeParam->load() > 0.5f;
    float drive = driveParam->load();
    float output = outputParam->load();
    float tone = toneParam->load();
    float mix = mixParam->load();
    float destroyAmount = destroyParam->load() / 100.0f;
    auto type = static_cast<DistortionType>(static_cast<int>(typeParam->load()));
    
    // Tone control coefficients (simple low-pass filter)
    const float alpha = tone * 0.99f;
    const float beta = 1.0f - alpha;

    if (ghostModeEnabled)
    {
        // Update ghost mode filter parameters
        float modFreq = 1000.0f + std::sin(currentSampleRate * 0.00001f) * 500.0f; // Subtle frequency modulation
        bandpassFilter.setCutoffFrequency(modFreq);
        bandpassFilter.setResonance(5.0f); // High resonance for that ghostly sound
        
        // Apply bandpass filter and additional processing in ghost mode
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float input = channelData[sample];
                
                // Apply bandpass filter
                float filtered = bandpassFilter.processSample(channel, input);
                
                // Add some ghostly distortion
                filtered = std::tanh(filtered * 2.0f);
                
                // Mix with original
                channelData[sample] = filtered * 0.8f + input * 0.2f;
            }
        }
    }

    // Process each channel
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        float& lastSample = (channel == 0) ? lastSampleL : lastSampleR;
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const float cleanSample = channelData[sample];
            float input = cleanSample * drive;
            
            // Apply distortion
            float processed = processDistortion(input, type);
            
            // Apply tone control (low-pass filter)
            processed = processed * beta + lastSample * alpha;
            lastSample = processed;
            
            // Apply destroy effect
            processed = processDestroy(processed, destroyAmount);
            
            // Mix dry/wet
            processed = cleanSample * (1.0f - mix) + processed * mix;
            
            // Apply output level
            channelData[sample] = processed * output;
        }
    }
}

//==============================================================================
bool IKDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* IKDistortionAudioProcessor::createEditor()
{
    return new IKDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void IKDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void IKDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IKDistortionAudioProcessor();
}

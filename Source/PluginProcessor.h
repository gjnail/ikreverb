/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class IKReverbAudioProcessor  : public juce::AudioProcessor
{
public:
    enum class ReverbType
    {
        Room,       // Small room reverb
        Hall,       // Concert hall reverb
        Plate,      // Plate reverb simulation
        Spring,     // Spring reverb simulation
        Shimmer,    // Shimmer reverb with pitch shifting
        NumTypes
    };

    //==============================================================================
    IKReverbAudioProcessor();
    ~IKReverbAudioProcessor() override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Made public for visualization
    float processShimmer(float input, float amount);

    juce::AudioProcessorValueTreeState apvts;
    juce::dsp::Reverb reverb;
    juce::dsp::DelayLine<float> preDelay { 192000 }; // 2 seconds at 96kHz
    bool shimmerEnabled = false;

private:
    //==============================================================================
    // Reverb parameters
    juce::dsp::Reverb::Parameters reverbParams;
    
    // Filters
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowCutFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highCutFilter;
    
    // Shimmer effect state
    float shimmerMix = 0.0f;
    float shimmerPhase = 0.0f;
    
    double currentSampleRate = 44100.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IKReverbAudioProcessor)
};

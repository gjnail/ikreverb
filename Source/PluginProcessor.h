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
class IKDistortionAudioProcessor  : public juce::AudioProcessor
{
public:
    enum class DistortionType
    {
        Soft,       // Soft clipping (tanh)
        Hard,       // Hard clipping
        Tube,       // Tube-style saturation
        Foldback,   // Foldback distortion
        Sine,       // Sine waveshaping
        NumTypes
    };

    //==============================================================================
    IKDistortionAudioProcessor();
    ~IKDistortionAudioProcessor() override;

    // Add the declaration here
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

    juce::AudioProcessorValueTreeState& getParameters() { return apvts; }

    // Made public for visualization
    float processDistortion(float input, DistortionType type);
    float processDestroy(float input, float amount);

    juce::AudioProcessorValueTreeState apvts;
    juce::dsp::StateVariableTPTFilter<float> bandpassFilter;
    bool ghostModeEnabled = false;

private:
    //==============================================================================
    // Tone filter state
    float lastSampleL = 0.0f;
    float lastSampleR = 0.0f;
    
    // Destroy filter state
    float destroyFilterL[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float destroyFilterR[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    double currentSampleRate = 44100.0;
    
    float ghostBandpassFreq = 1000.0f;
    float ghostBandpassQ = 0.707f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IKDistortionAudioProcessor)
};

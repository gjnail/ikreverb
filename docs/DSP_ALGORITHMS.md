# DSP Algorithms Documentation

## Overview
This document details the digital signal processing (DSP) algorithms used in the IK Distortion plugin. Each algorithm is designed for specific sonic characteristics and use cases.

## Core Distortion Algorithms

### 1. Soft Clipping
```cpp
float output = std::tanh(input);
```
- Uses hyperbolic tangent function
- Characteristics:
  - Smooth transition into distortion
  - Natural-sounding compression
  - Odd harmonics generation
  - Maintains musical character at high drive levels
- Best for:
  - Subtle warmth
  - Musical saturation
  - Bass enhancement

### 2. Hard Clipping
```cpp
float output = input > 1.0f ? 1.0f : (input < -1.0f ? -1.0f : input);
```
- Simple threshold-based clipping
- Characteristics:
  - Sharp transition at threshold
  - Aggressive harmonic generation
  - Digital character
  - Rich in odd harmonics
- Best for:
  - Rock/Metal tones
  - Lo-fi effects
  - Digital distortion

### 3. Tube Simulation
```cpp
float output = x > 0 ? 1.0f - std::exp(-x) : -1.0f + std::exp(x);
```
- Asymmetric waveshaping
- Characteristics:
  - Different response for positive/negative signals
  - Natural compression characteristics
  - Mix of even and odd harmonics
  - Smooth transition into saturation
- Best for:
  - Analog warmth
  - Guitar amplifier simulation
  - Vocal enhancement

### 4. Foldback Distortion
```cpp
float threshold = 0.5f;
while (std::abs(x) > threshold) {
    x = std::abs(x > 0 ? threshold - (x - threshold) 
                      : -(threshold - (-x - threshold)));
}
```
- Signal folding algorithm
- Characteristics:
  - Complex harmonic generation
  - Unique waveshaping
  - Non-linear response
  - Extreme effect at high drive levels
- Best for:
  - Sound design
  - Special effects
  - Experimental sounds

### 5. Sine Waveshaping
```cpp
float output = std::sin(input * juce::MathConstants<float>::pi * 0.5f);
```
- Trigonometric waveshaping
- Characteristics:
  - Predictable harmonic content
  - Smooth distortion character
  - Musical aliasing
  - Controlled response
- Best for:
  - Clean distortion
  - Subtle harmonics
  - Musical coloration

## Special Processing Features

### Destroy Mode
```cpp
float inputGain = 1.0f + (amount * 12.0f);
float boostedInput = input * inputGain;
float lowEnd = std::tanh(boostedInput * 0.8f) * (1.0f + amount * 2.5f);

// Resonant filter
float frequency = 150.0f + (amount * amount) * 200.0f;
float q = 15.0f + amount * 35.0f;
```
- Multi-stage processing:
  1. Aggressive input gain
  2. Enhanced low end
  3. Resonant filtering
- Characteristics:
  - Extreme distortion
  - Frequency-dependent processing
  - Dynamic resonance
  - Non-linear response
- Parameters:
  - Amount: 0.0 - 100.0
  - Frequency range: 150Hz - 350Hz
  - Q range: 15 - 50

### Ghost Mode
```cpp
bandpassFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
bandpassFilter.setResonance(ghostBandpassQ);  // 0.707
bandpassFilter.setCutoffFrequency(ghostBandpassFreq);  // 1000Hz
```
- State Variable TPT Filter
- Characteristics:
  - Focused frequency range
  - Controlled resonance
  - Clean filter response
  - Minimal phase distortion
- Parameters:
  - Fixed frequency: 1000Hz
  - Fixed Q: 0.707

## Parameter Smoothing

### Drive Parameter
- Logarithmic scaling
- Range: 1.0 - 25.0
- Smoothing time: ~20ms

### Output Level
- Linear scaling
- Range: 0.0 - 1.0
- Smoothing time: ~10ms

### Tone Control
- Linear to exponential mapping
- Affects pre/post distortion filtering
- Smoothing time: ~15ms

## CPU Optimization

### SIMD Optimization
- Vectorized processing where applicable
- Batch processing for efficiency
- Aligned memory access

### Memory Management
- No allocations in audio thread
- Pre-allocated buffers
- Efficient parameter updates

## Technical Specifications

### Sample Rate Support
- 44.1kHz - 192kHz
- Internal oversampling for high-quality modes
- Automatic sample rate adaptation

### Buffer Size
- Supports 64 - 2048 samples
- Automatic latency compensation
- Efficient buffer processing

### Bit Depth
- 32-bit floating point processing
- 24-bit output dithering (optional)
- Full headroom preservation 
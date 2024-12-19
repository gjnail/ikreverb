# IK Distortion Plugin Architecture

## Overview
IK Distortion is a versatile audio distortion plugin built using the JUCE framework. The plugin implements multiple distortion algorithms and advanced sound shaping features in a modular, maintainable architecture.

## Core Components

### 1. Audio Processor (`IKDistortionAudioProcessor`)
- Main DSP processing class
- Handles real-time audio processing
- Manages parameter state
- Implements distortion algorithms

### 2. GUI (`IKDistortionAudioProcessorEditor`)
- Handles user interface and controls
- Real-time parameter visualization
- Custom knob and button components

## Signal Flow
1. Input Stage
2. Drive Gain
3. Distortion Processing (Multiple Algorithms)
4. Tone Shaping
5. Destroy Effect (Optional)
6. Ghost Mode Processing (Optional)
7. Output Level

## Parameter Management
- Uses JUCE's AudioProcessorValueTreeState for parameter handling
- Automated parameter saving/loading
- Thread-safe parameter updates

## DSP Implementation Details

### Distortion Algorithms
1. **Soft Clipping**
   - Uses tanh() for smooth saturation
   - Maintains harmonic content while limiting peaks

2. **Hard Clipping**
   - Simple threshold-based clipping
   - More aggressive harmonic generation

3. **Tube Simulation**
   - Asymmetric waveshaping
   - Emulates vacuum tube characteristics
   - Uses exponential functions for natural response

4. **Foldback Distortion**
   - Signal folding at threshold points
   - Creates complex harmonic content
   - Useful for extreme effects

5. **Sine Waveshaping**
   - Sine-based transfer function
   - Smooth harmonic generation
   - Musical aliasing characteristics

### Special Features

#### Destroy Mode
- Multi-stage processing:
  1. Aggressive input gain
  2. Low-end enhancement
  3. Resonant filtering
  - Frequency: 150Hz - 350Hz (amount dependent)
  - Q: 15 - 50 (amount dependent)

#### Ghost Mode
- Bandpass filtering
- State Variable TPT Filter implementation
- Frequency: 1000Hz
- Q: 0.707 (default)

## Performance Considerations
- Optimized DSP algorithms
- Efficient parameter smoothing
- Minimal memory allocation during processing
- Thread-safe parameter updates

## Future Architecture Considerations
1. Potential for parallel processing
2. Module expansion capability
3. Preset management system
4. MIDI control implementation 
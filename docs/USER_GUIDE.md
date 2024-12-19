# IK Distortion User Guide

## Overview
IK Distortion is a powerful and versatile distortion plugin that offers multiple distortion types and sound shaping capabilities. From subtle warmth to extreme destruction, this plugin covers all your distortion needs.

## Controls

### Main Parameters

#### Drive (1.0 - 25.0)
- Controls the input gain before distortion
- Higher values result in more intense distortion
- Sweet spot typically between 3.0-8.0 for musical distortion

#### Output (0.0 - 1.0)
- Controls the final output level
- Use to compensate for increased gain from distortion
- Recommended to adjust after setting drive level

#### Mix (0.0 - 1.0)
- Blends between dry (original) and wet (distorted) signal
- 0.0 = 100% dry signal
- 1.0 = 100% wet signal
- Use for parallel distortion effects

### Distortion Types

1. **Soft Clipping**
   - Smooth, musical distortion
   - Great for subtle warmth and saturation
   - Ideal for bass and lead sounds

2. **Hard Clipping**
   - Aggressive, digital distortion
   - Creates sharp harmonics
   - Perfect for rock and metal tones

3. **Tube**
   - Emulates analog tube distortion
   - Adds warmth and character
   - Asymmetric response for even/odd harmonics

4. **Foldback**
   - Complex waveshaping distortion
   - Creates unique harmonic content
   - Great for experimental sounds

5. **Sine**
   - Smooth, predictable distortion
   - Musical harmonics
   - Good for subtle coloration

### Special Features

#### Destroy Mode (0.0 - 100.0)
- Extreme distortion effect
- Enhances low end
- Adds resonant filtering
- Use sparingly for special effects
- Higher values create more chaos

#### Ghost Mode
- Adds bandpass filtering to the signal
- Creates haunting, filtered tones
- Great for sound design
- Toggle on/off for dramatic effect

#### Tone Control (0.0 - 1.0)
- Shapes the frequency response
- Lower values emphasize bass
- Higher values emphasize treble

## Tips & Tricks

1. **Clean Boost**
   - Use Soft Clipping with low Drive
   - Keep Mix at 100%
   - Adjust Output to taste

2. **Warm Saturation**
   - Use Tube mode
   - Drive between 2.0-5.0
   - Mix around 70-80%

3. **Extreme Effects**
   - Enable Destroy Mode
   - Use Foldback distortion
   - Experiment with Ghost Mode
   - Mix to taste

4. **Bass Enhancement**
   - Use Soft or Tube mode
   - Lower Tone control
   - Moderate Drive (3.0-6.0)

5. **Lead Tones**
   - Use Hard Clipping
   - Higher Drive values
   - Raise Tone control
   - Mix 100%

## Troubleshooting

### Common Issues

1. **Too Much Distortion**
   - Reduce Drive
   - Lower Mix level
   - Adjust Output accordingly

2. **Not Enough Effect**
   - Increase Drive
   - Ensure Mix is at 100%
   - Try different distortion types

3. **Unwanted Noise**
   - Reduce Drive
   - Check input levels
   - Adjust Tone control

4. **CPU Usage**
   - Disable Ghost Mode if not needed
   - Reduce Destroy amount
   - Check buffer size settings 
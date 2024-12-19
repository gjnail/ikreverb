 /*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class IKDistortionLookAndFeel : public juce::LookAndFeel_V4
{
public:
    IKDistortionLookAndFeel()
    {
        // 90s neon/retro colors
        setColour(juce::Slider::thumbColourId, juce::Colour(255, 0, 255));  // Hot pink
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0, 255, 255));  // Cyan
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(255, 255, 0));  // Yellow
        setColour(juce::ComboBox::backgroundColourId, juce::Colours::black.withAlpha(0.8f));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0, 255, 255));  // Cyan
        setColour(juce::ComboBox::buttonColourId, juce::Colour(255, 0, 255));  // Hot pink
        setColour(juce::ComboBox::arrowColourId, juce::Colour(255, 255, 0));  // Yellow
        setColour(juce::PopupMenu::backgroundColourId, juce::Colours::black.withAlpha(0.9f));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(255, 0, 255));  // Hot pink
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                         const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Drop shadow
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.fillEllipse(rx + 3, ry + 3, rw, rw);

        // Outer bevel (light from top-left)
        g.setGradientFill(juce::ColourGradient(
            juce::Colours::white.withAlpha(0.3f), rx, ry,
            juce::Colours::black.withAlpha(0.3f), rx + rw, ry + rw,
            true));
        g.fillEllipse(rx, ry, rw, rw);

        // Main knob body
        auto mainGradient = juce::ColourGradient(
            juce::Colour(0, 255, 255).withAlpha(0.8f), rx + rw * 0.3f, ry + rw * 0.3f,
            juce::Colour(0, 150, 150), rx + rw * 0.7f, ry + rw * 0.7f,
            true);
        mainGradient.addColour(0.5, juce::Colour(0, 200, 200));
        g.setGradientFill(mainGradient);
        g.fillEllipse(rx + 2, ry + 2, rw - 4, rw - 4);

        // Inner bevel
        g.setGradientFill(juce::ColourGradient(
            juce::Colours::black.withAlpha(0.2f), rx + 4, ry + 4,
            juce::Colours::white.withAlpha(0.2f), rx + rw - 4, ry + rw - 4,
            true));
        g.fillEllipse(rx + 4, ry + 4, rw - 8, rw - 8);

        // Highlight glare
        auto glareGradient = juce::ColourGradient(
            juce::Colours::white.withAlpha(0.4f), rx + rw * 0.3f, ry + rw * 0.3f,
            juce::Colours::transparentWhite, rx + rw * 0.7f, ry + rw * 0.7f,
            true);
        g.setGradientFill(glareGradient);
        g.fillEllipse(rx + 4, ry + 4, rw - 8, rw - 8);

        // Draw grip marks
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        for (int i = 0; i < 9; ++i)
        {
            float markAngle = angle - juce::MathConstants<float>::pi * 0.75f + i * juce::MathConstants<float>::pi * 0.2f;
            float markLength = radius * 0.3f;
            float markThickness = 2.0f;
            
            juce::Path mark;
            mark.addRectangle(-markThickness * 0.5f, -radius + 8, markThickness, markLength);
            
            // Shadow
            g.setColour(juce::Colours::black.withAlpha(0.5f));
            g.fillPath(mark, juce::AffineTransform::rotation(markAngle).translated(centreX + 1, centreY + 1));
            
            // Mark
            g.setColour(juce::Colours::white.withAlpha(0.4f));
            g.fillPath(mark, juce::AffineTransform::rotation(markAngle).translated(centreX, centreY));
        }

        // Draw pointer
        juce::Path pointer;
        auto pointerLength = radius * 0.7f;
        auto pointerThickness = 4.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -radius + 6, pointerThickness, pointerLength);
        
        // Pointer shadow
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centreX + 1, centreY + 1));
        
        // Pointer
        g.setGradientFill(juce::ColourGradient(
            juce::Colour(255, 255, 0),  // Yellow
            centreX, centreY - radius/2,
            juce::Colour(255, 200, 0),  // Darker yellow
            centreX, centreY,
            false));
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    }
};

// Custom look and feel for the destroy knob
class DestroyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DestroyLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(255, 50, 50));  // Bright red
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(200, 0, 0));  // Dark red
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(255, 100, 100));  // Light red
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::red);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                         const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Drop shadow
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.fillEllipse(rx + 3, ry + 3, rw, rw);

        // Outer bevel (light from top-left)
        g.setGradientFill(juce::ColourGradient(
            juce::Colours::white.withAlpha(0.3f), rx, ry,
            juce::Colours::black.withAlpha(0.3f), rx + rw, ry + rw,
            true));
        g.fillEllipse(rx, ry, rw, rw);

        // Main knob body with metallic red gradient
        auto mainGradient = juce::ColourGradient(
            juce::Colour(255, 50, 50).withAlpha(0.9f), rx + rw * 0.3f, ry + rw * 0.3f,
            juce::Colour(120, 0, 0), rx + rw * 0.7f, ry + rw * 0.7f,
            true);
        mainGradient.addColour(0.5, juce::Colour(200, 20, 20));
        g.setGradientFill(mainGradient);
        g.fillEllipse(rx + 2, ry + 2, rw - 4, rw - 4);

        // Inner bevel
        g.setGradientFill(juce::ColourGradient(
            juce::Colours::black.withAlpha(0.3f), rx + 4, ry + 4,
            juce::Colours::white.withAlpha(0.2f), rx + rw - 4, ry + rw - 4,
            true));
        g.fillEllipse(rx + 4, ry + 4, rw - 8, rw - 8);

        // Highlight glare
        auto glareGradient = juce::ColourGradient(
            juce::Colours::white.withAlpha(0.4f), rx + rw * 0.3f, ry + rw * 0.3f,
            juce::Colours::transparentWhite, rx + rw * 0.7f, ry + rw * 0.7f,
            true);
        g.setGradientFill(glareGradient);
        g.fillEllipse(rx + 4, ry + 4, rw - 8, rw - 8);

        // Draw grip marks with glow effect
        for (int i = 0; i < 12; ++i)
        {
            float markAngle = angle - juce::MathConstants<float>::pi * 0.8f + i * juce::MathConstants<float>::pi * 0.15f;
            float markLength = radius * 0.35f;
            float markThickness = 2.5f;
            
            juce::Path mark;
            mark.addRectangle(-markThickness * 0.5f, -radius + 8, markThickness, markLength);
            
            // Glow
            g.setColour(juce::Colours::red.withAlpha(0.3f));
            g.fillPath(mark, juce::AffineTransform::rotation(markAngle).translated(centreX, centreY));
            
            // Shadow
            g.setColour(juce::Colours::black.withAlpha(0.5f));
            g.fillPath(mark, juce::AffineTransform::rotation(markAngle).translated(centreX + 1, centreY + 1));
            
            // Mark
            g.setColour(juce::Colours::white.withAlpha(0.5f));
            g.fillPath(mark, juce::AffineTransform::rotation(markAngle).translated(centreX, centreY));
        }

        // Draw pointer with glow effect
        juce::Path pointer;
        auto pointerLength = radius * 0.75f;
        auto pointerThickness = 5.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -radius + 6, pointerThickness, pointerLength);
        
        // Glow
        g.setColour(juce::Colours::red.withAlpha(0.4f));
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        
        // Pointer shadow
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centreX + 1, centreY + 1));
        
        // Pointer
        g.setGradientFill(juce::ColourGradient(
            juce::Colour(255, 50, 50),  // Bright red
            centreX, centreY - radius/2,
            juce::Colour(200, 0, 0),  // Dark red
            centreX, centreY,
            false));
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    }
};

class SaturationVisualizer : public juce::Component, public juce::Timer
{
public:
    SaturationVisualizer() : processor(nullptr)
    {
        startTimerHz(30); // Update at 30fps
    }

    void setProcessor(IKDistortionAudioProcessor* p)
    {
        processor = p;
    }

    void paint(juce::Graphics& g) override
    {
        if (!processor) return;
        
        // Background
        g.fillAll(juce::Colours::black.withAlpha(0.3f));
        
        auto bounds = getLocalBounds().toFloat().reduced(4);
        
        // Draw grid
        g.setColour(juce::Colours::cyan.withAlpha(0.2f));
        for (float x = 0; x <= 1.0f; x += 0.25f)
        {
            float xPos = bounds.getX() + x * bounds.getWidth();
            g.drawVerticalLine(int(xPos), bounds.getY(), bounds.getBottom());
        }
        for (float y = 0; y <= 1.0f; y += 0.25f)
        {
            float yPos = bounds.getY() + y * bounds.getHeight();
            g.drawHorizontalLine(int(yPos), bounds.getX(), bounds.getRight());
        }

        // Draw transfer function
        juce::Path curve;
        const int numPoints = 512;
        bool first = true;

        auto type = static_cast<IKDistortionAudioProcessor::DistortionType>(
            static_cast<int>(processor->getParameters().getParameter("type")->getValue() * 4.0f));
        float drive = processor->getParameters().getParameter("drive")->getValue() * 24.0f + 1.0f;
        float destroy = processor->getParameters().getParameter("destroy")->getValue();

        for (int i = 0; i < numPoints; ++i)
        {
            float input = (i / float(numPoints - 1)) * 2.0f - 1.0f;
            float output = input;
            
            // Apply drive
            output *= drive;
            
            // Apply distortion
            output = processor->processDistortion(output, type);
            
            // Apply destroy if active
            if (destroy > 0.0f)
            {
                output = processor->processDestroy(output, destroy);
            }

            float x = bounds.getX() + (input + 1.0f) * 0.5f * bounds.getWidth();
            float y = bounds.getBottom() - (output + 1.0f) * 0.5f * bounds.getHeight();

            if (first)
            {
                curve.startNewSubPath(x, y);
                first = false;
            }
            else
            {
                curve.lineTo(x, y);
            }
        }

        // Draw curve with glow effect
        g.setColour(juce::Colours::black);
        g.strokePath(curve, juce::PathStrokeType(2.5f));
        
        g.setColour(juce::Colours::magenta.withAlpha(0.5f));
        g.strokePath(curve, juce::PathStrokeType(2.0f));
        
        g.setColour(juce::Colours::cyan);
        g.strokePath(curve, juce::PathStrokeType(1.0f));
    }

    void timerCallback() override
    {
        repaint();
    }

private:
    IKDistortionAudioProcessor* processor;
};

//==============================================================================
/**
*/
class IKDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                       public juce::Timer
{
public:
    IKDistortionAudioProcessorEditor (IKDistortionAudioProcessor&);
    ~IKDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void drawCheckerboard(juce::Graphics& g, juce::Rectangle<int> area);
    void drawGhostModeBackground(juce::Graphics& g);
    void drawNormalBackground(juce::Graphics& g);
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    IKDistortionAudioProcessor& audioProcessor;
    IKDistortionLookAndFeel customLookAndFeel;
    DestroyLookAndFeel destroyLookAndFeel;

    juce::Slider driveSlider;
    juce::Slider outputSlider;
    juce::Slider toneSlider;
    juce::Slider mixSlider;
    juce::Slider destroySlider;
    juce::ComboBox typeBox;
    
    juce::Label driveLabel;
    juce::Label outputLabel;
    juce::Label toneLabel;
    juce::Label mixLabel;
    juce::Label destroyLabel;
    juce::Label typeLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> destroyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;

    SaturationVisualizer visualizer;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ghostModeAttachment;
    juce::TextButton ghostModeButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IKDistortionAudioProcessorEditor)
};

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class IKReverbLookAndFeel : public juce::LookAndFeel_V4
{
public:
    IKReverbLookAndFeel()
    {
        // 90s neon/retro colors like IK Distortion
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

class ReverbVisualizer : public juce::Component, public juce::Timer
{
public:
    ReverbVisualizer() : processor(nullptr)
    {
        startTimerHz(30); // Update at 30fps
    }

    void setProcessor(IKReverbAudioProcessor* p)
    {
        processor = p;
    }

    void paint(juce::Graphics& g) override
    {
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

        // Draw impulse response visualization
        if (processor)
        {
            juce::Path curve;
            const int numPoints = 512;
            bool first = true;

            auto type = static_cast<int>(processor->getAPVTS().getParameter("type")->getValue() * 4.0f);
            float size = processor->getAPVTS().getParameter("size")->getValue();
            float damping = processor->getAPVTS().getParameter("damping")->getValue();

            for (int i = 0; i < numPoints; ++i)
            {
                float x = i / float(numPoints - 1);
                float y = std::exp(-x * (5.0f + damping * 10.0f)) * std::sin(x * 50.0f * (1.0f + size));
                y = y * 0.5f + 0.5f; // Normalize to 0-1

                float xPos = bounds.getX() + x * bounds.getWidth();
                float yPos = bounds.getBottom() - y * bounds.getHeight();

                if (first)
                {
                    curve.startNewSubPath(xPos, yPos);
                    first = false;
                }
                else
                {
                    curve.lineTo(xPos, yPos);
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
    }

    void timerCallback() override
    {
        repaint();
    }

private:
    IKReverbAudioProcessor* processor;
};

class IKReverbAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Timer
{
public:
    IKReverbAudioProcessorEditor (IKReverbAudioProcessor&);
    ~IKReverbAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void drawCheckerboard(juce::Graphics& g, juce::Rectangle<int> area);
    void drawBackground(juce::Graphics& g);

    IKReverbAudioProcessor& audioProcessor;
    IKReverbLookAndFeel customLookAndFeel;

    juce::Slider sizeSlider;
    juce::Slider mixSlider;
    juce::Slider predelaySlider;
    juce::Slider lowCutSlider;
    juce::Slider highCutSlider;
    juce::ComboBox typeBox;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label sizeLabel;
    juce::Label mixLabel;
    juce::Label predelayLabel;
    juce::Label lowCutLabel;
    juce::Label highCutLabel;
    juce::Label typeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> predelayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowCutAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highCutAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;

    ReverbVisualizer visualizer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IKReverbAudioProcessorEditor)
};

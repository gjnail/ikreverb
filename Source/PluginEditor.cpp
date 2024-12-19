/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

IKReverbAudioProcessorEditor::IKReverbAudioProcessorEditor (IKReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set custom look and feel
    setLookAndFeel(&customLookAndFeel);

    // Title and subtitle
    titleLabel.setText("INTERNET KIDS", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("REVERB", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    subtitleLabel.setJustificationType(juce::Justification::centred);
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colour(0, 255, 255));  // Cyan
    addAndMakeVisible(subtitleLabel);

    // Setup sliders
    auto setupSlider = [this](juce::Slider& slider, const juce::String& labelText, juce::Label& label)
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(slider);

        label.setText(labelText, juce::dontSendNotification);
        label.setFont(juce::Font(14.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        label.attachToComponent(&slider, false);
        addAndMakeVisible(label);
    };

    setupSlider(sizeSlider, "SIZE", sizeLabel);
    setupSlider(mixSlider, "MIX", mixLabel);
    setupSlider(predelaySlider, "PREDELAY", predelayLabel);
    setupSlider(lowCutSlider, "LOW CUT", lowCutLabel);
    setupSlider(highCutSlider, "HIGH CUT", highCutLabel);

    // Setup type selection box
    typeBox.addItem("HALL", 1);
    typeBox.addItem("ROOM", 2);
    typeBox.addItem("PLATE", 3);
    typeBox.addItem("SPACE", 4);
    typeBox.setJustificationType(juce::Justification::centred);
    typeBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    addAndMakeVisible(typeBox);

    typeLabel.setText("TYPE", juce::dontSendNotification);
    typeLabel.setFont(juce::Font(14.0f));
    typeLabel.setJustificationType(juce::Justification::centred);
    typeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    typeLabel.attachToComponent(&typeBox, false);
    addAndMakeVisible(typeLabel);

    // Setup parameter attachments
    auto& apvts = audioProcessor.getAPVTS();
    sizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "size", sizeSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "mix", mixSlider);
    predelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "predelay", predelaySlider);
    lowCutAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "lowcut", lowCutSlider);
    highCutAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "highcut", highCutSlider);
    typeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "type", typeBox);

    // Setup visualizer
    visualizer.setProcessor(&audioProcessor);
    addAndMakeVisible(visualizer);

    // Window size
    setSize (600, 500);
    startTimerHz(30);
}

IKReverbAudioProcessorEditor::~IKReverbAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    stopTimer();
}

void IKReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    drawBackground(g);
}

void IKReverbAudioProcessorEditor::drawCheckerboard(juce::Graphics& g, juce::Rectangle<int> area)
{
    const int squareSize = 10;
    const float alpha = 0.03f;

    for (int y = area.getY(); y < area.getBottom(); y += squareSize)
    {
        for (int x = area.getX(); x < area.getRight(); x += squareSize)
        {
            if ((x / squareSize + y / squareSize) % 2 == 0)
            {
                g.setColour(juce::Colours::white.withAlpha(alpha));
                g.fillRect(x, y, squareSize, squareSize);
            }
        }
    }
}

void IKReverbAudioProcessorEditor::drawBackground(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Main background gradient
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(40, 40, 40),
        0.0f, 0.0f,
        juce::Colour(20, 20, 20),
        0.0f, (float)bounds.getHeight(),
        false));
    g.fillAll();

    // Draw checkerboard pattern
    drawCheckerboard(g, bounds);

    // Draw grid lines
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    for (int x = 0; x < bounds.getWidth(); x += 20)
    {
        g.drawVerticalLine(x, 0.0f, (float)bounds.getHeight());
    }
    for (int y = 0; y < bounds.getHeight(); y += 20)
    {
        g.drawHorizontalLine(y, 0.0f, (float)bounds.getWidth());
    }

    // Draw border glow
    auto borderBounds = bounds.toFloat().reduced(2);
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(0, 255, 255).withAlpha(0.5f),  // Cyan
        borderBounds.getTopLeft(),
        juce::Colour(255, 0, 255).withAlpha(0.5f),  // Magenta
        borderBounds.getBottomRight(),
        true));
    g.drawRoundedRectangle(borderBounds, 5.0f, 2.0f);
}

void IKReverbAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    auto topSection = bounds.removeFromTop(80);

    // Title and subtitle
    titleLabel.setBounds(topSection.removeFromTop(30));
    subtitleLabel.setBounds(topSection.removeFromTop(30));

    // Visualizer
    visualizer.setBounds(bounds.removeFromTop(100));

    // Type selection box - smaller and at the top
    auto typeBoxBounds = bounds.removeFromTop(40);
    typeBox.setBounds(typeBoxBounds.reduced(typeBoxBounds.getWidth() / 3, 5));

    // Controls section - now in two rows
    auto controlsArea = bounds.reduced(10, 10);
    
    // Top row (Size, Mix, Predelay)
    auto topRow = controlsArea.removeFromTop(controlsArea.getHeight() / 2);
    auto sliderWidth = topRow.getWidth() / 3;
    
    sizeSlider.setBounds(topRow.removeFromLeft(sliderWidth).reduced(5));
    mixSlider.setBounds(topRow.removeFromLeft(sliderWidth).reduced(5));
    predelaySlider.setBounds(topRow.removeFromLeft(sliderWidth).reduced(5));

    // Bottom row (Low Cut, High Cut)
    auto bottomRow = controlsArea;
    sliderWidth = bottomRow.getWidth() / 2;
    
    lowCutSlider.setBounds(bottomRow.removeFromLeft(sliderWidth).reduced(5));
    highCutSlider.setBounds(bottomRow.removeFromLeft(sliderWidth).reduced(5));
}

void IKReverbAudioProcessorEditor::timerCallback()
{
    repaint();
}

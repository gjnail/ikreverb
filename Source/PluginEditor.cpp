/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
IKDistortionAudioProcessorEditor::IKDistortionAudioProcessorEditor (IKDistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), visualizer()
{
    setSize (400, 700);  // Made taller to accommodate visualizer
    
    // Set custom look and feel
    setLookAndFeel(&customLookAndFeel);
    
    // Add visualizer
    addAndMakeVisible(visualizer);
    visualizer.setProcessor(&audioProcessor);
    
    // Ghost Mode Button
    ghostModeButton.setButtonText("GHOST MODE");
    ghostModeButton.setClickingTogglesState(true);
    ghostModeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
    ghostModeButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkred);
    ghostModeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    ghostModeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::red);
    addAndMakeVisible(ghostModeButton);
    
    // Common slider setup
    auto setupSlider = [this](juce::Slider& slider, bool isSmall = false) {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, isSmall ? 60 : 90, 25);
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::cyan);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(slider);
    };
    
    // Common label setup
    auto setupLabel = [this](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font(juce::Font::FontStyleFlags::bold).withHeight(16.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::yellow);
        addAndMakeVisible(label);
    };
    
    // Setup all sliders
    setupSlider(driveSlider);
    setupSlider(outputSlider);
    setupSlider(toneSlider);
    setupSlider(mixSlider);
    
    // Setup destroy slider with custom look
    destroySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    destroySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 25);
    destroySlider.setLookAndFeel(&destroyLookAndFeel);
    addAndMakeVisible(destroySlider);
    
    // Setup all labels
    setupLabel(driveLabel, "DRIVE");
    setupLabel(outputLabel, "OUTPUT");
    setupLabel(toneLabel, "TONE");
    setupLabel(mixLabel, "MIX");
    setupLabel(typeLabel, "TYPE");
    
    // Setup destroy label
    destroyLabel.setText("DESTROY", juce::dontSendNotification);
    destroyLabel.setFont(juce::Font(juce::Font::FontStyleFlags::bold).withHeight(16.0f));
    destroyLabel.setJustificationType(juce::Justification::centred);
    destroyLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    addAndMakeVisible(destroyLabel);
    
    // Type selection setup
    typeBox.setColour(juce::ComboBox::textColourId, juce::Colours::cyan);
    typeBox.addItemList({"SOFT", "HARD", "TUBE", "FOLDBACK", "SINE"}, 1);
    typeBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(typeBox);
    
    // Create parameter attachments
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "drive", driveSlider);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "output", outputSlider);
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "tone", toneSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "mix", mixSlider);
    destroyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "destroy", destroySlider);
    typeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.apvts, "type", typeBox);
    ghostModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "ghostMode", ghostModeButton);
        
    startTimerHz(30); // For UI updates
}

IKDistortionAudioProcessorEditor::~IKDistortionAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    destroySlider.setLookAndFeel(nullptr);
}

void IKDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Get the current ghost mode state directly from the button
    bool isGhostMode = ghostModeButton.getToggleState();
    
    if (isGhostMode)
        drawGhostModeBackground(g);
    else
        drawNormalBackground(g);

    // Draw a border around the ghost mode button that glows when active
    if (isGhostMode)
    {
        auto buttonBounds = ghostModeButton.getBounds().toFloat();
        g.setColour(juce::Colours::red.withAlpha(0.5f));
        g.drawRoundedRectangle(buttonBounds.expanded(2.0f), 4.0f, 2.0f);
        g.setColour(juce::Colours::red.withAlpha(0.2f));
        g.drawRoundedRectangle(buttonBounds.expanded(4.0f), 4.0f, 2.0f);
    }
}

void IKDistortionAudioProcessorEditor::drawGhostModeBackground(juce::Graphics& g)
{
    // Base background (dark red/black gradient)
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(40, 0, 0), 0.0f, 0.0f,
        juce::Colour(10, 0, 0), 0.0f, (float)getHeight(),
        false));
    g.fillAll();
    
    // Add some eerie red elements
    g.setColour(juce::Colours::red.withAlpha(0.1f));
    for (int i = 0; i < getWidth(); i += 20)
    {
        g.drawLine(i, 0, i + 20, getHeight(), 1.0f);
    }
    
    // Add some static/noise effect
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    juce::Random random;
    for (int i = 0; i < 1000; i++)
    {
        float x = random.nextFloat() * getWidth();
        float y = random.nextFloat() * getHeight();
        g.fillRect(juce::Rectangle<float>(x, y, 1.0f, 1.0f));
    }
    
    // Title with ghost style
    auto titleArea = getLocalBounds().removeFromTop(100);
    
    // Title shadow
    g.setColour(juce::Colours::red.withAlpha(0.5f));
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawFittedText("INTERNET KIDS", titleArea.translated(2, 2).removeFromTop(50),
                     juce::Justification::centred, 1);
                     
    // Main title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawFittedText("INTERNET KIDS", titleArea.removeFromTop(50),
                     juce::Justification::centred, 1);
    
    // Subtitle
    g.setColour(juce::Colours::red);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawFittedText("GHOST MODE", titleArea,
                     juce::Justification::centred, 1);

    // Add some flickering elements
    if (random.nextFloat() > 0.7f)
    {
        g.setColour(juce::Colours::red.withAlpha(0.1f));
        g.fillRect(random.nextInt(getWidth()), random.nextInt(getHeight()), 
                  random.nextInt(50), random.nextInt(50));
    }
}

void IKDistortionAudioProcessorEditor::drawNormalBackground(juce::Graphics& g)
{
    // Base background (dark purple/blue gradient)
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(40, 0, 60), 0.0f, 0.0f,
        juce::Colour(20, 0, 40), 0.0f, (float)getHeight(),
        false));
    g.fillAll();
    
    // Draw checkerboard pattern
    drawCheckerboard(g, getLocalBounds());
    
    // Add some 90s style grid lines
    g.setColour(juce::Colours::cyan.withAlpha(0.1f));
    for (int i = 0; i < getWidth(); i += 20) {
        g.drawVerticalLine(i, 0.0f, (float)getHeight());
    }
    for (int i = 0; i < getHeight(); i += 20) {
        g.drawHorizontalLine(i, 0.0f, (float)getWidth());
    }
    
    // Title with 90s style
    auto titleArea = getLocalBounds().removeFromTop(100);
    
    // Title shadow
    g.setColour(juce::Colours::magenta.withAlpha(0.5f));
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawFittedText("INTERNET KIDS", titleArea.translated(2, 2).removeFromTop(50),
                     juce::Justification::centred, 1);
                     
    // Main title
    g.setColour(juce::Colours::cyan);
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawFittedText("INTERNET KIDS", titleArea.removeFromTop(50),
                     juce::Justification::centred, 1);
    
    // Subtitle
    g.setColour(juce::Colours::magenta);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawFittedText("DISTORTION", titleArea,
                     juce::Justification::centred, 1);
}

void IKDistortionAudioProcessorEditor::drawCheckerboard(juce::Graphics& g, juce::Rectangle<int> area)
{
    int squareSize = 20;
    bool isWhite = false;
    
    for (int y = area.getY(); y < area.getBottom(); y += squareSize)
    {
        isWhite = !isWhite;  // Alternate starting color for each row
        for (int x = area.getX(); x < area.getRight(); x += squareSize)
        {
            g.setColour(isWhite ? juce::Colours::white.withAlpha(0.03f) 
                               : juce::Colours::black.withAlpha(0.2f));
            g.fillRect(x, y, squareSize, squareSize);
            isWhite = !isWhite;
        }
    }
}

void IKDistortionAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(100); // Space for title
    
    // Ghost mode button at the top
    ghostModeButton.setBounds(area.removeFromTop(30).reduced(5));
    
    // Visualizer below ghost mode button
    auto visArea = area.removeFromTop(120).reduced(20, 10);
    visualizer.setBounds(visArea);
    
    // Type selection below visualizer
    auto typeArea = area.removeFromTop(60).reduced(20, 0);
    typeLabel.setBounds(typeArea.removeFromTop(20));
    typeBox.setBounds(typeArea);
    
    // Rest of the layout stays the same...
    auto sliderArea = area.reduced(30);
    auto sliderWidth = sliderArea.getWidth() / 2;
    auto sliderHeight = sliderArea.getHeight() / 3;
    
    // Top row
    auto topRow = sliderArea.removeFromTop(sliderHeight);
    
    // Drive
    auto driveArea = topRow.removeFromLeft(sliderWidth);
    driveLabel.setBounds(driveArea.removeFromTop(20));
    driveSlider.setBounds(driveArea.reduced(10));
    
    // Output
    outputLabel.setBounds(topRow.removeFromTop(20));
    outputSlider.setBounds(topRow.reduced(10));
    
    // Middle row
    auto middleRow = sliderArea.removeFromTop(sliderHeight);
    
    // Tone
    auto toneArea = middleRow.removeFromLeft(sliderWidth);
    toneLabel.setBounds(toneArea.removeFromTop(20));
    toneSlider.setBounds(toneArea.reduced(10));
    
    // Mix
    mixLabel.setBounds(middleRow.removeFromTop(20));
    mixSlider.setBounds(middleRow.reduced(10));
    
    // Bottom row (Destroy only)
    auto bottomRow = sliderArea;
    
    // Center the destroy section
    auto destroySection = bottomRow.reduced(sliderWidth * 0.25f, 0);  // Center by reducing sides
    
    // Destroy (large, centered)
    destroyLabel.setBounds(destroySection.removeFromTop(20));
    destroySlider.setBounds(destroySection.reduced(5));
}

void IKDistortionAudioProcessorEditor::timerCallback()
{
    repaint(); // Update the UI regularly for ghost mode effects
}

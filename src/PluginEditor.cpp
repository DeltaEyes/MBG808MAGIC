#include "PluginProcessor.h"
#include "PluginEditor.h"

// FL Studioルック（ホワイト・モノトーン仕様）
class FLStudioLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FLStudioLookAndFeel()
    {
        // ★ 数値テキストボックスの背景を少し濃い半透明の黒にして読みやすくする
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xcc101010));
        setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0xff4A525C));
    }
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 8.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        juce::Path track;
        track.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xff14171A));
        g.strokePath(track, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // ★ ツマミの現在値を「白」に変更
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(juce::Colours::white);
        g.strokePath(valueArc, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        float innerRadius = radius - 7.0f;
        g.setColour(juce::Colour(0xff2F353C));
        g.fillEllipse(centreX - innerRadius, centreY - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);
        g.setColour(juce::Colour(0xff1A1D21));
        g.drawEllipse(centreX - innerRadius, centreY - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.5f);

        juce::Path pointer;
        pointer.startNewSubPath(centreX + 4.0f * std::sin(angle), centreY - 4.0f * std::cos(angle));
        pointer.lineTo(centreX + (innerRadius - 2.0f) * std::sin(angle), centreY - (innerRadius - 2.0f) * std::cos(angle));
        g.setColour(juce::Colours::white);
        g.strokePath(pointer, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    juce::Font getLabelFont(juce::Label &) override { return juce::FontOptions(12.0f, juce::Font::bold); }
};

static FLStudioLookAndFeel flLookAndFeel;

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    knobBgImage = juce::ImageCache::getFromMemory(BinaryData::knob_bg_jpg, BinaryData::knob_bg_jpgSize);

    auto repaintGraph = [this]
    { repaint(); };
    setLookAndFeel(&flLookAndFeel);

    freqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 20);
    freqSlider.onValueChange = repaintGraph;
    addAndMakeVisible(freqSlider);
    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ", freqSlider);
    freqLabel.setText("FREQ", juce::dontSendNotification);
    freqLabel.setJustificationType(juce::Justification::centred);
    freqLabel.setColour(juce::Label::textColourId, juce::Colours::white); // ★ ラベルを白に変更
    addAndMakeVisible(freqLabel);

    intensitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    intensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 20);
    intensitySlider.onValueChange = repaintGraph;
    addAndMakeVisible(intensitySlider);
    intensityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INTENSITY", intensitySlider);
    intensityLabel.setText("INT", juce::dontSendNotification);
    intensityLabel.setJustificationType(juce::Justification::centred);
    intensityLabel.setColour(juce::Label::textColourId, juce::Colours::white); // ★ ラベルを白に変更
    addAndMakeVisible(intensityLabel);

    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 20);
    driveSlider.onValueChange = repaintGraph;
    addAndMakeVisible(driveSlider);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);
    driveLabel.setText("DRIVE", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);
    driveLabel.setColour(juce::Label::textColourId, juce::Colours::white); // ★ ラベルを白に変更
    addAndMakeVisible(driveLabel);

    setSize(560, 390);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() { setLookAndFeel(nullptr); }

void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff23282C));
    juce::Rectangle<float> headerArea(0.0f, 0.0f, (float)getWidth(), 35.0f);
    g.setColour(juce::Colour(0xff363D43));
    g.fillRect(headerArea);
    g.setColour(juce::Colour(0xff15181A));
    g.drawHorizontalLine(35, 0.0f, (float)getWidth());

    // ★ ヘッダーのアイコンを白に変更
    g.setColour(juce::Colours::white);
    g.fillEllipse(12.0f, 10.0f, 15.0f, 15.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    g.drawText("MBG 808 MAGIC", 38, 0, 200, 35, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff8F959D));
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("64BIT / VST3", getWidth() - 100, 0, 85, 35, juce::Justification::centredRight);

    juce::Rectangle<float> graphArea(15.0f, 50.0f, 530.0f, 150.0f);
    g.setColour(juce::Colour(0xff16191D));
    g.fillRect(graphArea);
    g.setColour(juce::Colour(0xff3D444A));
    g.drawRect(graphArea, 1.0f);
    g.setColour(juce::Colour(0xff262B30));
    for (int i = 1; i < 10; ++i)
        g.drawVerticalLine((int)(graphArea.getX() + (graphArea.getWidth() / 10.0f) * (float)i), graphArea.getY(), graphArea.getBottom());
    for (int i = 1; i < 4; ++i)
        g.drawHorizontalLine((int)(graphArea.getY() + (graphArea.getHeight() / 4.0f) * (float)i), graphArea.getX(), graphArea.getRight());

    float freq = (float)freqSlider.getValue();
    float intensity = (float)intensitySlider.getValue();
    float drive = (float)driveSlider.getValue();
    float freqX = juce::jmap(freq, 32.0f, 256.0f, graphArea.getX() + 30.0f, graphArea.getRight() - 100.0f);
    g.setColour(juce::Colour(0xff656C75));
    g.drawVerticalLine((int)freqX, graphArea.getY(), graphArea.getBottom());
    g.setColour(juce::Colours::white);
    g.fillEllipse(freqX - 4.5f, graphArea.getBottom() - 25.0f, 9.0f, 9.0f);

    juce::Path harmonicCurve;
    harmonicCurve.startNewSubPath(freqX, graphArea.getBottom());
    float peakX = freqX + 60.0f + (drive * 8.0f);
    float peakY = juce::jmap(intensity, -24.0f, 24.0f, graphArea.getBottom(), graphArea.getY() + 10.0f);
    harmonicCurve.quadraticTo(peakX, peakY - (drive * 1.5f), graphArea.getRight() - 10.0f, graphArea.getBottom());

    // ★ グラフのカーブを白のグラデーションに変更
    juce::ColourGradient fillGrad(juce::Colours::white.withAlpha(0.45f), 0.0f, peakY, juce::Colours::white.withAlpha(0.0f), 0.0f, graphArea.getBottom(), false);
    g.setGradientFill(fillGrad);
    g.fillPath(harmonicCurve);
    g.setColour(juce::Colours::white);
    g.strokePath(harmonicCurve, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // =======================================================
    // ★ 画像背景と視認性アップのためのダーク・ティント
    // =======================================================
    juce::Rectangle<float> controlArea(15.0f, 215.0f, 530.0f, 160.0f);
    if (knobBgImage.isValid())
    {
        g.drawImage(knobBgImage, controlArea, juce::RectanglePlacement::stretchToFit);

        // 画像の上に半透明の黒（40%）を重ねて、白いUIパーツを際立たせる
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillRect(controlArea);
    }
    else
    {
        g.setColour(juce::Colour(0xff1C2024));
        g.fillRect(controlArea);
    }
    g.setColour(juce::Colour(0xff3D444A));
    g.drawRect(controlArea, 1.0f);
}

void AudioPluginAudioProcessorEditor::resized()
{
    int sliderWidth = 100;
    int sliderHeight = 100;
    int yPos = 240;
    int spacing = 58;
    int startX = 15 + spacing;

    freqSlider.setBounds(startX, yPos, sliderWidth, sliderHeight);
    freqLabel.setBounds(startX, yPos - 20, sliderWidth, 20);
    startX += sliderWidth + spacing;
    intensitySlider.setBounds(startX, yPos, sliderWidth, sliderHeight);
    intensityLabel.setBounds(startX, yPos - 20, sliderWidth, 20);
    startX += sliderWidth + spacing;
    driveSlider.setBounds(startX, yPos, sliderWidth, sliderHeight);
    driveLabel.setBounds(startX, yPos - 20, sliderWidth, 20);
}
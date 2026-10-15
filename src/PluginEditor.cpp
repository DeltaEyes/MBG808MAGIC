#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // ツマミの見た目を設定（丸いツマミで、下に数値を表示）
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

    // 画面にツマミを追加
    addAndMakeVisible(driveSlider);

    // ツマミとAPVTSの "DRIVE" パラメータを接続する
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DRIVE", driveSlider);

    // 画面サイズを設定
    setSize(400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    // 背景色を真っ黒に
    g.fillAll(juce::Colours::black);

    // 上のほうにタイトルを描画
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("MBG 808 MAGIC", 0, 10, getWidth(), 30, juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // ツマミを画面の真ん中に、縦横150pxのサイズで配置
    driveSlider.setBounds(getWidth() / 2 - 75, getHeight() / 2 - 75, 150, 150);
}
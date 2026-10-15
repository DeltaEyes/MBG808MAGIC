#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // =======================================================
    // ★ ツマミの変更を検知して、画面（グラフ）を再描画する設定
    // =======================================================
    auto repaintGraph = [this]
    { repaint(); };

    // FREQ
    freqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    freqSlider.onValueChange = repaintGraph; // 回したら再描画！
    addAndMakeVisible(freqSlider);
    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ", freqSlider);

    freqLabel.setText("Freq (Hz)", juce::dontSendNotification);
    freqLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(freqLabel);

    // INTENSITY
    intensitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    intensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    intensitySlider.onValueChange = repaintGraph; // 回したら再描画！
    addAndMakeVisible(intensitySlider);
    intensityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INTENSITY", intensitySlider);

    intensityLabel.setText("Intensity", juce::dontSendNotification);
    intensityLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(intensityLabel);

    // DRIVE
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    driveSlider.onValueChange = repaintGraph; // 回したら再描画！
    addAndMakeVisible(driveSlider);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);

    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(driveLabel);

    // 画面サイズを少し縦に広げる（グラフのスペースを作るため）
    setSize(600, 380);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

// =======================================================
// ★ 画面の描画（グラフを描く魔法はここ！）
// =======================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    // 背景
    g.fillAll(juce::Colour(0xff181818)); // 少しだけ明るい黒

    // タイトル
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("MBG 808 MAGIC", 0, 15, getWidth(), 30, juce::Justification::centred, 1);

    // --- ここからグラフ描画 ---
    juce::Rectangle<float> graphArea(50.0f, 60.0f, 500.0f, 120.0f);

    // グラフの背景ボックス
    g.setColour(juce::Colour(0xff0a0a0a));
    g.fillRoundedRectangle(graphArea, 8.0f);
    g.setColour(juce::Colour(0xff333333));
    g.drawRoundedRectangle(graphArea, 8.0f, 2.0f);

    // 現在のツマミの値を取得
    float freq = (float)freqSlider.getValue();
    float intensity = (float)intensitySlider.getValue();
    float drive = (float)driveSlider.getValue();

    // 周波数をグラフのX座標に変換（32Hz〜256Hz を 左端〜中央付近 にマッピング）
    float freqX = juce::jmap(freq, 32.0f, 256.0f, graphArea.getX() + 20.0f, graphArea.getX() + 250.0f);

    // 1. ターゲット周波数（カットされる基音）をグレーの点線で描画
    g.setColour(juce::Colours::grey);
    float dashPattern[2] = {4.0f, 4.0f};
    g.drawDashedLine(juce::Line<float>(freqX, graphArea.getBottom(), freqX, graphArea.getY()), dashPattern, 2, 1.0f);

    g.setFont(12.0f);
    g.drawText("Target", (int)freqX - 25, (int)graphArea.getBottom() - 20, 50, 20, juce::Justification::centred);

    // 2. 倍音の山（オレンジ色のカーブ）を描画
    juce::Path harmonicsCurve;
    harmonicsCurve.startNewSubPath(freqX, graphArea.getBottom()); // ターゲット周波数から山がスタート

    // 山の頂点の位置を計算
    float peakX = freqX + 60.0f + (drive * 8.0f); // ドライブを上げると倍音が広域まで伸びる
    float peakY = juce::jmap(intensity, -24.0f, 24.0f, graphArea.getBottom(), graphArea.getY() + 10.0f);

    // なめらかなカーブを作成
    harmonicsCurve.quadraticTo(peakX, peakY - (drive * 2.0f), graphArea.getRight() - 20.0f, graphArea.getBottom());

    // カーブの中身を半透明のオレンジで塗りつぶす
    g.setColour(juce::Colours::orange.withAlpha(0.4f));
    g.fillPath(harmonicsCurve);

    // カーブの輪郭線を濃いオレンジで描く
    g.setColour(juce::Colours::orange);
    g.strokePath(harmonicsCurve, juce::PathStrokeType(2.5f));
}

// =======================================================
// ★ ツマミの配置場所
// =======================================================
void AudioPluginAudioProcessorEditor::resized()
{
    int sliderWidth = 120;
    int sliderHeight = 120;
    int yPos = 210; // グラフの下になるように位置を下げました

    // 左：FREQ
    freqSlider.setBounds(100, yPos, sliderWidth, sliderHeight);
    freqLabel.setBounds(100, yPos - 25, sliderWidth, 20);

    // 中：INTENSITY
    intensitySlider.setBounds(240, yPos, sliderWidth, sliderHeight);
    intensityLabel.setBounds(240, yPos - 25, sliderWidth, 20);

    // 右：DRIVE
    driveSlider.setBounds(380, yPos, sliderWidth, sliderHeight);
    driveLabel.setBounds(380, yPos - 25, sliderWidth, 20);
}
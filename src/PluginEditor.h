#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    AudioPluginAudioProcessor &audioProcessor;

    // 3つのツマミ
    juce::Slider freqSlider;
    juce::Slider intensitySlider;
    juce::Slider driveSlider;

    // 3つのラベル（文字）
    juce::Label freqLabel;
    juce::Label intensityLabel;
    juce::Label driveLabel;

    // APVTSとの紐付け
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intensityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
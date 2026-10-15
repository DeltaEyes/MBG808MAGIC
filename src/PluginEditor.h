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

    juce::Image knobBgImage; // ★ 埋め込み画像を保持する変数

    juce::Slider freqSlider;
    juce::Slider intensitySlider;
    juce::Slider driveSlider;
    juce::Label freqLabel;
    juce::Label intensityLabel;
    juce::Label driveLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intensityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
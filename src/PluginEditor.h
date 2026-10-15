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

    // ツマミ本体
    juce::Slider driveSlider;

    // ツマミとProcessorのAPVTSを繋ぐ紐
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
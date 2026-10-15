#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      // apvtsの初期化 ("Parameters" というツリーを作ります)
      apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

// パラメータの定義
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // "DRIVE" パラメータの登録
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("DRIVE", 1), // IDとバージョン
        "Drive",                       // 表示名
        1.0f,                          // 最小値
        10.0f,                         // 最大値
        5.0f                           // 初期値
        ));

    return layout;
}

const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
}

void AudioPluginAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

// 音声処理の心臓部
void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // APVTSから最新の "DRIVE" の値を取得する
    float drive = apvts.getRawParameterValue("DRIVE")->load();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto *channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float in = channelData[sample];

            // tanh関数によるサチュレーション（歪み）処理
            float out = std::tanh(in * drive) / std::tanh(drive);

            channelData[sample] = out;
        }
    }
}

bool AudioPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
}

void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
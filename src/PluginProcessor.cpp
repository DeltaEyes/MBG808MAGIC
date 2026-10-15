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
      apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FREQ", 1), "Freq (Hz)", 32.0f, 256.0f, 80.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("INTENSITY", 1), "Intensity (dB)", -24.0f, 24.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DRIVE", 1), "Drive", 1.0f, 10.0f, 5.0f));
    return layout;
}

const juce::String AudioPluginAudioProcessor::getName() const { return JucePlugin_Name; }
bool AudioPluginAudioProcessor::acceptsMidi() const { return false; }
bool AudioPluginAudioProcessor::producesMidi() const { return false; }
bool AudioPluginAudioProcessor::isMidiEffect() const { return false; }
double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int AudioPluginAudioProcessor::getNumPrograms() { return 1; }
int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }
void AudioPluginAudioProcessor::setCurrentProgram(int index) {}
const juce::String AudioPluginAudioProcessor::getProgramName(int index) { return {}; }
void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String &newName) {}

void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;

    lowPassFilterL.prepare(spec);
    lowPassFilterL.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    lowPassFilterR.prepare(spec);
    lowPassFilterR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    highPassFilterL.prepare(spec);
    highPassFilterL.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highPassFilterR.prepare(spec);
    highPassFilterR.setType(juce::dsp::StateVariableTPTFilterType::highpass);
}

void AudioPluginAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}
#endif

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float freq = apvts.getRawParameterValue("FREQ")->load();
    float intensityDB = apvts.getRawParameterValue("INTENSITY")->load();
    float drive = apvts.getRawParameterValue("DRIVE")->load();
    float intensity = juce::Decibels::decibelsToGain(intensityDB);

    lowPassFilterL.setCutoffFrequency(freq);
    lowPassFilterR.setCutoffFrequency(freq);
    highPassFilterL.setCutoffFrequency(freq * 1.2f);
    highPassFilterR.setCutoffFrequency(freq * 1.2f);

    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
    {
        auto *channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float in = channelData[sample];
            float low = 0.0f;
            float harmonicsOnly = 0.0f;
            if (channel == 0)
            {
                low = lowPassFilterL.processSample(0, in);
                float distortedLow = std::tanh(low * drive) + (low * low * drive * 0.2f);
                harmonicsOnly = highPassFilterL.processSample(0, distortedLow);
            }
            else if (channel == 1)
            {
                low = lowPassFilterR.processSample(0, in);
                float distortedLow = std::tanh(low * drive) + (low * low * drive * 0.2f);
                harmonicsOnly = highPassFilterR.processSample(0, distortedLow);
            }
            channelData[sample] = in + (harmonicsOnly * intensity);
        }
    }
}

bool AudioPluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor() { return new AudioPluginAudioProcessorEditor(*this); }
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {}
void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {}
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new AudioPluginAudioProcessor(); }
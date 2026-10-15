cmake_minimum_required(VERSION 3.20)

# プロジェクト名とバージョン
project(MBG808MAGIC VERSION 1.0.0)

# サブモジュールとして追加したJUCEを読み込む
add_subdirectory(JUCE)

# プラグインの基本設定
juce_add_plugin(MBG808MAGIC
    COMPANY_NAME "MottainaiBananaGang"
    PLUGIN_MANUFACTURER_CODE "MBG_"
    PLUGIN_CODE "808M"
    FORMATS VST3 Standalone
    PRODUCT_NAME "MBG 808 MAGIC"
    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_add_binary_data(MBG808MAGIC_Resources
    SOURCES
        assets/knob_bg.jpg
)

juce_generate_juce_header(MBG808MAGIC)

# コンパイルするソースコードを指定
target_sources(MBG808MAGIC
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
)

# 必要なJUCEのモジュールをリンクする
target_link_libraries(MBG808MAGIC
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_plugin_client
        juce::juce_dsp
        MBG808MAGIC_Resources
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

# =========================================================================
# エラー回避: VST2との互換性を無効化する
# (C1189 error: You may have a conflict with parameter automation... の対策)
# =========================================================================
target_compile_definitions(MBG808MAGIC
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)
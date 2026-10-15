# MBG 808 MAGIC

**808の音をマッジで太くする**  
サブベースや808キックの重低音の存在感を小型スピーカーやスマホ再生時にも圧倒的にクリアに引き立てる、MBGお手製のプラグインや。

超低域（基音）そのものを無理に持ち上げるのではなく、人間の聴覚特性（錯覚）を利用して音の「倍音（Harmonics）」を正確に生成・付加することで、ミックスのヘッドルーム（音量的な余裕）を消費することなく、タイトで太い重低音感を実現するんや。

---


## 動作環境 & 技術スペック 

* **フォーマット**: VST3 (64-bit)
* **対応アーキテクチャ**: x86_64 / Windows 10, 11
* **開発フレームワーク**: JUCE (C++20)
* **ビルドシステム**: CMake 3.22+ / MSBuild (Visual Studio 2022)
* **インストールパス**: `C:\Program Files\Common Files\VST3\MBG 808 MAGIC.vst3`

---

## 使用方法
MBG808MAGIC.zipをダウンロードし、解凍後フォルダ内部のMBG808MAGIC.vst3をお使いの環境の指定のVST3フォルダにコピーしてつかうんや。

## 【開発者向け】ビルド方法

### 事前準備
* Visual Studio 2022 (C++によるデスクトップ開発ワークスペース)
* CMake 3.22 以降
* JUCE 8 モジュール

### ビルド手順
```bash
# 1. リポジトリのクローン
git clone [https://github.com/your-account/MBG808MAGIC.git](https://github.com/your-account/MBG808MAGIC.git)
cd MBG808MAGIC

# 2. CMakeの設定 (Configure)
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 3. ビルド実行 (Build)
cmake --build build --config Release

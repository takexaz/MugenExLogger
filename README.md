## Mugen Extra Logger
### 概要
MUGENの試合中に追加のログを出力します。

### 必要環境

* WinMUGEN（約728KB）
* ASI Loader

※このプロジェクトは **WinMUGEN（約728KBのもの）専用** の拡張モジュールです。
他バージョンでは動作しません。

### セットアップ手順

1.  ASI Loaderを導入

   Ultimate-ASI-Loaderの[Release](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases)から **ddraw.dll** をダウンロードし、`winmugen.exe` と同じフォルダに配置してください。

2. プラグインの配置

   `plugins`フォルダを作成し、 **ExLogger.asi** を配置してください。

3. 出力

   ログは `logs` フォルダに出力されます。存在しない場合は自動で生成されます。


**構成図**
```
winmugen/
├─ winmugen.exe
├─ ddraw.dll
├─ logs/
└─ plugins/
　　└─ ExLogger.asi
```

### ライセンス

このプロジェクトはMIT Licenseですが、一部にBSL-1.0 Lisenceのコードを含みます。

### 使用ライブラリ

本プロジェクトでは以下のライブラリを使用しています：

* spdlog （MIT License）

  [https://github.com/gabime/spdlog](https://github.com/gabime/spdlog)

* safetyhook （BSL-1.0 Lisence）

  [https://github.com/cursey/safetyhook](https://github.com/cursey/safetyhook)

* Ultimate-ASI-Loader （MIT License）

  [https://github.com/ThirteenAG/Ultimate-ASI-Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader)

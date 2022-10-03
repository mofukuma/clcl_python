# clcl_python Ver 1.0

## About
CLCL (https://github.com/nakkag/CLCL) をPythonで拡張できるプラグインツールです。

Pythonが書ける人ならCLCLをコピペやクリックから何でもできるツールへ進化させます。

PythonだとCより文字列処理が簡単ですし、簡単にインターネットにアクセスできます。以下のようなことも簡単にできるでしょう。

- 自動翻訳（DeepL API）　※sc1.pyのサンプル
- LINEで発言
- discordに流す
- URLの画像をサーバの保管庫に保存
- Stable Diffusionに命令発信

などなど。
埋め込みPythonなのでパフォーマンスにも優れています。

## インストール
1. CLCLと同じディレクトリにzipの中身を解凍してすべてコピーする。
2. CLCLのオプション画面を開き「ツール」タブを表示。
3. 追加ボタンを押して clcl_python.dll を指定すると追加するツールを選択できます。
4. オプションの「ツール」タブを開いてそこに clcl_python.dll をドラッグ＆ドロップすると複数まとめて登録できます。
5. プロパティで実行するPythonスクリプト、実行する関数名を指定できる。
   デフォルトはPython実行１だとsc1.pyのmain関数が実行される。Python実行2はsc2.pyのmain。
   実行時、main関数の引数にはクリップボードの中身が入り、mainの返り値がCLCLに渡される。
6. メニューに「ツール」を追加してPython実行。

バージョンアップする場合は、CLCLを終了して clcl_python.dll を上書きしてください。
アンインストールする場合は、CLCLからツールを削除してから clcl_python.dll を削除してください。

インストールするときに clcl_python.dll が見当たらない場合は、DLL を表示しない設定になっている可能性があります。
エクスプローラのフォルダ オプションで「すべてのファイルとフォルダを表示する」設定にしてください。

## pip
pythonフォルダの中がEmbeddable Pythonになっています。
このPythonからpipモジュールを起動すればモジュール入れられると思います。

## コンパイル
VS2017でXP向けのコンパイル環境をダウンロードしてください。
コンパイルには公式Python 3.10のincludeおよびLibsも必要。

## ライセンス
　Python Embeddableのライセンス　LICENSE.txt 参照


## CLCLのクレジット
Copyright (C) 1996-2015 by Ohno Tomoaki. All rights reserved.
	http://www.nakka.com/


-------------------------


# clcl_python Ver 1.0

## About
This is a plugin tool to extend CLCL(https://github.com/nakkag/CLCL) with Python.
If you can write Python, it will evolve CLCL into a tool that can do anything from copy and paste to click.

Python makes string processing easier than C, and you can access the Internet easily.
You could easily do things like

- Automatic translation (DeepL API) *sc1.py
- Speak on LINE Chat
- Send to discord
- Store URL images in your server
- Send order to Stable Diffusion

and so on.
Embedded Python for superior performance.

## Installation
1. Unzip and copy the entire contents of the zip file to the same directory as CLCL.
2. Open the options screen of CLCL and display the "Tools" tab.
3. Click the "Add" button and specify clcl_python.dll to select the tool to be added.
4. Open the "Tools" tab of the options screen and drag & drop clcl_python.dll to register multiple tools at once.
5. You can specify the Python script to be executed and the function name to be executed in the properties.
　　The default is sc1.py's main function for Python run 1, sc2.py's main for Python run 2, and sc3.py's main function for Python run 3.
　　At runtime, the argument of the main function is the contents of the clipboard, and the return value of main is passed to CLCL.
6. add "Tools" to the menu and execute Python.

To upgrade, exit CLCL and overwrite clcl_python.dll.
To uninstall, remove the tool from CLCL and then delete clcl_python.dll.

If you do not see clcl_python.dll when you install, you may have the DLL set to not display.
Set "Show all files and folders" in the Explorer folder options.

## pip
The python folder contains Embeddable Python.
If you run the pip module from this Python folder, you should be able to install the module.


## License
　Python Embeddable license See LICENSE.txt


CLCL Credits
Copyright (C) 1996-2015 by Ohno Tomoaki. All rights reserved.
http://www.nakka.com/




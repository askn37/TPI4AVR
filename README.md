# TPI4AVR (Rev.1)

TPI (Tiny Programing Interface) host firmware over serial\
For ATtiny4/5/9/10 series from using avrdude, Arduino IDE\
/// High-Voltage programing FUSE fixed supported (experimental : hardware support required) ///

> TPI対応ATtiny系列のためのシリアル通信ホストファームウェア\
> ATtiny4/5/7/10系列を対象に avrdude と Arduino IDEで応用可能\
> /// 高電圧プログラミングによる FUSE 書換対応（実験的：要ハードウェア支援） ///

## 特徴

- TPI方式AVRのための、プログラミングホストファームウェア
  - ターゲット側は ATtiny4/5/9/10 系列とその上位品種専用
  - ホスト側は megaAVR-0系列、tinyAVR-2系列、AVR Dx系列専用
- Arduino IDE および avrdude コマンドラインから使用可能
  - JTAGmkIIプロトコル準拠（JTAG2UPDI に同等）
  - ホスト/ターゲット間は、対象系列チップ固有のハードウェアUART支援による高速伝送
  - ホスト/PC間は最大 3M bps 対応（要 avrdude 7.0、ホスト側F_CPU=24Mhz以上）
  - デバッガ機能なし（メモリプログラミング＆FUSE書換専用）
- 高電圧プログラミングに実験的対応（要ハードウェア支援）

### TPI4AVR hardware

[<img src="https://askn37.github.io/product/TPI4AVR/2222_Zinnia-TPI4AVRF-MZU2217B/Zinnia-TPI4AVRF-MZU2217B_top.svg" width="160">](https://askn37.github.io/product/TPI4AVR/2222_Zinnia-TPI4AVRF-MZU2217B/Zinnia-TPI4AVRF-MZU2217B_top.svg)

[[ハードウェアとしての TPI4AVR の紹介はこちら]](https://askn37.github.io/product/TPI4AVR)

## Arduino IDE への導入

[askn37 / Multix Zinnia Product SDK](https://askn37.github.io/)をインストール済の場合、
このライブラリは既に用意されている。

- `ファイル` -> `スケッチ例` -> `UPDI4AVR` を選ぶ\
  重要！__ビルド可能なボード選択をしていなければ、メニューにこの選択肢は表示されない__

そうでなければ次のようにする。

1. .ZIPアーカイブをダウンロードする。[Click here](https://github.com/askn37/UPDI4AVR/archive/master.zip)
1. ライブラリマネージャで読み込む\
  `スケッチ` -> `ライブラリをインクルード` -> `.ZIP形式のライブラリをインストール...`
1. ツールメニューのボード選択で、UPDIホストにする 適切なターゲットを選ぶ（次節）
1. `ファイル` -> `スケッチ例` -> `UPDI4AVR` を選ぶ\
  重要！__ビルド可能なボード選択をしていなければ、メニューにこの選択肢は表示されない__

## ライタホストに選択可能なボード種別

このファームウェアは次の要件を満たす AVR にアップロード（インストール）できる。

megaAVR-0 シリーズ、tinyAVR-2 シリーズ、AVR DA/DB シリーズのうち、

- 8KB 以上の FLASH 領域
- 1KB 以上の SRAM 容量
- 2組の ハードウェアUSART サポート

を有していること。

> __リファレンス AVR は ATtiny824 とその上位品種である。__

その他にこのファームウェアは 以下の製品でインストールできる。

[Arduiono / Arduino megaAVR Boards](https://docs.arduino.cc/software/ide-v1/tutorials/getting-started/cores/arduino-megaavr)

- Arduino UNO WiFi Rev 2 (ATmega4809)
- Arduino Nano Every (ATmega4809) -- __注意事項有り。（後述）__

[askn37 / Multix Zinnia Product SDK](https://askn37.github.io/)
-- megaAVR-0, tinyAVR-0/1/2, AVR DA/DB/DD series support

> デフォルトSDK。HV書込対応。

- megaAVR-0 / tinyAVR-2 / AVR DA/DB
- Microchip Curiosity Nano ATmega4809 (DM320115)
- Microchip Curiosity Nano ATtiny1627 (DM080104)
- Microchip Curiosity Nano AVR128DA48 (DM164151)
- Microchip Curiosity Nano AVR128DB48 (EV35L43A)

> Microchip Curiosity Nano 系列製品中、対象となるのは上記4種。

[MCUdude / MegaCoreX](https://github.com/MCUdude/MegaCoreX)
-- megaAVR-0 series support

- megaAVR-0

[SpenceKonde / megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)
-- tinyAVR-0,1,2 series support

- tinyAVR-2

[SpenceKonde / DxCore](https://github.com/SpenceKonde/DxCore)
-- AVR DA,DB,DD series support

- AVR DA/DB

`Arduino megaAVR Borads` はボードマネージャーから追加インストールする。\
その他はそれぞれのサポートサイトの指示に従って、インストールする。

サブメニューで主クロック設定が可能な場合は、\
16Mhz、20Mhz、24Mhz (オーバークロック時 28Mhz、32Mhz) から選択する。

### Arduino Nano Every について

オンボード搭載プログラムライタチップが JTAG2UPDI over UART 下位互換の特殊品である。
UPDI4AVR を書き込んでも制御が横取りされるために期待した動作をしないことが多い。

> オンボードUSB を使わずに追加の UART を JTAG通信用に別途用意し、DTR/RTS信号も配線すれば良いが
ほぼ同じ部材で SerialUPDI 書込器を作れてしまうので、それだと価値がない。

## TPIターゲットに選択可能なAVR品種

- ATtiny4/5/9/10
- ATtiny20/40
- ATtiny102/104

> この8品種しか TPI方式対応型番は存在しないようだ。

## Arudiuino IDE 上への TPI開発環境構築

[Multix Zinnia Product SDK [reduceAVR]](https://github.com/askn37/multix-zinnia-sdk-reduceAVR)
の導入が最も簡易。

- 元々 TPI4AVR はこのSDKから使うために設計されているので。
- 開発に必要なバージョンの AVR-GCC AVR-LIBC avrdude toolkit が簡単に揃う。

## 基本的な使い方

もっとも簡単な ターゲット 〜 TPI4AVR 間配線は、VCC、GND、TCLK、TDAT、TRST の5本を配線する。\
6pin品種の場合、4番目以外の全部が必要である。

```plain

Target tiny6pin    TPI4AVR         Host PC
    +------+     +----------+     +------+
    | PB0:1|<--->|TDAT      |     |      |
    | GND:2|---->|GND   JTTX|---->|RX    |
    | PB1:3|<----|TCLK      |     |      |
    | PB2:4|-x   |          |     |      |
    | VCC:5|<----|VCC   JTRX|<----|TX    |
    | PB3:6|<----|TRST      |     |      |
    +------+     +----------+     +------+

Fig.1 Programing Bridge Wireing
```

その他のピン接続は
[設定ファイル](examples/TPI4AVR/configuration.h)
を参照のこと

TPI4AVRは、*avrdude* からは __JTAG2UPDI 互換の UPDI書込器__ のように見える。
これに対応した設定は標準の設定ファイルに含まれていないため、
__[添付の設定ファイル](appendix/avrdude.conf.tinyrc)を必ず指定__ しなければならない。

```sh
avrdude -v -p attiny10 -c tpi4avr \
  -C avrdude.conf.tinyrc \
  -P <SerialPort> \
  -e -U flash:w:Blink.ino.hex:i
```

> [添付のパッチ](appendix/avrdude_with_TPI4AVR.patch)を *avrdude 7.0* のコードに当てれば
デフォルトの設定ファイルでも扱えるようにできる。

## HV書込でFUSEを初期化するには

次項の外付回路を追加している場合は、*avrdude* に`-e -F -U`の三つ組オプションを加えることで、
フューズを工場出荷時状態に書き戻す（PB3端子をRESET専用に戻す）ことが出来る。

```sh
avrdude -v -p attiny10 -c tpi4avr \
  -C avrdude.conf.tinyrc \
  -P <SerialPort> \
  -e -F -U fuse:w:0xff:m
```

## HV書込対応外付回路

高電圧プログラミングを可能にするには、何らかの手段で __12V__ 電圧を得る必要がある。
そこで`HVP1`と`HVP2`信号を使うと、簡単な部品の追加で昇圧回路（3段チャージポンプ）を構成できるようになっている。
数ミリAほどの低出力しかとれないが高電圧プログラミングには充分だ。

```pre
        C
HVP1 ---||--+------+      +----+----+---- HV
            |      |      |    |    |
          D -      V    D -    |    |
            A    D -      A    = C  ~ ZD
            |      |      |    |    A
HVP2 -------+--||--+------+    |    |     x3 C  = 0.1uF
               C               |    |     x3 D  = SBD (Vf=0.3V)
                               =    =     x1 ZD = Zener diode 12V
                              GND  GND
Fig.2 Tripled Chargepump
```

この高電圧レベルをターゲットMCUの`RESET/PB3`端子に印加するには、
MOSFETによるスイッチ回路を用意して`HVEN`端子から制御する。
`TRST`信号とターゲットMCUの`RESET/PB3`端子間にはレベルシフトMOSFETを設ける。

```pre
                 S D                           x1 Pch = BSS84 or IRLML6402
  HV -----+-------<----------------------+     x2 Nch = BSS138
          |      |A|                     |
          |      === Pch                 |           VCC
          |  R1   |                      |            |
          +--VVV--+                      |         D  |
                  |                      |       +-|<-+----+
                |-|D                     |       |    |    |
H.HVEN -----+---|>A  Nch                 |    R4 >    |G   > R5
            |   |-|S                     |       >   ===   >
         R2 >     |                      |  R3   |   |A|   |
            >     |      T.RESET/PB3 ----+--VVV--+----<----+---- H.TRST
            |     |                                  D S
            =     =                                  Nch
            V     V     R1 = R2 = 10kohm
                        R4 = R5 = 100kohm   R3 = 330ohm  D = SBD (Vf=0.3V)

Fig.3 HV Switching
```

以上の回路を組むには以下の外付部品が別途必要。

|種類|個数|説明|
|-|-|-|
|SBD|4|ショットキーバリアダイオード耐圧16V以上 Vf=0.3V以下
|N-MOSFET|2|BSS138など（レベルシフトとP-MOSゲート制御用）
|P-MOSFET|1|BSS84またはIRLML6402など（HV印加用）
|ZD 12V|1|ツェナーダイオード 12V（絶対定格制限用）
|R 330Ω|1|抵抗器330オーム（過電流導通保護）
|R 10kΩ|2|抵抗器10kオーム（2箇所の MOSFETゲートプルアップ/ダウン）
|R 100kΩ|2|抵抗器100kオーム（レベルシフト前後2箇所のプルアップ）
|C 0.1uf|3|セラミックコンデンサ0.1uf耐圧16V以上

> TPIの場合、通常のプログラミング中は`RESET`をLOWのまま維持するが、
HV書込時は 12V HIGH を維持し続けなければならない。
（これはPDIも同じ。UPDIは全く違う）

## 注意

- AS IS（無保証、無サポート、使用は自己責任で）
- UNO WiFi、Nano Every などは PCとの
UART通信開始時の自動リセットを無効化することができない。
このため TPI4AVR 通信開始時に自分自身のリセットを含む、数秒以上の無応答遅延時間がある。
  - オンボードデバッガによって直接リセットされるため、外付部品追加での回避はできない。
  - より高度な機能を活用するにはベアチップと専用の回路とで構築するほうが好ましい。
- Arduino IDE からは高電圧プログラミング、デバイス施錠、デバイス解錠、
EEPROM書換などの高度な指示をすることは原則出来ない。
これらはコマンドラインから *avrdude* を操作することで対応する。
  - [Multix Zinnia Product SDK](https://askn37.github.io/) はサブメニューで選べる。
- デバッガインタフェースとしては機能しない。特殊領域書換とチップ消去にのみ対応。
- 高電圧プログラミングには、追加のハードウェア支援が絶対に必要。（必然的に）

### うまく動かない場合

- メニュー設定を確認する
  - AVR のボードマネージャー種別
    - megaAVR-0、tinyAVR-2、AVR DA/DB の各系統以外ではビルドできない
    - ATmega328P 等の旧世代 AVR は使用不能
  - サブメニュー選択
  - シリアルポートの選択
  - 書込装置の選択
- 配線を確認する
  - 電源供給はホスト側かターゲット側のどちらか一方からのみ供給する（電圧を揃える）
  - なるべく短い線材を使う
- 復旧に高電圧プログラミングが必要になる FUSE設定を安易に試さない
  - そういう FUSE をくれぐれも間違って書かない

## 課題（Rev.2に向けて）

- USARTを2系統使うため少ピン品種で選べる形式が限られる。（tinyAVR-2のみ）
  - CCL/LUTや PORTMUXを駆使すれば1系統消費で済み対応品種を増やせるかも。
  - むしろそうしないと AVR DD 少ピン品種で対応できない。（販売開始が遅かったので考慮しなかった）
- DTR / RTS信号線を活用していない。
  - 元々Arduino互換機設計あわせだったのでリセット検出にしか使えていない。
  - レベル検出可能なハード設計に特化すれば *avrdude* との連携を深められるはず。

## 関連リンク

- [askn37 / Multix Zinnia Product SDK](https://askn37.github.io/)
-- megaAVR-0, tinyAVR-0/1/2, AVR DA/DB/DD series support
- [MCUdude / MegaCoreX](https://github.com/MCUdude/MegaCoreX)
-- megaAVR-0 series support
- [SpenceKonde / megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)
-- tinyAVR-0,1,2 series support
- [SpenceKonde / DxCore](https://github.com/SpenceKonde/DxCore)
-- AVR DA,DB,DD series support

## 著作表示

Twitter: [@askn37](https://twitter.com/askn37) \
GitHub: [https://github.com/askn37/](https://github.com/askn37/) \
Product: [https://askn37.github.io/](https://askn37.github.io/)

Copyright (c) askn (K.Sato) multix.jp \
Released under the MIT license \
[https://opensource.org/licenses/mit-license.php](https://opensource.org/licenses/mit-license.php) \
[https://www.oshwa.org/](https://www.oshwa.org/)

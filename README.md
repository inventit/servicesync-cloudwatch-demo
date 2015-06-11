ServiceSync 教育用サンプルプログラム
=======================

## 概要
このアプリケーションは、ServiceSyncを使ってアプリケーションを開発するためのサンプルプログラムです。
サンプルとして２つのアプリケーションを提供します。

* CPU使用率監視
* 温度監視

### CPU監視
ゲートウェイ機器のCPU使用率を定期的にサーバーにアップロードします。
サーバー側は受信したCPU使用率情報をログに出力します。

### 温度監視
Armadillo-IoTに搭載された温度センサを用いて、取得した温度情報を定期的にサーバーにアップロードします。
サーバー側は受信した温度情報をDBに保存します。

保存された温度情報はRESTクライアントを用いて、CSV形式で取得することができます。

また、温度監視の開始・停止を、Armadillo-IoTに搭載されたスイッチ、および、RESTクライアントから制御することができます。

## 動作環境
### ゲートウェイ機器
* PC版シミュレータ環境 （CPU監視のみ）
* 株式会社アットマークテクノ製 Armadillo-IoT

### ServiceSync
* ServiceSync 2.0以上

### ServiceSync Clinet
* 1.0.6以上

### Moat-C SDK
* 1.0.5以上

## 事前準備

### ATDE5のセットアップ

下記のURLからATDE5をダウンロードしセットアップを行ってください。

* http://armadillo.atmark-techno.com/armadillo-iot/downloads

### パッケージの追加

ServiceSyncのクライアントアプリケーションおよびサーバープラグインの開発に必要なパッケージをインストールするために、以下のコマンドを実行してください。

```
$ sudo apt-get install gyp ¥
	checkinstall ¥
	openjdk-7-jdk
```

### IIDNコマンドのセットアップ

#### Node.jsのセットアップ

IIDNコマンドは Node.js v0.8.8 以上を必要します。下記の手順で Node.js をインストールしてください。

```
$ wget -N http://nodejs.org/dist/v0.8.8/node-v0.8.8.tar.gz
$ tar xzvf node-v0.8.8.tar.gz && cd node-v*
$ ./configure
$ sudo checkinstall
$ sudo dpkg -i node_*
```

`checkinstall` で尋ねられる質問には、以下のように回答してください。

```
The package documentation directory ./doc-pak does not exist. 
Should I create a default set of package docs?  [y]:
```
**y** と入力します。

```
パッケージのドキュメンテーションを準備..OK

このパッケージの説明を書いてください
説明の末尾は空行かEOFにしてください。
>>
```
特に記入の必要はないので、Enter を押してください。

```
*****************************************
**** Debian package creation selected ***
*****************************************

このパッケージは以下の内容で構成されます: 

0 -  Maintainer: [ root@atde5 ]
1 -  Summary: [ Package created with checkinstall 1.6.2 ]
2 -  Name:    [ node ]
3 -  Version: [ v0.8.8 ]
4 -  Release: [ 1 ]
5 -  License: [ GPL ]
6 -  Group:   [ checkinstall ]
7 -  Architecture: [ amd64 ]
8 -  Source location: [ node-v0.8.8 ]
9 -  Alternate source location: [  ]
10 - Requires: [  ]
11 - Provides: [ node ]
12 - Conflicts: [  ]
13 - Replaces: [  ]

変更するものの番号を入力してください。Enterで続行します:
```
Versionの指定方法がdebianパッケージのルールに合わないため変更します。 **3** を入力してください。

```
変更するものの番号を入力してください。Enterで続行します: 3
バージョンを入力: 
>>
```
**0.8.8** と入力してください。

```
このパッケージは以下の内容で構成されます: 

0 -  Maintainer: [ root@atde5 ]
1 -  Summary: [ Package created with checkinstall 1.6.2 ]
2 -  Name:    [ node ]
3 -  Version: [ 0.8.8 ]
4 -  Release: [ 1 ]
5 -  License: [ GPL ]
6 -  Group:   [ checkinstall ]
7 -  Architecture: [ amd64 ]
8 -  Source location: [ node-v0.8.8 ]
9 -  Alternate source location: [  ]
10 - Requires: [  ]
11 - Provides: [ node ]
12 - Conflicts: [  ]
13 - Replaces: [  ]

変更するものの番号を入力してください。Enterで続行します:
```
Versionが **0.8.8** となっていることを確認し、Enterを押してください。

```
Some of the files created by the installation are inside the home directory: /home

You probably don't want them to be included in the package.
それらを表示しますか？ [n]:
```
**n** と入力してください。

```
それらをパッケージから除外しますか？(yesと答えることをおすすめします) [n]:
```
**y** と入力してください。

#### IIDNコマンドのセットアップ

GitHubからIIDNコマンドを取得してください。

```
$ git clone https://github.com/inventit/iidn-cli.git
$ cd iidn-cli
$ ./iidn --help
[IIDN] ** Using node... **
[IIDN] unknown command:--help
[IIDN] iidn <COMMAND> [ARGS]
[IIDN] VERSION: 1.0.1-r4
[IIDN] COMMAND:
[IIDN]  signup      .... Allows you to sign up IIDN. Your OAuth2 account is mandatory.
[IIDN]  reset       .... Allows you to reset your client_secret. Your OAuth2 account is mandatory.
[IIDN]  jsdeploy    .... Allows you to deploy your MOAT js script package archive.

… (snip) …
```
デフォルトでは、IIDNコマンドはServiceSyncのSandbox環境に接続します。
お使いの環境に合わせて `common.js` 内の `MOAT_REST_API_URI` を変更してください。

```
/**
 * Copyright 2014 Inventit Inc.
 */

var IIDN_TERM_OF_SERVICE_URI = 'http://dev.inventit.io/files/TERMS.txt';
var IIDN_EULA_URI = 'http://dev.inventit.io/files/2013JAN-IBCLA.txt';
var MOAT_REST_API_URI = 'https://sandbox.service-sync.com/moat/v1';  <== この行を変更
var SIGNUP_TIMEOUT_MS = 5 * 60 * 1000;
var cred = [];
```

## CPU監視

CPU監視アプリケーションは、デバイス（ゲートウェイ）のCPU使用率を計測し、３秒間隔でServiceSyncサーバーにアップロードします。

サーバープラグインは、受信したデータをログに出力します。

### ソースコード取得

下記のULRからサンプルコードをダウンロードしてください。

* （準備中）

サンプルコードを展開します。

```
atmark@atde5:~$ tar xzvf egypt-educational-samples.tar.gz
```

CPU監視アプリケーションのファイル構成は以下の通りです。

```
egypt-educational-samples/cpu-usage-monitoring/
├── c    ... クライアントアプリケーション
│   ├── Makefile
│   ├── moatapp.gyp
│   ├── common.gypi
│   ├── configure
│   ├── include/servicesync/moat    .... MOAT-C SDKヘッダファイル
│   ├── package
│   │   └── package.json
│   ├── src
│   │   └── sample-cpu-usage-monitoring.c    .... クライアントアプリケーション本体
│   ├── test
│   └── tools
└── js    ... サーバープラグイン 
    ├── common-tasks.rb
    └── sample-cpu-usage-monitoring
        ├── Rakefile
        ├── package.json
        └── upload-cpu-usage!1.0.0.js    .... サーバープラグイン本体
```

### プラットフォーム証明書

`egypt-educational-samples/cpu-usage-monitoring/certs` に `moat.pem` という名前でプラットフォーム証明書が格納してください。

プラットフォーム証明書の入手についてはプラットフォーム提供者にお問い合わせください。

### token.binの取得

アプリケーションパッケージは、アプリケーションIDに紐づく証明書で署名されている必要があります。
IIDNコマンドを用いて署名を行うためのトークンを取得します。

```
$ ./iidn-cli$ ./iidn tokengen sample-cpu-usage-monitoring
[IIDN] ** Using node... **
[IIDN] Enter your appId:
<Input your appId>
[IIDN] Enter your clientId:
<Input your clientId>
[IIDN] Enter your password:
<Input your passowd>
[IIDN] Downloading the secure token for the package sample-cpu-usage-monitoring … (snip) …
```
`(13桁の数値)-token.bin` という名前でトークンが生成されるので、 `token.bin` という名前でコピーしてください。

```
$ cp 1424238427323-token.bin ~/egypt-educational-samples/cpu-usage-monitoring/c/package/token.bin
```

### ビルド
#### サーバープラグイン

サーバープラグインのパッケージを作成するために、以下のコマンドを実行します。
`pkg` の中に `sample-cpu-usage-monitoring-1.0.0.zip` という名前でパッケージが作成されます。

```
$ cd ~/egypt-educational-samples/cpu-usage-monitoring/js/sample-cpu-usage-monitoring
$ rake clobber_package package
```

#### クライアントアプリケーション

シミュレータ環境用のパッケージを作成する場合は、以下のコマンドを実行します。
`sample-cpu-usage-monitoring_1.0.0_x86_64.zip` という名前でパッケージが作成されます。

```
$ cd ~/egypt-educational-samples/cpu-usage-monitoring/c/
$ ./configure
$ make
$ make package
```

Armadillo-IoT用のパッケージを作成する場合は、以下のコマンドを実行します。
`sample-cpu-usage-monitoring_1.0.0_arm.zip` という名前でパッケージが作成されます。

```
$ cd ~/egypt-educational-samples/cpu-usage-monitoring/c/
$ export CROSS=arm-linux-gnueabi-
$ export CC=${CROSS}gcc
$ export CXX=${CROSS}g++
$ export AR=${CROSS}ar
$ export LD=${CROSS}ld
$ export RANLIB=${CROSS}ranlib
$ export STRIP=${CROSS}strip
$ ./configure --dest-cpu=arm
$ make
$ make package
```

### デプロイ＆インストール

ServiceSync WebConsoleにログインし、パッケージのデプロイおよびインストールを行います。

クライアントアプリケーションをする前に、デバイス登録が完了している必要があります。

#### サーバープラグイン

* サイドメニューから [アプリ管理] - [パッケージ] を選択してください。
* [サーバーパッケージ] タブを選択してください。
* パッケージ欄の [登録] ボタンをクリックしてください。
* ポックアップの [追加] ボタンをクリックし、先ほど作成した `sample-cpu-usage-monitoring-1.0.0.zip` を追加します。
* 必要に応じて [ラベル] と [説明] を記入してください。
* ポックアップの [送信する] ボタンを押してください。
* [サーバーパッケージ一覧] から追加したパッケージを選択し、[詳細情報] の [インストール] ボタンをクリックします。
* ポックアップの [OK] ボタンを押すとインストールが完了します。

#### クライアントアプリケーション

* サイドメニューから [アプリ管理] - [パッケージ] を選択してください。
* [ゲートウェイパッケージ] タブを選択してください。
* パッケージ欄の [登録] ボタンをクリックしてください。
* ポックアップの [追加] ボタンをクリックし、先ほど作成した `sample-cpu-usage-monitoring_1.0.0_x86_64.zip` または `sample-cpu-usage-monitoring_1.0.0_arm.zip` を追加します。
* [プラットフォーム] は [ネイティブアプリ] を選択してください。選択すると、[OS] と [Arch] 欄が現れます。
* [OS] として　`Linux` を選択してください。
* [Arch] として `x86` または `arm` 適切な設定を選択してください。
* 必要に応じて [ラベル] と [説明] を記入してください。
* ポックアップの [送信する] ボタンを押してください。
* [ゲートウェイパッケージ一覧] から追加したパッケージを選択し、[詳細情報] の [インストール] ボタンをクリックします。
* デバイスを選択し、[インストール] ボタンをクリックします。
* インストールの進行状況はサイドメニューの [ジョブ] か確認できます。

### 動作確認

クライアントアプリケーションのインストールが完了すると、CPU使用率のアップロードが開始されます。
サーバー側およびクライアント側のログを確認することで、動作を確認することができます。

#### サーバーログの確認

IIDNコマンドを用いて、サーバーログの確認を行うことができます。

```
$ ./iidn log
[IIDN] ** Using node... **
[IIDN] Enter your appId:
<== [アプリケーションID] を入力してください。
[IIDN] Enter your clientId:
<== [クライアントID] を入力してください。
[IIDN] Enter your password:
<== [パスワード] を入力してください。
[IIDN] Tailing the MOAT js server script log:
```

正しく動作すると、以下のようなログが表示されます。

```
[IIDN] 2015-02-18 10:06:45,548 JST <744> [upload-cpu-usage] Start!
[IIDN] 2015-02-18 10:06:45,549 JST <744> [upload-cpu-usage] [CpuUsage]
[IIDN] 2015-02-18 10:06:45,550 JST <744> [upload-cpu-usage]     user      = [0.000000]
[IIDN] 2015-02-18 10:06:45,550 JST <744> [upload-cpu-usage]     nice      = [0.000000]
[IIDN] 2015-02-18 10:06:45,551 JST <744> [upload-cpu-usage]     system    = [0.000000]
[IIDN] 2015-02-18 10:06:45,551 JST <744> [upload-cpu-usage]     idle      = [100.000000]
[IIDN] 2015-02-18 10:06:45,552 JST <744> [upload-cpu-usage]     timestamp = [1424221783899]
[IIDN] 2015-02-18 10:06:45,553 JST <744> [upload-cpu-usage] save : entity = [CpuUsage]
[IIDN] 2015-02-18 10:06:45,561 JST <744> [upload-cpu-usage] save : array = [[]]
[IIDN] 2015-02-18 10:06:45,576 JST <744> [upload-sensing-data:save] The object@uid:38d78103-4f6e-474c-a373-b48cc7999e1a has been saved to the database.
[IIDN] 2015-02-18 10:06:45,578 JST <744> [upload-cpu-usage] uid = [38d78103-4f6e-474c-a373-b48cc7999e1a] have been saved into the database.
[IIDN] 2015-02-18 10:06:45,579 JST <744> [upload-cpu-usage] Done!
```

#### クライアントログの確認

クライアントのログは syslog に表示されます。

* シミュレータ環境の場合、 `/var/log/syslog` を参照してください。
* Armadillo-IoTの場合、 `/var/log/messages` を参照してください。

`/opt/inventit/ssegw/res/gwconf.json` を編集することで、ログレベルおよびログの出力先を変更することができます。

ログを syslog の代わりにファイルに出力するには、[config_log] - [direction] に `file` を指定します。
ファイルパスは [config_log] - [file] - [path] で指定します。

```
{
        "config_log" : {
                "direction" : "file",
... (snip) ...
                "file" : {
                        "path" : "/opt/inventit/ssegw/res/ssegw.log",
                        "limitSize" : 2048000
                }
```

ログレベルを変更するには、[config_log] - [filter] を変更します。

```
{
        "config_log" : {
                "direction" : "file",
                "filter" : {
                        "error" : true,
                        "warn" : false,
                        "info" : true,
                        "trace" : false,
                        "debug" :false
                },
```

## 温度監視

温度監視アプリケーションは、Armadillo-IoTに搭載された温度センサーによって取得された温度情報を定期的にサーバーにアップロードします。

温度情報は１０秒間隔で計測されて、一旦、クライアント（ゲートウェイ）内に保存されます。保存された温度情報は３０秒間隔でサーバーにアップロードされます。

温度情報を受信したサーバープラグインは、データベースにその情報を保存します。

保存されたデータは、MOAT REST APIを用いて取得することができます。

### ソースコード取得

ソースコードは『CPU監視』アプリケーションと同じアーカイブに格納されています。

温度監視アプリケーションのファイル構成は以下の通りです。

```
egypt-educational-samples/temperature-monitoring/
|-- REST ... RESTクライアント
|   |-- monitoring.py  ... 監視開始・停止用 REST クライアント
|   `-- report.py  ... 情報取得用 REST クライアント
|-- c ... クライアントアプリケーション
|   |-- Makefile
|   |-- common.gypi
|   |-- moatapp.gyp
|   |-- configure
|   |-- include/servicesync/moat ... MOAT C SDK ヘッダファイル
|   |-- package
|   |   `-- package.json
|   |-- src
|   |   |-- led.c
|   |   |-- led.h
|   |   |-- sensor.c
|   |   |-- sensor.h
|   |   |-- switch.c
|   |   |-- switch.h
|   |   |-- temperature-monitoring.c
|   |   |-- uploader.c
|   |   `-- uploader.h
|   |-- test
|   `-- tools
`-- js ... サーバープラグイン
    |-- common-tasks.rb
    `-- temperature-monitoring
        |-- Rakefile
        |-- package.json
        |-- startstop-monitoring!1.0.0.js
        `-- upload-temperature!1.0.0.js
```

### プラットフォーム証明書

`egypt-educational-samples/temperature-monitoring/certs` に `moat.pem` という名前でプラットフォーム証明書が格納してください。

プラットフォーム証明書の入手についてはプラットフォーム提供者にお問い合わせください。

### token.binの取得

アプリケーションパッケージは、アプリケーションIDに紐づく証明書で署名されている必要があります。
IIDNコマンドを用いて署名を行うためのトークンを取得します。

```
$ ./iidn-cli$ ./iidn tokengen temperature-monitoring
[IIDN] ** Using node... **
[IIDN] Enter your appId:
<Input your appId>
[IIDN] Enter your clientId:
<Input your clientId>
[IIDN] Enter your password:
<Input your passowd>
[IIDN] Downloading the secure token for the package sample-cpu-usage-monitoring … (snip) …
```
`(13桁の数値)-token.bin` という名前でトークンが生成されるので、 `token.bin` という名前でコピーしてください。

```
$ cp 1424238427323-token.bin ~/egypt-educational-samples/temperature-monitoring/c/package/token.bin
```

### ビルド
#### サーバープラグイン

サーバープラグインのパッケージを作成するために、以下のコマンドを実行します。
`pkg` の中に `temperature-monitoring-1.0.0.zip` という名前でパッケージが作成されます。

```
$ cd ~/egypt-educational-samples/temperature-monitoring/js/temperature-monitoring
$ rake clobber_package package
```

#### クライアントアプリケーション

Armadillo-IoT用のパッケージを作成する場合は、以下のコマンドを実行します。
`temperature-monitoring_1.0.0_arm.zip` という名前でパッケージが作成されます。

```
$ cd ~/egypt-educational-samples/temperature-monitoring/c
$ export CROSS=arm-linux-gnueabi-
$ export CC=${CROSS}gcc
$ export CXX=${CROSS}g++
$ export AR=${CROSS}ar
$ export LD=${CROSS}ld
$ export RANLIB=${CROSS}ranlib
$ export STRIP=${CROSS}strip
$ ./configure --dest-cpu=arm
$ make
$ make package
```

### デプロイ＆インストール

CPU監視アプリケーションと同様に、クライアントパッケージ、および、サーバーパッケージをインストールしてください。

### RESTクライアントの準備

RESTクライアントを用いて収集したデータの取得等を行います。

RESTクライアントは以下に格納されています。

```
~/egypt-educational-samples/temperature-monitoring/REST
|-- dot.env
|-- moat/
|-- monitoring.py
`-- report.py
```

`dot.env` を `.env` とい名前でコピーして、中身を更新してください。

* MOAT_REST_API_URI
    - 使用するサーバー環境（DMS）の MOAT REST API の URL を記入してください。
    - ご不明な場合は、プラットフォーム提供者にお問い合わせください。
* APP_ID, CLIENT_ID, CLIENT_SECERT
    - ServiceSync Web コンソールにログインし、[設定] - [IIDN設定] を選択し、[API認証情報]で表示される値を記入してください。
* DEVICE_NAME
    - ServiceSync Web コンソールにログインし、[デバイス管理] - [デバイス] を選択し、[詳細情報] に表示される [デバイスID] を記入してください。
     - e.g. `AGI:00:11:0C:18:10:B6`

環境変数を設定するために、RESTクライアントを実行する前に、下記のコマンドを実行してください。

```
$ . .env
```

### 動作確認

クライアントアプリケーションが起動すると、Armadillo-IoT本体の、左から２番目のLEDが点灯します。
この状態では、まだ、温度監視は開始されていません。

一番左の本体スイッチを押すか、RESTクライアントを用いて、温度監視を開始することができます。

```
$ cd ~/egypt-educational-samples/temperature-monitoring/REST
$ ./monitoring.py start
{
  "activatedAt": "Mon, 09 Mar 2015 11:54:18 +0000",
  "arguments": {
    "command": "start",
    "uid": "402881634b9beea5014bc954b8582289"
  },
  "expiredAt": "Mon, 21 Jan 2100 13:05:55 +0000",
  "jobServiceId": "urn:moat:8fac244f-0a28-4995-bfab-453fdc5ae56d:temperature-monitoring:startstop-monitoring:1.0.0",
  "name": "TEST:shasegawa:0001",
  "uid": "402881634bfd1328014bfe62bb04024d"
}
```

温度監視が開始すると、左から３番目のLEDが点灯します。また、温度情報をサーバーにアップロード中は左から４番目のLEDが点灯します。
（アップロードは一瞬で終わるため、点灯時間は１秒程度です。）

同様に、一番左の本体スイッチをもう一度押すか、RESTクライアントを用いて、温度監視を停止することができます。

```
$ ./monitoring.py stop
{
  "activatedAt": "Mon, 09 Mar 2015 11:58:18 +0000",
  "arguments": {
    "command": "stop",
    "uid": "402881634b9beea5014bc954b8582289"
  },
  "expiredAt": "Mon, 21 Jan 2100 13:05:55 +0000",
  "jobServiceId": "urn:moat:8fac244f-0a28-4995-bfab-453fdc5ae56d:temperature-monitoring:startstop-monitoring:1.0.0",
  "name": "TEST:shasegawa:0001",
  "uid": "402881634bfd1328014bfe6664a70255"
}
```

アップロードされたデータは、RESTクライアントを用いて取得することができます。

```
$ ./report.py
"timestampe", "temperature"
"2015-03-09 11:26:30.335", 33.75
"2015-03-09 11:26:31.333", 33.75
"2015-03-09 11:26:32.344", 33.75
... (snip) ...
"2015-03-09 20:57:42.965", 38.625
"2015-03-09 20:57:52.974", 38.625
"2015-03-09 20:58:02.985", 38.625
```


## バージョン
* 1.0.1
 
## 変更履歴

#### Changes in `1.0.1` (2015/04/23)

* ATDE5 (32bit) support
* Fix using old MOAT model description rule in CPU usage monitoring

#### Changes in `1.0.0` (2015/03/16)

* Initial Release

ServiceSync CloudWatch Gateway App
========

## 概要
ServiceSyncとCloudWatch連携のゲートウェイアプリです。
このアプリケーションは、定期的にセンサーデータを収集しアップロードします。

##事前準備
クライアントアプリケーション、サーバープラグインをビルドできる環境を整えます。[試しにArmadillo IoTでアプリ開発！](https://developers.servicesync.net/?page_id=300)を参照して、サンプルプログラム①を動かせるまでの環境を構築してください。

## センサーデータ

送信するセンサーデータは、以下のフィールドから構成されています。

 | データ種別  | 説明  |
 |-------------|-----------------|
 | temperature  | 温度(℃)です。0.0 - 40.0のランダムな値です。 |
 | humidity  | 湿度(%)です。0.0 - 100.0のランダムな値です。 |
 | timestamp  | センサーデータの収集時刻です。ISO8601形式の文字列です。 |

## アップロード周期

アップロード周期は、`sscw.c`にて、定数で定義されています。ここを変更するとアップロード周期を変更することが出来ますが、
CloudWatchの制約上、あまりにも短い間隔ではアップロードすることは出来ません。制約については、[CloudWatchの制限](http://docs.aws.amazon.com/ja_jp/AmazonCloudWatch/latest/DeveloperGuide/cloudwatch_limits.html)を確認してください。

```
#define UPLOAD_INTERVAL (60) /* sec */
```

##ディレクトリ構成
ディレクトリ構成は、以下に示す通りです。以降、ルートディレクトリを、`${MOATC_ROOT}`と表記します。

```
${MOATC_ROOT}
├── Makefile
├── moatapp.gyp
├── common.gypi
├── configure
├── certs/moat.pem … プラットフォーム証明書
├── include/servicesync/moat … MOAT-C SDKヘッダファイル
├── package
│   └── package.json
├── src
│   └── sscw.c … クライアントアプリケーション本体
├── test
└── tools
```

##証明書(moat.pem)の格納
プラットフォーム証明書は ${MOATC_ROOT}/certs に moat.pem という名前で格納してください。
プラットフォーム証明書は使用する ServiceSyncサーバー環境のサーバー管理者から入手してください。

##token.binの生成と格納
ファイル名1404886824665-token.binは異なりますので、適宜読み替えてください。
$ cd iidn-cli
$ iidn tokengen dominica
'1404886824665-token.bin'が生成されます。

$ mv 1404886824665-token.bin ${MOATC_ROOT}/package/token.bin

備考
iidn-cliはgitからcloneできます。
$ git clone https://github.com/inventit/iidn-cli.git

## コマンド

###Configureの実行
$ cd ${MOATC_ROOT}
$ ./configure

### ビルドとパッケージ作成
$ make
$ make package

`sscw-<version>-<cpu type>.zip`が作成されます。

## 変更履歴

### 1.0.0 Jun 26, 2015

- イニシャルリリース
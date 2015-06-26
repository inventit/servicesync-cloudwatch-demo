ServiceSync CloudWatch App
========

## 概要

このアプリケーションは、ゲートウェイからアップロードされたデータを、[Amazone CloudWatch](http://aws.amazon.com/jp/cloudwatch/)にポストする、MOAT jsアプリです。

## ディレクトリ構成

ディレクトリ構成は、以下に示す通りです。以降、ルートディレクトリを、`${MOATJS_ROOT}`と表記します。

```
${MOATJS_ROOT}
├── .env
├── .gitignore
├── .npmignore
├── Gruntfile.js
├── README.md
├── dotenv
├── node_modules
├── package.json
├── src
└── test
```

## スクリプト

このパッケージは、以下のスクリプトを含んでいます。

- `upload-sensing-data.js`: ゲートウェイアプリからアップロードされたデータを、Amazon CloudWatchにポストするスクリプトです。

`test`ディレクトリにある、*.test.js は、ユニットテストのスクリプトです。

## モデル

| フィールド | 説明 |
|---|---|
| temperature | 温度です。`float`型です。 |
| humidity | 湿度です。`float`型です。 |
| timestamp | 収集した時刻のミリ秒表現です。`int64`型です。 |

## AWSの認証情報の取り扱い

**!!! IMPORTANT !!!**

このパッケージは、Amazon CloudWatch にアクセスするための認証情報等を、スクリプトに記述する必要があります。ただし、各個人の認証情報をスクリプトに定義したまま、バージョン管理システムにコミットするのは好ましくないので、直接ご自身の認証情報を、スクリプトにハードコーディングするのはやめてください。

その代わり、`grunt build`の際に、スクリプト内の認証情報を環境変数を使って置換するようにしています。`grunt build`実行前には、ご自身の`.env`ファイルに、認証情報を記述してください。`.env`ファイルは、`.gitignore`に追加されていますので、間違ってコミットされることはありません。

`.env`ファイルの雛形は、`dotenv`ファイルです。`dotenv`ファイルをコピーして、ご自身の認証情報を、以下に示すように`.env`に記述してください。

```
AWS_ENDPOINT = "monitoring.us-west-2.amazonaws.com"
AWS_ACCESS_KEY_ID = "ABCDEFGHIJKLMNOPQRSTU"
AWS_SECRET_ACCESS_KEY = "AxBbCcDdEeFfHhIiJjKkLl/MmNnOoPp"
```

`@@`が付与された箇所が、置換されます。

```
var aws_endpoint = '@@AWS_ENDPOINT',
    aws_access_key_id = '@@AWS_ACCESS_KEY_ID',
    aws_secret_access_key = '@@AWS_SECRET_ACCESS_KEY';
```

## コマンド

### セットアップ

依存するパッケージをインストールします。最初に必ず実行する必要があります。

    npm install

### ビルドとユニットテスト

スクリプトの難読化やユニットテストを実行します。このコマンドを実行すると、ユニットテストやスクリプトの難読化を行った後、パッケージに含めるファイルを、`build`ディレクトリにコピーします。

    grunt

### パッケージの作成

デプロイ可能な配布パッケージを作成します。以下のコマンドを実行すると、`sscw-<version>.zip`が作成されます。

    grunt pack

### クリーン

`grunt`コマンドで生成された中間生成物をクリーンします。

    grunt clean

## 変更履歴

### 1.0.0 Jun 26, 2015

- イニシャルリリース

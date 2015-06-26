ServiceSync CloudWatch 連携アプリ
=======================

## 概要

このアプリケーションは、[ServiceSync](https://developers.servicesync.net/)と[Amazon CloudWatch](http://aws.amazon.com/jp/cloudwatch/)との連携アプリです。
ゲートウェイで収集したセンサーデータを、CloudWatchに送信し、CloudWatchのコンソールでデータを閲覧することが出来ます。

## ディレクトリ構成

```
.
├── README.md
├── c .................. (1)
└── js ................. (2)
```

- (1) ゲートウェイアプリが配置されています。詳細については、`c/README.md`を参照してください。
- (2) サーバーアプリが配置されています。詳細については、`js/README.md`を参照してください。

## 変更履歴

### 1.0.0 Jun 26, 2015

- イニシャルリリース
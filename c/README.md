ServiceSync CloudWatch Gateway App
========

ServiceSyncとCloudWatch連携のゲートウェイアプリです。
このアプリケーションは、定期的にセンサーデータを収集しアップロードします。

## センサーデータ

送信するセンサーデータは、以下のフィールドから構成されています。

 | データ種別  | 説明  |
 |-------------|-----------------|
 | temperature  | 温度(℃)です。  |
 | humidity  | 湿度(%)です。  |
 | timestamp  | センサーデータの収集時刻です。ISO8601形式の文字列です。 |

## アップロード周期

アップロード周期は、`sscw.c`にて、定数で定義されています。ここを変更するとアップロード周期を変更することが出来ますが、
CloudWatchの制約上、あまりにも短い間隔ではアップロードすることは出来ません。制約については、[CloudWatchの制限](http://docs.aws.amazon.com/ja_jp/AmazonCloudWatch/latest/DeveloperGuide/cloudwatch_limits.html)を確認してください。

```
#define UPLOAD_INTERVAL (60) /* sec */
```

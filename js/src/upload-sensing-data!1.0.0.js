/*
 * JobServiceID:
 * urn:moat:${APPID}:sscw:upload-sensing-data:1.0.0
 * Description: Post sensing data to CloudWatch.
 */

var TAG = 'upload-sensing-data';

var moat = require('moat'),
    context = moat.init(),
    session = context.session,
    clientRequest = context.clientRequest;

  session.log(TAG, 'Start!');
  session.log(JSON.stringify(clientRequest.objects));

var aws_endpoint = '@@AWS_ENDPOINT',
    aws_access_key_id = '@@AWS_ACCESS_KEY_ID',
    aws_secret_access_key = '@@AWS_SECRET_ACCESS_KEY';

sendData2CloudWatch(aws_endpoint, aws_access_key_id, aws_secret_access_key);

/**
 * Returns an ISO8601 formatted date string.
 * 
 * @param d Date object to be parsed
 * @return String ISO8601 formatted string
 * @example
 * isoDateString(new Date(1434368351767)) // 2015-06-15T11:39:11Z
 */
function isoDateString(d) {
  function pad(n) {
    return n < 10 ? '0' + n : n;
  }
  return d.getUTCFullYear() + 
    '-' + pad(d.getUTCMonth()+1) + 
    '-' + pad(d.getUTCDate()) + 
    'T' + pad(d.getUTCHours()) + 
    ':' + pad(d.getUTCMinutes()) + 
    ':' + pad(d.getUTCSeconds()) + 
    'Z';
}

/* Returns encoded URI string according to RFC3986*/
function encodeURIComponent_RFC3986(str) {
  return encodeURIComponent(str).replace(/[!'()*]/g, function(c) {
    return '%' + c.charCodeAt(0).toString(16);
  });
}

function sendData2CloudWatch(endpoint, access_id, secret_key)
 {
     session.log(TAG,"ClouldWatch query Start!" );
     var timestamp = isoDateString(new Date());     

     //create Hash object
     var params = [
                   ["Action","PutMetricData"],
                   ["Namespace","SSCloudWatch"],
                   ["AWSAccessKeyId", access_id],
                   ["SignatureMethod", "HmacSHA256"],
                   ["SignatureVersion", 2],
                   ["Version", "2010-08-01"],
                   ["Timestamp", timestamp]
                   ];
    
     var arg_params = [
                       ["MetricData.member.1.MetricName" , "temperature"],
                       ["MetricData.member.1.Unit" , "Count"],
                       ["MetricData.member.1.Value" , 23.5],
                       ["MetricData.member.1.Dimensions.member.1.Name" , "Project"],
                       ["MetricData.member.1.Dimensions.member.1.Value" , "test"],
                       ["MetricData.member.2.MetricName" , "humidity"],
                       ["MetricData.member.2.Unit" , "Count"],
                       ["MetricData.member.2.Value" , 80.5],
                       ["MetricData.member.2.Dimensions.member.1.Name" , "Project"],
                       ["MetricData.member.2.Dimensions.member.1.Value" , "test"]
                       ];
     
     //params.merge(arg_params);
     for(var i=0; i< arg_params.length; i++){
         params.push(arg_params[i]);
     }
     
     //create canonical_querystring
     for(var j=0; j< params.length; j++){
         params[j][1] = encodeURIComponent_RFC3986(params[j][0]) +
                         "=" + encodeURIComponent_RFC3986(params[j][1]);
     }
     params.sort();

     var canonical_querystring = "";
     for(var k=0; k< params.length; k++){
         canonical_querystring += "&" + params[k][1];
     }
     //Remove redundant initial '&' 
     canonical_querystring =  canonical_querystring.substr(1);
     
     string_to_sign = "GET"+"\n" + endpoint +"\n" + "/" + "\n" + canonical_querystring;
     
     var signature_hex = session.hmac('SHA256','plain',secret_key,string_to_sign);
     signature = session.hex2b64(signature_hex);

     //Add signature on params
     params.push(["Signature", "Signature=" + encodeURIComponent_RFC3986(signature)]);
     params.sort();
     
     //Generagte querystring
     var querystring = "";
     for(var l=0; l< params.length; l++){
         querystring += "&" +  params[l][1];
     }
     querystring = querystring.substr(1);
     session.log(TAG,"querystring: " + querystring);
     
     var url_and_query = "https://" + endpoint + "/?" + querystring;
     session.log(TAG,"url_and_query: " + url_and_query);
     
     //Http Access
     var resp = session.fetchUrlSync(url_and_query, {
     method: 'GET',
     });

     if (parseInt(resp.responseCode / 100) === 2) {
        session.log(TAG, 'Success!');
     } else {
       throw 'Failed to post data: responseCode=' + resp.responseCode;
     }
   
     session.log(TAG,"CloudWatch query Finish!");
 }


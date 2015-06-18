/*
 * JobServiceID:
 * urn:moat:${APPID}:sscw:upload-sensing-data:1.0.0
 * Description: Post sensing data to CloudWatch.
 */

var TAG = 'upload-sensing-data';

var moat = require('moat'),
    context = moat.init(),
    session = context.session,
    clientRequest = context.clientRequest,
    objects = clientRequest.objects,
    container = objects[0];

var aws_endpoint = '@@AWS_ENDPOINT',
    aws_access_key_id = '@@AWS_ACCESS_KEY_ID',
    aws_secret_access_key = '@@AWS_SECRET_ACCESS_KEY';

session.log(TAG, 'Start!');
session.log(TAG, JSON.stringify(clientRequest.objects));

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
    '-' + pad(d.getUTCMonth() + 1) + 
    '-' + pad(d.getUTCDate()) + 
    'T' + pad(d.getUTCHours()) + 
    ':' + pad(d.getUTCMinutes()) + 
    ':' + pad(d.getUTCSeconds()) + 
    'Z';
}

/**
 * Returns an encoded URI string according to RFC3986
 * 
 * @return String encoded in RFC3986
 * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/encodeURIComponent#Description
 * @example
 * fixedEncodeURIComponent('foo @+%/') // foo%20%40%2B%25%2F
 */
function fixedEncodeURIComponent(str) {
  return encodeURIComponent(str).replace(/[!'()*]/g, function(c) {
    return '%' + c.charCodeAt(0).toString(16);
  });
}

function sendData2CloudWatch(endpoint, access_id, secret_key) {
  var timestamp = isoDateString(new Date(container.timestamp));     
  var params = [
    ["Action","PutMetricData"],
    ["Namespace","SSCloudWatch"],
    ["AWSAccessKeyId", access_id],
    ["SignatureMethod", "HmacSHA256"],
    ["SignatureVersion", 2],
    ["Version", "2010-08-01"],
    ["Timestamp", timestamp],
    ["MetricData.member.1.MetricName" , "temperature"],
    ["MetricData.member.1.Unit" , "Count"],
    ["MetricData.member.1.Value" , container.temperature],
    ["MetricData.member.1.Dimensions.member.1.Name" , "Project"],
    ["MetricData.member.1.Dimensions.member.1.Value" , "test"],
    ["MetricData.member.2.MetricName" , "humidity"],
    ["MetricData.member.2.Unit" , "Count"],
    ["MetricData.member.2.Value" , container.humidity],
    ["MetricData.member.2.Dimensions.member.1.Name" , "Project"],
    ["MetricData.member.2.Dimensions.member.1.Value" , "test"]
  ];
  
  params.forEach(function(element, index) {
    element[0] = fixedEncodeURIComponent(element[0]);
    element[1] = fixedEncodeURIComponent(element[1]);
    element[1] = element.join('=');
  });    
  params.sort();
  
  canonical_querystring = "";
  params.forEach(function(element, index) {
    canonical_querystring += '&' + element[1];
  });
  
  //Remove redundant initial '&'
  canonical_querystring =  canonical_querystring.substr(1);
  
  var string_to_sign = "GET"+"\n" + endpoint +"\n" + "/" + "\n" + canonical_querystring;
  var signature_hex = session.hmac('SHA256','plain',secret_key,string_to_sign);
  var signature = session.hex2b64(signature_hex);
  
  //Add signature into params
  params.push(["Signature", "Signature=" + fixedEncodeURIComponent(signature)]);
  params.sort();
  
  //Generagte querystring
  var querystring = "";
  params.forEach(function(element, index) {
    querystring += '&' + element[1];
  });
  querystring = querystring.substr(1);
  
  var url_and_query = "https://" + endpoint + "/?" + querystring;
  session.log(TAG,"url_and_query: " + url_and_query);
  
  //HTTP Access
  var resp = session.fetchUrlSync(url_and_query, {method: 'GET'});
  
  if (parseInt(resp.responseCode / 100) === 2) {
    session.log(TAG, 'Success!');
  } else {
    throw 'Failed to upload data: responseCode=' + resp.responseCode;
  }
}  

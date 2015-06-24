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

var aws_endpoint = '@@AWS_ENDPOINT',
    aws_access_key_id = '@@AWS_ACCESS_KEY_ID',
    aws_secret_access_key = '@@AWS_SECRET_ACCESS_KEY';

session.log(TAG, 'Start!');

var objects = clientRequest.objects;
if (objects.length === 0) {
  session.log('No objects are received');
  throw 'No objects are received!';
}

var datum = objects[0];
session.log(TAG, JSON.stringify(datum));

sendData2CloudWatch();

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

/**
 * Returns a string joined array elements with '&'
 * 
 * @return String 
 * @example
 * concatArrayElementsWithAmp(array) // 'ement1Str&ement2Str&element3Str'
 */
 function concatArrayElementsWithAmp(in_array){
    var str;
    in_array.forEach(function(element, index) {
      if(index === 0){
        str = element[1];
      } else {
        str += '&' + element[1];
      }
    });
    return str;
  }

function sendData2CloudWatch() {
  var timestamp = isoDateString(new Date(datum.timestamp));     
  var params = [
    ['Action', 'PutMetricData'],
    ['Namespace', 'SSCloudWatch'],
    ['AWSAccessKeyId', aws_access_key_id],
    ['SignatureMethod', 'HmacSHA256'],
    ['SignatureVersion', 2],
    ['Version', '2010-08-01'],
    ['Timestamp', timestamp],
    ['MetricData.member.1.MetricName', 'temperature'],
    ['MetricData.member.1.Unit', 'Count'],
    ['MetricData.member.1.Value', datum.temperature],
    ['MetricData.member.1.Dimensions.member.1.Name', 'Project'],
    ['MetricData.member.1.Dimensions.member.1.Value', 'test'],
    ['MetricData.member.2.MetricName', 'humidity'],
    ['MetricData.member.2.Unit', 'Count'],
    ['MetricData.member.2.Value', datum.humidity],
    ['MetricData.member.2.Dimensions.member.1.Name', 'Project'],
    ['MetricData.member.2.Dimensions.member.1.Value', 'test']
  ];
  
  params.forEach(function(element, index) {
    element[0] = fixedEncodeURIComponent(element[0]);
    element[1] = fixedEncodeURIComponent(element[1]);
    element[1] = element.join('=');
  });    
  params.sort();

  //Generate signature 
  var canonical_querystring = '';
  canonical_querystring = concatArrayElementsWithAmp(params);

  var string_to_sign = 'GET' + '\n' + aws_endpoint + '\n' + '/' + '\n' + canonical_querystring;
  session.log('string_to_sign: ' + string_to_sign);
  var signature_hex = session.hmac('SHA256', 'plain', aws_secret_access_key, string_to_sign);
  var signature = session.hex2b64(signature_hex);
  
  // Add signature into params
  params.push(['Signature', 'Signature=' + fixedEncodeURIComponent(signature)]);
  params.sort();
  
  // Generagte querystring
  var querystring = '';
  querystring = concatArrayElementsWithAmp(params);

  var url_and_query = 'https://' + aws_endpoint + '/?' + querystring;
  session.log(TAG,'url_and_query: ' + url_and_query);
  
  // HTTP Access
  var resp = session.fetchUrlSync(url_and_query, {method: 'GET'});
  if (parseInt(resp.responseCode / 100) === 2) {
    session.log(TAG, 'Success!');
  } else {
    throw 'Failed to upload data: responseCode=' + resp.responseCode + '\n responseContent: ' + JSON.stringify(resp.content);
  }
}  

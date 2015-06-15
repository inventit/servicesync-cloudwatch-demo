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

var resp = session.fetchUrlSync('http://localhost', {
  method: 'POST',
  contentType: 'application/json',
  payload: JSON.stringify(clientRequest.objects)
});

if (parseInt(resp.responseCode / 100) === 2) {
  session.log(TAG, 'Success!');
} else {
  throw 'Failed to post data: responseCode=' + resp.responseCode;
}

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

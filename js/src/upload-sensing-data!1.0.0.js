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

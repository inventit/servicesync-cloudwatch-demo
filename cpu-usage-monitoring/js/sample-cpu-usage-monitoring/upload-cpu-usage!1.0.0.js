/*
 * Jobserviceid:
 * urn:moat:${APPID}:sample-cpu-usage-monitoring$:upload-cpu-usage:1.0
 */

var TAG = "upload-cpu-usage";

// Main
(function() {
  var moat = require('moat');
  var context = moat.init();
  var session = context.session;
  var clientRequest = context.clientRequest;

  session.log(TAG, 'Start!');

  var objects = clientRequest.objects;
  var size = objects.length;
  for (var i = 0; i < size; i++) {
    var container = objects[i];
    session.log(TAG, 'container = [' + container + ']');
    session.log(TAG, '    user      = [' + container.user + ']');
    session.log(TAG, '    nice      = [' + container.nice + ']');
    session.log(TAG, '    system    = [' + container.system + ']');
    session.log(TAG, '    idle      = [' + container.idle + ']');
    session.log(TAG, '    timestamp = [' + container.timestamp + ']');
  }

  session.log(TAG, 'Done!');
})();

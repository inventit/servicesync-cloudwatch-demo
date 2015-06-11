/*
 * Jobserviceid:
 * urn:moat:${APPID}:temperature-monitoring:startstop-monitoring:1.0
 */

var TAG = "startstop-monitoring";

// Main
(function() {
  var moat = require('moat');
  var context = moat.init();
  var session = context.session;

  session.log(TAG, 'Start!');

  var args = context.clientRequest.dmjob.arguments;
  session.log(TAG, 'args =>' + args);

  // Run SensingData#startstop() method on the remote device.
  var mapper = session.newModelMapperStub('SensingData');
  var device = mapper.newModelStub(args.uid);
  device.startstop(session, args.command, {
    success: function(obj) {
      session.log(TAG, 'Successful');
    },
    error: function(type, code) {
      session.log(TAG, 'Type:' + type + ',Unexpected status => ' + code);
      session.notifyAsync({
        result: 'error',
        code: code,
        type: type
      });
    }
  });

  session.log(TAG, 'Done!');
})();

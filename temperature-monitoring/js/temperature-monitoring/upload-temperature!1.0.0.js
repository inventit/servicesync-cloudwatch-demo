/*
 * Jobserviceid:
 * urn:moat:${APPID}:temperature-monitoring:upload-temperature:1.0
 */

var TAG = "upload-temperature";

// Store the given model object into the database.
function save(session, database, entity) {
  var result;
  if (entity.uid) {
    // try to find
    var array = database.queryByUids('SensingData', [entity.uid]);
    if (array && array.length > 0) {
      // Copy revision
      entity.rev = array[0].rev;
      result = database.update(entity);
    } else {
      result = database.insert(entity);
    }
  } else {
    result = database.insert(entity);
  }
  // The inserted object is internally associated with the device where the data origins.
  session.log(TAG, 'The object@uid:' + result.uid + ' has been saved to the database.');
}

// Main
(function() {
  var moat = require('moat');
  var context = moat.init();
  var session = context.session;
  var clientRequest = context.clientRequest;
  var device = clientRequest.device;
  var database = context.database;

  session.log(TAG, 'Start!');

  var objects = clientRequest.objects;
  var size = objects.length;

  for (var i = 0; i < size; i++) {
    var container = objects[i];
    if (container.array) {
      var array = container.array;
      for (var j = 0; j < array.length; j++) {
        session.log(TAG, 'temperature = ' + array[j].temperature);
        session.log(TAG, 'timestamp   = ' + array[j].timestamp);
        save(session, database, array[j]);
      }
    } else {
      session.log(TAG, 'temperature = ' + container.temperature);
      session.log(TAG, 'timestamp   = ' + container.timestamp);
      save(session, database, container);
    }
  }

  try {
    session.notifyAsync(objects);
  } catch (e) {
    session.log(TAG, 'error', e);
  }

  session.log(TAG, 'Done!');
})();

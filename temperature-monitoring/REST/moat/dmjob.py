#!/usr/bin/env python

import os
import time
import json
import urllib2
import device

MOAT_REST_API_URI = os.environ.get('MOAT_REST_API_URI', 'https://sandbox.service-sync.com/moat/v1')

def get(token):
    url = "%s/sys/dmjob?token=%s" % (MOAT_REST_API_URI, token)
    request = urllib2.Request(url)
    response = json.load(urllib2.urlopen(request))
    return response

def post(token, device_name, app_id, model_name, operation, version, command):
    url = "%s/sys/dmjob?token=%s" % (MOAT_REST_API_URI, token)
    job_service_id = "urn:moat:%s:%s:%s:%s" % (app_id, model_name, operation, version)
    device_uid = device.uid(token, device_name)

    data = {
        'name': device_name,
        'activatedAt': time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime()),
        'jobServiceId': job_service_id,
        'expiredAt': 'Mon, 21 Jan 2100 13:05:55 +0000',
        'arguments': {'uid': device_uid, 'command': command}
        }
    data2 = json.dumps(data)
    request = urllib2.Request(url, data2)
    request.get_method = lambda: 'POST'
    request.add_header('Content-Type', 'application/json')
    response = json.load(urllib2.urlopen(request))
    return response



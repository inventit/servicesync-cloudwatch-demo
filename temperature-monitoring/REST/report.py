#!/usr/bin/env python
#
# LEGAL NOTICE
#
# Copyright (C) 2012-2015 InventIt Inc. All rights reserved.
#
# This source code, product and/or document is protected under licenses
# restricting its use, copying, distribution, and decompilation.
# No part of this source code, product or document may be reproduced in
# any form by any means without prior written authorization of InventIt Inc.
# and its licensors, if any.
#
# InventIt Inc.
# 9F KOJIMACHI CP BUILDING
# 4-4-7 Kojimachi, Chiyoda-ku, Tokyo 102-0083
# JAPAN
# http://www.yourinventit.com/

import os
import sys
import json
import datetime
import moat.auth
import moat.device
import moat.modelobject

def usage():
    print 'Usage: ' + os.path.basename(sys.argv[0])

def run(args):
    app_id = os.environ['APP_ID']
    client_id = os.environ['CLIENT_ID']
    client_secert = os.environ['CLIENT_SECERT']
    device_name = os.environ['DEVICE_NAME']
    
    token = moat.auth.token(app_id, client_id, client_secert)
    device_uid = moat.device.uid(token, device_name)
    objects= moat.modelobject.list(token, device_uid, app_id,
                                   'temperature-monitoring', 'SensingData')

    print '"timestampe", "temperature"'
    for row in sorted(objects, key=lambda x:x['timestamp']):
        timestamp = str(datetime.datetime.fromtimestamp(float(row['timestamp']) / 1000))
        temperature = float(row['temperature']) / 1000
        print '"%s", %s' % (timestamp[:-3], temperature)

if __name__ == '__main__':
    run(sys.argv[:])

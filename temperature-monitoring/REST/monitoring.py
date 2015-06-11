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
import moat.auth
import moat.dmjob

def usage():
    print 'Usage: ' + os.path.basename(sys.argv[0]) + ' [ start | stop ]'

def run(args):
    app_id = os.environ['APP_ID']
    client_id = os.environ['CLIENT_ID']
    client_secert = os.environ['CLIENT_SECERT']
    device_name = os.environ['DEVICE_NAME']
    
    try:
        command = args[1]
        if not command in ['start', 'stop']:
            usage()
            return 1
    except IndexError:
        usage()
        return 1

    token = moat.auth.token(app_id, client_id, client_secert)
    response = moat.dmjob.post(token, device_name, app_id,
                               'temperature-monitoring', 'startstop-monitoring', '1.0.0',
                               command)
    print json.dumps(response, sort_keys=True, indent=2)

if __name__ == '__main__':
    run(sys.argv[:])

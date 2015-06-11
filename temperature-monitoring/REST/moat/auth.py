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
import json
import urllib2

MOAT_REST_API_URI = os.environ.get('MOAT_REST_API_URI', 'https://sandbox.service-sync.com/moat/v1')

def token(app_id, client_id, client_secret):
    url = "%s/sys/auth?a=%s&u=%s&c=%s" % (MOAT_REST_API_URI, app_id, client_id, client_secret)
    response = json.load(urllib2.urlopen(url))
    return response['accessToken']

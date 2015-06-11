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

def list(token, device_uid, app_id, package_id, model_name):
    offset = '0'
    objects = []

    while offset != None:
        url = ("%s/sys/device/%s/%s:%s/%s?token=%s&l=25&o=%s" %
               (MOAT_REST_API_URI, device_uid, app_id, package_id, model_name, token, offset))
        response = json.load(urllib2.urlopen(url))
        array = response['array']
        objects.extend(array)
        try:
            offset = response['offset']
        except KeyError:
            offset = None

    return objects

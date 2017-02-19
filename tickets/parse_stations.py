# -*- coding: utf-8 -*-

import re
import requests
import json
from pprint import pprint
from requests.packages.urllib3.exceptions import InsecureRequestWarning

requests.packages.urllib3.disable_warnings(InsecureRequestWarning)

url = 'https://kyfw.12306.cn/otn/resources/js/framework/station_name.js?station_version=1.8971'
response = requests.get(url, verify=False)

stations = re.findall(u'([\u4e00-\u9fa5]+)\|([A-Z]+)\|([a-z]+)', response.text)

station_dict = {}
for station in stations:
    station_dict[station[0]] = station[1]
    station_dict[station[2]] = station[1]

#pprint(station_dict, indent=4)

station_json = json.dumps(station_dict, indent=4, ensure_ascii=False)
file_name = 'stations.json'
with open(file_name, 'w') as file_fd:
    file_fd.write(station_json)

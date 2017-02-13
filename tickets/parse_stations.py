import re
import requests
from pprint import pprint
from requests.packages.urllib3.exceptions import InsecureRequestWarning

requests.packages.urllib3.disable_warnings(InsecureRequestWarning)

#url = 'https://kyfw.12306.cn/otn/resources/js/framework/station_name.js?station_version=1.8971'
#response = requests.get(url, verify=False)

#print(response.text)
#stations = re.findall(u'(\u4e00-\u9fa5)\|([A-Z]+)\|([a-z]+)', response.text)
file_name = 'stations.txt'
with open file_name as file_stations:

stations = re.findall(u'(\u4e00-\u9fa5)\|([A-Z]+)\|([a-z]+)', response.text)
#pprint(dict(stations), indent = 4)
print(stations)

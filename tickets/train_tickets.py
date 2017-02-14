# coding: utf-8

'''命令行火车票查看器

Usage:
    train_tickets [-dgktz] <departure> <destination> <date>

Options:
    -h, --help  查看帮助
    -d          动车
    -g          高铁
    -k          快速
    -t          特快
    -z          直达

Examples:
    train_tickets 上海 北京 2017-02-14
    train_tickets -dg 上海 北京 2017-03-14
'''

import requests, os, json, re
from datetime import datetime
from docopt import docopt
from prettytable import PrettyTable
from colorama import init, Fore
from requests.packages.urllib3.exceptions import InsecureRequestWarning

requests.packages.urllib3.disable_warnings(InsecureRequestWarning)

# colorama initialize
init()

class TrainsCollection():
    header = '车次 车站 时间 历时 商务 特等 一等 二等 高软 软卧 硬卧 软座 硬座 无座'.split()

    def __init__(self, available_trains, options):
        '''查询到的火车班次集合
        :param available_trains: 一个列表， 包含可获得火车班次
        :param options: 查询的选项， 如高铁，动车， etc...
        '''
        self.available_trains = available_trains
        self.options = options

    def _get_duration(self, raw_train):
        duration = raw_train.get('lishi').replace(':', '小时') + '分'
        if duration.startswith('00'):
            return duration[4:]
        elif duration.startswith('0'):
            return duration[1:]
        return duration

    @property
    def trains(self):
        for raw_train in self.available_trains:
            train_info = raw_train['queryLeftNewDTO']
            train_no = train_info['station_train_code']
            initial = train_no[0].lower()
            if not self.options or initial in self.options:
                train = [
                    train_no,
                    '\n'.join([Fore.GREEN + train_info['from_station_name'] + Fore.RESET, 
                              Fore.RED + train_info['to_station_name'] + Fore.RESET]),
                    '\n'.join([Fore.GREEN + train_info['start_time'] + Fore.RESET,
                              Fore.RED + train_info['arrive_time'] + Fore.RESET]),
                    self._get_duration(train_info),
                    train_info['swz_num'], #商务座
                    train_info['tz_num'], #特等座
                    train_info['zy_num'], #一等
                    train_info['ze_num'], #二等
                    train_info['gr_num'], #高软
                    train_info['rw_num'], #软卧
                    train_info['yw_num'], #硬卧
                    train_info['rz_num'], #软座
                    train_info['yz_num'], #硬座
                    train_info['wz_num'], #无座
                ]
                yield train

    def pretty_print(self):
        pt = PrettyTable()
        pt._set_field_names(self.header)
        for train in self.trains:
            pt.add_row(train)
        print(pt)

def get_station_dict():
    if not os.path.exists('stations.json'):
        os.system('/usr/bin/python3 parse_stations.py')
    station_dict = {}
    with open('stations.json', 'r') as stations_fd:
        station_dict = json.load(stations_fd)

    return station_dict

def check_date(date):
    '''Check and return a valid date.
    
    e.g. current year 2017
        input 3-15, 3.15, 3/15, 315, ... return 2017-03-15
        input 2017.03.15, 2017/03/15, ... return 2017-03-15
    '''
    year = str(datetime.today().year)

    result = ''.join(re.findall('\d', date))
    l = len(result)
    # User only input month and day, like 6.1, 0601
    if l in (2, 3, 4):
        result = year + result

    try:
        date = datetime.strptime(result, '%Y%m%d')
    except ValueError:
        print('日期错误')

    # A valid date shoulce within 30 days
    offset = date - datetime.today()
    if offset.days not in range(0, 31):
        raise ValueError('日期错误，超出查询日期')

    return datetime.strftime(date, '%Y-%m-%d')

def cli():
    '''Command-line interface
    '''
    arguments = docopt(__doc__)
    
    date = arguments['<date>']
    date = check_date(date)

    station_dict = get_station_dict()
    
    departure = station_dict.get(arguments['<departure>'].lower())
    destination = station_dict.get(arguments['<destination>'].lower())
    
    #url = 'https://kyfw.12306.cn/otn/leftTicket/queryX?leftTicketDTO.train_date=2017-02-14&leftTicketDTO.from_station=BJP&leftTicketDTO.to_station=QDK&purpose_codes=ADULT'
    url = 'https://kyfw.12306.cn/otn/leftTicket/queryX?leftTicketDTO.train_date={}&leftTicketDTO.from_station={}&leftTicketDTO.to_station={}&purpose_codes=ADULT'.format(date, departure, destination)
    r = requests.get(url, verify=False)
    
    # Used to dump all the availabe tickets to a json file
    #tickets_json = json.dumps(r.json(), indent=4, ensure_ascii=False)
    #with open('tickets.json', 'w') as tickets_fd:
    #    tickets_fd.write(tickets_json)
    
    # Used to load the ticket info from a json file
    #with open('tickets.json', 'r') as tickets_fd:
    #    tickets = json.load(tickets_fd)
    #available_trains = tickets['data']

    available_trains = r.json()['data']

    options = ''.join([
        key for key, value in arguments.items() if value is True
    ])
    TrainsCollection(available_trains, options).pretty_print()

if __name__ == '__main__':
    cli()

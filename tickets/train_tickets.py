# coding: utf-8

'''命令行火车票查看器

Usage:
    train_tickets [-dgktz] <departure> <destination> <data>

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

import requests
from docopt import docopt
from prettytable import PrettyTable
from colorama import init, Fore


url = 'https://kyfw.12306.cn/otn/lcxxcx/query'
?purpose_codes=ADULT&queryDate=2017-02-13&from_station=BJP&to_station=QDK'

def cli():
    '''Command-line interface
    '''
    arguments = docopt(__doc__)
    print(arguments)

if __name__ == '__main__':
    # colorama initialize
    init()
    cli()

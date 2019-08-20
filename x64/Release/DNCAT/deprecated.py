# -*- coding: utf-8 -*-
import multiprocessing
from multiprocessing import freeze_support
import threading
import time
import sqlite3
import os
# import scrapy.spiderloader
# import scrapy.statscollectors
# import scrapy.logformatter
# import scrapy.dupefilters
# import scrapy.squeues
#
# import scrapy.extensions.spiderstate
# import scrapy.extensions.corestats
# import scrapy.extensions.telnet
# import scrapy.extensions.logstats
# import scrapy.extensions.memusage
# import scrapy.extensions.memdebug
# import scrapy.extensions.feedexport
# import scrapy.extensions.closespider
# import scrapy.extensions.debug
# import scrapy.extensions.httpcache
# import scrapy.extensions.statsmailer
# import scrapy.extensions.throttle
#
# import scrapy.core.scheduler
# import scrapy.core.engine
# import scrapy.core.scraper
# import scrapy.core.spidermw
# import scrapy.core.downloader
#
# import scrapy.downloadermiddlewares.stats
# import scrapy.downloadermiddlewares.httpcache
# import scrapy.downloadermiddlewares.cookies
# import scrapy.downloadermiddlewares.useragent
# import scrapy.downloadermiddlewares.httpproxy
# import scrapy.downloadermiddlewares.ajaxcrawl
# import scrapy.downloadermiddlewares.decompression
# import scrapy.downloadermiddlewares.defaultheaders
# import scrapy.downloadermiddlewares.downloadtimeout
# import scrapy.downloadermiddlewares.httpauth
# import scrapy.downloadermiddlewares.httpcompression
# import scrapy.downloadermiddlewares.redirect
# import scrapy.downloadermiddlewares.retry
# import scrapy.downloadermiddlewares.robotstxt
#
# import scrapy.spidermiddlewares.depth
# import scrapy.spidermiddlewares.httperror
# import scrapy.spidermiddlewares.offsite
# import scrapy.spidermiddlewares.referer
# import scrapy.spidermiddlewares.urllength
#
# import scrapy.pipelines
#
# import scrapy.core.downloader.handlers.http
import scrapy.core.downloader.contextfactory

from scrapy.crawler import CrawlerProcess
from scrapy.utils.project import get_project_settings
from multiprocessing import Process
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import random
import requests
import json


class QuotesSpider(scrapy.Spider):
    name = "goldcar"

    def showPercent(self):
        print("")

    def __del__(self):
        self.showPercent()
        # time.sleep(self.minute * 60)

    def __init__(self, shop_name=None, *args, **kwargs):
        super(QuotesSpider, self).__init__(*args, **kwargs)
        self.name = shop_name
        self.sql_name = self.name + ".db"
        self.shop_name = shop_name.lower()
        self.lock = threading.Lock()
        self.start_urls = ['https://www.noon.com/saudi-en/p-14718']
        self.page_index = 1
        out = "抓取数据：" + self.start_urls[0]
        print(out)

    def parse(self, response):
        for quote in response.xpath(".//div[@class='jsx-2127843686 productContainer']"):
            time.sleep(random.randint(0, 1))
            uri = "https://www.noon.com" + str(
                quote.xpath(".//a[@class='jsx-2683212362 product gridView']/@href").extract()[0])
            if uri is not None:
                uri = uri.split('?')[0]
                yield response.follow(uri, callback=self.prase1)

        # 获取下一页的url, （DEL::如果没有就从头开始）
        value = str(response.xpath(
            ".//div[@class='jsx-2341487112 paginationWrapper']//a[@class='nextLink']/@aria-disabled").extract()[0])
        if value is not None and value == "false":
            self.page_index = self.page_index + 1
            next_page = self.start_urls[0] + "?page=" + str(self.page_index)
            yield response.follow(next_page, callback=self.parse)

    def prase1(self, response):
        infos, gold_shop = self.getAllPirce(response)  # 获取所有的价格并以此形式返回{shop_name:[price, rating, fullfilled], ...}
        if gold_shop == "$Rt%6y":
            gold_shop = self.shop_name
        ean = response._get_url().split("/")[-2]  # EAN
        self.solutionNoon(ean, infos, gold_shop)

    def getAllPirce(self, response):
        infos = {}
        gold_shop = "$Rt%6y"
        rows = response.xpath(".//ul[@class='jsx-1312782570 offersList']/li")
        for row in rows:
            price = row.xpath(".//span[@class='jsx-3799960900 sellingPrice']//text()").extract()[0]
            price = round(float(price.strip().split('SAR')[-1]), 2)
            shop_name = row.xpath(".//p[@class='jsx-1312782570']//text()").extract()
            shop_name = str(shop_name[2]).lower()
            if gold_shop == "$Rt%6y":
                gold_shop = shop_name
            ret = row.xpath(".//div[@class='jsx-3304762718 container']")
            is_fbn = False
            if len(ret) > 0:
                is_fbn = True
            rating = 100
            infos[shop_name] = [price, rating, is_fbn]
        if len(infos) == 0:
            price = response.xpath(
                ".//div[@class='jsx-3799960900 pdpPrice']//span[@class='jsx-3799960900 sellingPrice']//text()").extract()[
                0]
            price = round(float(price.strip().split('SAR')[-1]), 2)
            is_fbn = False
            ret = response.xpath(
                ".//div[@class='jsx-2490358733 shippingEstimatorContainer']//div[@class='jsx-3304762718 container']")
            if len(ret) > 0:
                is_fbn = True
            rating = 100
            infos[self.shop_name] = [price, rating, is_fbn]
        return infos, gold_shop

    def getAttr(self, ean):
        attr = {"self_least_price": 0, "minute": 0,
                "max_times": 5, "max_percent": 0.2,
                "percent": 0.1, "lowwer": 0,
                "control": 0, "my_shop": []}

        self.lock.acquire()
        conn = sqlite3.connect("DataBase.db")
        # 本店铺本产品限制的最低价
        try:
            ret = conn.execute("select least_price from 'CPConplexAttr' where ean=? and shop=?;",
                               (ean, self.name)).fetchall()
            if len(ret) > 0:
                attr["self_least_price"] = ret[0][0]
        except:
            attr["self_least_price"] = 0

        # 实时改价参数
        try:
            ret = conn.execute(
                "select minute,max_times,max_percent,percent,lowwer,control,my_shop from 'CPAttr' where shop=?;",
                (self.name,)).fetchall()
            if len(ret) > 0:
                attr["minute"] = ret[0][0]
                attr["max_times"] = ret[0][1]
                attr["max_percent"] = ret[0][2]
                attr["percent"] = ret[0][3]
                attr["lowwer"] = ret[0][4]
                attr["control"] = ret[0][5]
                attr["my_shop"] = ret[0][6].lower().strip().split(",")
        except:
            # print("获取改价参数出错，建议查明原因")
            tmp = 1
        conn.close()
        self.lock.release()
        attr["my_shop"].append(self.shop_name)
        return attr

    def solutionNoon(self, ean, infos, gold_shop):
        attr = self.getAttr(ean)
        out = time.strftime("%Y-%m-%d %H:%M:%S") + " " + ean + " 本店铺[" + str(infos[self.shop_name][0]) + "]\t" + \
              "购物车[" + str(infos[gold_shop][0]) + "][" + gold_shop + "]"
        self.spiderRecord(ean, infos[gold_shop][0], gold_shop)
        if gold_shop in attr["my_shop"]:  # 黄金购物车是自家店铺
            out = "情况A " + out + "\t不修改"
        else:
            if infos[self.shop_name][2] == True:  # 是FBN产品
                diff1 = abs(infos[gold_shop][0] - infos[self.shop_name][0]) / infos[self.shop_name][0]
                if infos[gold_shop][2]:  # 黄金购物车是FBN
                    if diff1 > attr["percent"]:
                        out = "情况B " + out + "\t不修改"
                    else:
                        price = round(min(infos[gold_shop][0], infos[self.shop_name][0]) - attr["lowwer"], 2)
                        if price < attr["self_least_price"]:
                            out = "情况C " + out + "\t不修改"
                        else:
                            self.needToChangePrice(ean, price, gold_shop)
                            out = "情况C " + out + "\t差价比[" + str(round(diff1 * 100, 2)) + "%]\t改价为[" + str(price) + "]"
                else:
                    price = round(infos[self.shop_name][0] - attr["lowwer"], 2)
                    if price < min(infos[gold_shop][0], attr["self_least_price"]):
                        out = "情况D " + out + "\t不修改"
                    else:
                        self.needToChangePrice(ean, price, gold_shop)
                        out = "情况D " + out + "\t改价为[" + str(price) + "]"

            else:
                least_price = 99999
                for info in infos.values():
                    if least_price > info[0]:
                        least_price = info[0]
                diff2 = abs(min(infos[gold_shop][0], least_price) - infos[self.shop_name][0]) / infos[self.shop_name][0]
                if diff2 > attr["percent"]:
                    out = "情况E " + out + "\t最低价[" + str(least_price) + "]\t" + "差价比[" + \
                          str(round(diff2 * 100, 2)) + "%]" + "不修改"
                else:
                    price = round(min(infos[gold_shop][0], least_price) - attr["lowwer"], 2)
                    if price < attr["self_least_price"]:
                        out = "情况F " + out + "\t最低价[" + str(least_price) + "]\t" + "不修改"
                    else:
                        self.needToChangePrice(ean, price, gold_shop)
                        out = "情况F " + out + "\t最低价[" + str(least_price) + "]\t" + "差价比[" + \
                              str(round(diff2 * 100, 2)) + "%]\t改价为[" + str(price) + "]"
        out = "前台：" + out
        print(out)

    '''
    def prase2(self, response):
        # 黄金购物车店铺名
        infos, gold_shop = self.getAllPirce(response)  # 获取所有的价格并以此形式返回{shop_name:[price, rating, fullfilled], ...}
        if gold_shop == "$Rt%6y":
            gold_shop = self.shop_name
        ean = response._get_url().split("/")[-2]  # EAN
        self_least_price = self.getAttr(ean)  # 获取改价参数，并返回此ean限制的最低价
        gold_price = infos[gold_shop][0]  # 黄金购物车价格
        self_price = infos[self.shop_name][0]  # 本店铺的price
        percent = round(((self_price - gold_price + self.lowwer) / self_price), 2)  # 差价比
        first_min_price = 999999  # 最低价
        second_min_price = 999999  # 第二低价
        first_min_shop = []  # 最低价的店铺
        second_min_shop = []  # 第二低价的店铺
        fbs_shop = []  # FBS店铺
        fbs_price = 999999  # FBS店铺中的最低价
        for key, info in infos.items():
            if info[0] < first_min_price:
                second_min_price = first_min_price
                first_min_price = info[0]
        for key, info in infos.items():
            if info[2]:
                fbs_shop.append(key)
            if first_min_price == info[0]:
                first_min_shop.append(key)
            if second_min_price == info[0]:
                second_min_shop.append(key)
        for value in fbs_shop:
            if value != self.shop_name:
                if fbs_price > infos[value][0]:
                    fbs_price = infos[value][0]
        self.solution(gold_shop, ean, self_least_price, infos, gold_price, self_price, percent,
                      first_min_price, second_min_price, first_min_shop, second_min_shop, fbs_shop, fbs_price)

    # 不修改情况：
    #       1、改价店铺是黄金购物车 and 【改价店铺的价格】不是最低价 （有买家提议可以适当考虑提价）
    #       2、自己的其他店铺（不包括改价店铺）是黄金购物车 and 【改价店铺的价格】不比【黄金购物车的价格】低
    #       3、其他店铺是黄金购物车 and 【黄金购物车的价格】比【改价店铺的价格】低 and 【差价比】超过【降价幅度】
    # TODO  4、【改价次数】超过【改价次数上限】 and 【总降价比】超过【总降价比上限】 这个规则放在修改进程中判断
    # 考虑提高价格:
    #       5、改价店铺是黄金购物车 and 【改价店铺的价格】是最低价 --> 要判断是否只有自己一人是最低价 1、只有自己一个人是最低价
    #       6、自己的其他店铺（不包括改价店铺）是黄金购物车 and 【改价店铺的价格】比【黄金购物车的价格】低 --> 改价为【黄金购物车的价格】
    #       7、其他店铺是黄金购物车 and 【黄金购物车的价格】比【改价店铺的价格】高 -->（方法1：在不超过差价比的情况下一直降价   方法2：由人工处理）
    # 考虑降低价格
    # ##要满足的条件##  【改价次数】不超过【改价次数上限】 and 【总降价比】不超过【总降价比上限】
    #       8、其他店铺是黄金购物车 and 【黄金购物车的价格】比【改价店铺的价格】低 and 【差价比】不超过【降价幅度】--> 改价为【黄金购物车的价格 - 降价】

    #

    def solution(self, gold_shop, ean, self_least_price, infos, gold_price, self_price, percent,
                 first_min_price, second_min_price, first_min_shop, second_min_shop, fbs_shop, fbs_price):
        percent = round(percent, 2)

        # 1
        if gold_shop == self.shop_name and self_price == gold_price and self_price > first_min_price:
            self.handler1(ean, self_price, first_min_price, gold_shop)
        # 2
        elif gold_shop != self.shop_name and gold_shop in self.my_shop and self_price >= gold_price:
            self.handler2(ean, self_price, gold_price, gold_shop)
        # 3
        elif gold_shop not in self.my_shop and gold_price < self_price and percent > self.percent:
            self.handler3(ean, self_price, gold_price, gold_shop, percent)
        # 5
        elif gold_shop == self.shop_name and self_price == first_min_price:
            self.handler5(ean, self_price, gold_shop, infos, first_min_price, second_min_price,
                          first_min_shop, second_min_shop, fbs_price)
        # 6
        elif gold_shop != self.shop_name and gold_shop in self.my_shop and self_price < gold_price:
            self.handler6(ean, self_price, gold_price, gold_shop)
        # 7
        elif gold_shop not in self.my_shop and gold_price > self_price:
            self.handler7(ean, self_price, gold_price, gold_shop, infos, self_least_price)
        # 8
        elif gold_shop not in self.my_shop and gold_price < self_price and percent <= self.percent:
            self.handler8(ean, self_price, gold_price, gold_shop, percent, self_least_price)

    def handler1(self, ean, self_price, first_min_price, gold_shop):
        out_str = "#1\t" + ean + ":" + "价格[" + str(self_price) + "]\t最低价[" + str(
            first_min_price) + "]\t本店铺[" + gold_shop + "]"
        print(out_str)

    def handler2(self, ean, self_price, gold_price, gold_shop):
        out_str = "#2\t" + ean + ":" + "价格[" + str(self_price) + "]\t黄金购物车价格[" + str(
            gold_price) + "]\t自家店铺[" + gold_shop + "]"
        print(out_str)

    def handler3(self, ean, self_price, gold_price, gold_shop, percent):
        out_str = "#3\t" + ean + ":" + "价格[" + str(self_price) + "]\t购物车[" + str(
            gold_price) + "]\t其他店铺[" + gold_shop + "]\t差价比[" + str(round(percent*100, 2)) + "%]"
        print(out_str)

    def handler5(self, ean, self_price, gold_shop, infos, first_min_price, second_min_price,
                 first_min_shop, second_min_shop, fbs_price):
        out_str = "#5\t" + ean + ":" + "价格[" + str(self_price) + "]\t最低价[" + str(
            first_min_price) + "]\t本店铺[" + gold_shop + "]\t"
        to_price = 0
        if infos[self.shop_name][2]:  # 是否是FBS
            if len(first_min_shop) == 1:  # 是否只有自己一家是最低价
                # 判断second_min_shop中是否有店铺是fbs
                for value in second_min_shop:
                    if infos[value][2]:
                        to_price = infos[value][0]
                        break
                memo = "改价店铺（FBS）是黄金购物车,且只有改价店铺一家是最低价(" + str(first_min_price) + ")," + \
                       "第二低价格(" + str(second_min_price) + ")中"
                if to_price == 0:
                    memo += "没有FBS店铺"
                    if fbs_price < 999999:
                        memo += ",FBS店铺中的最低价格是(" + str(fbs_price) + ")"
                else:
                    memo += "有FBS店铺"
                    to_price = to_price - self.lowwer
                    if to_price != self_price:
                        out_str += "改价为[" + str(to_price) + "]"
                        self.needToChangePrice(ean, to_price, gold_shop)
                self.sendNotice(ean, memo)
            else:
                flag1 = False
                for value in first_min_shop:
                    if infos[value][2]:
                        flag1 = True
                        break
                memo = "改价店铺（FBS）是黄金购物车,不止改价店铺一家是最低价(" + str(first_min_price) + ")," + \
                       "最低价格中"
                if not flag1:
                    memo += "没有FBS店铺"
                    if fbs_price < 999999:
                        memo += ",FBS店铺中的最低价格是(" + str(fbs_price) + ")"
                else:
                    memo += "有FBS店铺"
                self.sendNotice(ean, memo)
        else:  # 不是FBS
            if len(first_min_shop) == 1:  # 是否只有自己一家是最低价
                # 判断min_second_price_shop中是否有店铺是fbs
                for value in second_min_shop:
                    if infos[value][2]:
                        to_price = infos[value][0]
                        break
                memo = "改价店铺（非FBS）是黄金购物车,且只有改价店铺一家是最低价(" + str(first_min_price) + ")," + \
                       "第二低价格(" + second_min_price + ")中"
                if to_price == 0:
                    memo += "没有FBS店铺"
                    if fbs_price < 999999:
                        memo += ",FBS店铺中的最低价格是(" + str(fbs_price) + ")"
                else:
                    memo += "有FBS店铺"
                    to_price = to_price - self.lowwer
                    if to_price != self_price:
                        out_str += "改价为[" + str(to_price) + "]"
                        self.needToChangePrice(ean, to_price, gold_shop)
                self.sendNotice(ean, memo)
            else:
                flag1 = False
                for value in first_min_shop:
                    if infos[value][2]:
                        flag1 = True
                        break
                memo = "改价店铺（非FBS）是黄金购物车,不止改价店铺一家是最低价(" + str(first_min_price) + ")," + \
                       "最低价格中"
                if not flag1:
                    memo += "没有FBS店铺"
                    if fbs_price < 999999:
                        memo += ",FBS店铺中的最低价格是(" + str(fbs_price) + ")"
                else:
                    memo += "有FBS店铺"
                self.sendNotice(ean, memo)
        print(out_str)

    def handler6(self, ean, self_price, gold_price, gold_shop):
        out_str = "#6\t" + ean + ":" + "价格[" + str(self_price) + "]\t购物车[" + str(gold_price) + \
                  "]\t自家店铺[" + gold_shop + "]\t修改"
        print(out_str)
        self.needToChangePrice(ean, gold_price, gold_shop)

    def handler7(self, ean, self_price, gold_price, gold_shop, infos, self_least_price):
        out_str = "#7\t" + ean + ":" + "价格[" + str(self_price) + "]\t购物车[" + str(
            gold_price) + "]其他店铺[" + gold_shop + "]"
        if (not infos[gold_shop][2]) and infos[self.shop_name][2]:  # 黄金购物车不是FBS 自家店铺是FBS
            memo = "黄金购物车(" + gold_shop + ":" + str(infos[gold_shop][0]) + \
                   ")不是FBS, 改价店铺(" + infos[self.shop_name][0] + ")是FBS"
            self.sendNotice(ean, memo)
        else:
            to_price = self_price - self.lowwer
            if to_price < self_least_price:
                out_str += "改价[" + str(to_price) + "]" + "低于此EAN最低价限制[" + self_least_price + "]"
            else:
                out_str += "改价为[" + str(to_price) + "]"
                self.needToChangePrice(ean, to_price, gold_shop)
        print(out_str)

    def handler8(self, ean, self_price, gold_price, gold_shop, percent, self_least_price):
        out_str = "#8\t" + ean + ":" + "价格[" + str(self_price) + "]\t购物车[" + str(
            gold_price) + "]\t其他店铺[" + gold_shop + "]\t差价比[" + str(round(percent*100, 2)) + "%]\t"
        to_price = gold_price - self.lowwer
        if to_price < self_least_price:
            out_str += "改价[" + str(to_price) + "]" + "低于此EAN最低价限制[" + self_least_price + "]"
        else:
            out_str += "改价为[" + str(to_price) + "]"
            self.needToChangePrice(ean, to_price, gold_shop)
        print(out_str)
    '''

    def spiderRecord(self, ean, price, gold_shop):
        # 将需要修改的添加到item表中
        self.lock.acquire()
        conn = sqlite3.connect(self.sql_name)
        conn.execute("REPLACE INTO 'record'(ean, price, shop) VALUES (?, ?, ?);", (ean, price, gold_shop))
        conn.commit()
        conn.close()
        self.lock.release()

    def needToChangePrice(self, ean, price, gold_shop):
        # 将需要修改的添加到item表中
        self.lock.acquire()
        conn = sqlite3.connect(self.sql_name)
        conn.execute("REPLACE INTO 'item'(ean, price, shop) VALUES (?, ?, ?);", (ean, price, gold_shop))
        conn.commit()
        conn.close()
        self.lock.release()

    def sendNotice(self, ean, memo):
        self.lock.acquire()
        conn = sqlite3.connect(self.sql_name)
        conn.execute("REPLACE INTO 'notice'(ean, memo) VALUES (?, ?);", (ean, memo))
        conn.commit()
        conn.close()
        self.lock.release()


class SpiderProcess(multiprocessing.Process):
    def __init__(self, name):
        multiprocessing.Process.__init__(self)  # 重构了Process类里面的构造函数
        self.name = name

    def run(self):  # 固定用run方法，启动进程自动调用run方法
        print("启动前台抓取任务")
        process = CrawlerProcess(get_project_settings())
        process.crawl(QuotesSpider, shop_name=self.name)
        process.start()
        process.join()
        print("前台抓取数据一轮完成")
        time.sleep(random.randint(30, 70))


class OperateProcess(multiprocessing.Process):
    def __init__(self, name):
        multiprocessing.Process.__init__(self)  # 重构了Process类里面的构造函数
        conn = sqlite3.connect("DataBase.db")
        self.name = name
        ret = conn.execute("select account, password from 'shopInfo' where shop=?;", (name,)).fetchall()
        self.account = ""
        self.password = ""
        if len(ret) > 0:
            self.account = ret[0][0]
            self.password = ret[0][1]
        conn.close()
        self.sql_name = name + ".db"

    def run(self):  # 固定用run方法，启动进程自动调用run方法
        print("启动后台改价任务")
        while 1:
            user_dir = "./" + self.name
            if not os.path.exists(user_dir):
                os.mkdir(user_dir)
            option = webdriver.ChromeOptions()
            option.add_argument("headless")
            # option.add_argument("user-data-dir=" + os.path.abspath(user_dir))
            option.add_argument('--ignore-certificate-errors')
            option.add_argument('log-level=3')
            option.add_argument('lang=zh_CN.UTF-8')
            prefs = {
                'profile.default_content_setting_values': {
                    'images': 2,
                    'stylesheet': 2,
                }
            }
            option.add_experimental_option('prefs', prefs)
            chromePath = "chromedriver.exe"
            self.chrome = webdriver.Chrome(executable_path=chromePath, chrome_options=option)
            self.chrome.maximize_window()
            try:
                self.LoginAccount()
            except:
                raise
                self.chrome.quit()
                continue

    def LoginAccount(self):
        print("后台：登录账户")
        self.chrome.get('https://login.noon.partners/en/')
        try:
            xpath = ".//div[@class='jsx-1240009043 group']"
            WebDriverWait(self.chrome, 30, 0.5).until(EC.presence_of_element_located((By.XPATH, xpath)))
            elemLogin = self.chrome.find_elements_by_xpath(".//div[@class='jsx-1240009043 group']/input")
            elemNewLoginBtn = self.chrome.find_element_by_xpath(
                ".//button[@class='jsx-1789715842 base ripple primary uppercase fullWidth']")
            elemLogin[0].send_keys(self.account)
            elemLogin[1].send_keys(self.password)
            elemNewLoginBtn.click()
        except:
            print("后台：方式1登录失败，尝试方式2")
        i = 0
        while self.chrome.current_url != "https://core.noon.partners/en-sa/":
            time.sleep(1)
            i = i + 1
            if i > 150:
                raise TimeoutError
        while 1:
            try:
                self.NewInventory()
            except:
                raise
                continue

    def NewInventory(self):
        print("后台：打开改价页面")
        self.loginHandler = self.chrome.current_window_handle
        handlers = self.chrome.window_handles
        self.unknownHandler = ""
        for handler in handlers:
            if handler != self.loginHandler:
                self.unknownHandler = handler
                break
        js = 'window.open("https://catalog.noon.partners/en-sa/catalog")'
        self.chrome.execute_script(js)
        handlers = self.chrome.window_handles
        for handler in handlers:
            if handler != self.loginHandler and handler != self.unknownHandler:
                self.inventoryHandler = handler
                break
        while 1:
            try:
                self.OperateProductSelenium()
            except:
                raise
                self.chrome.refresh()
                continue

    def prepareJSON(self, ret_json, partner_sku, price):
        global is_active, json_sale_end, json_sale_start
        json_price = ""
        json_sale_price = ""
        for part in ret_json["psku"]["available_psku"]:
            if part["psku_canonical"] == partner_sku:
                is_active = part["is_active"]
                json_price = str(round(price, 2))
                if "sale_price_sa" not in part and part["sale_price_sa"] is not None:
                    json_sale_price = str(round(price, 2))
                    json_price = str(part["price_sa"])
                    json_sale_end = part["sale_end_sa"]
                    json_sale_start = part["sale_start_sa"]
        json_stock = []
        for stock in ret_json["psku"]["stock"][0]["stock_group"]:
            json_stock.append({
                "id_warehouse": stock["id_warehouse"],
                "quantity": "0",
                "stock_gross": stock["stock_gross"],
                "stock_transferred": stock["stock_transferred"],
                "stock_reserved": stock["stock_reserved"],
                "stock_net": stock["stock_net"],
                "processing_time": str(stock["processing_time"]),
                "stock_updated": False,
                "country_code": stock["country_code"],
                "max_processing_time": stock["max_processing_time"]
            })
        ret_json = {
            "pskus": [{
                "id_warranty": "0",
                "partner_sku": ret_json["psku"]["partner_sku"],
                "sku": ret_json["psku"]["sku"],
                "psku_canonical": ret_json["psku"]["psku_canonical"],
                "is_active": is_active,
                "stocks": json_stock,
                "price": json_price
            }]
        }
        if len(json_sale_price) > 0:
            ret_json["pskus"][0]["sale_price"] = json_sale_price
            ret_json["pskus"][0]["sale_end"] = json_sale_end
            ret_json["pskus"][0]["sale_start"] = json_sale_start

        return json.dumps(ret_json)

    def OperateProductRequests(self):
        print("后台：开始改价")
        selenium_cookies = self.chrome.get_cookies()
        selenium_headers = self.chrome.execute_script("return navigator.userAgent")
        selenium_headers = {
            'User-Agent': selenium_headers,
            "origin": "https://catalog.noon.partners",
            "Content-Type": "application/json",
            "x-locale": "en-sa"
        }
        s = requests.session()
        s.headers.update(selenium_headers)
        for cookie in selenium_cookies:
            short_cookie = {cookie["name"]: cookie["value"]}
            requests.utils.add_dict_to_cookiejar(s.cookies, short_cookie)
        s.verify = "./noon.cer"
        while True:
            time.sleep(random.randint(0, 2))
            conn = sqlite3.connect(self.sql_name)
            ret = conn.execute("SELECT * FROM 'item' LIMIT 1;").fetchall()
            if len(ret) <= 0:
                conn.close()
                continue
            ean = ret[0][0]
            price = ret[0][1]
            out = "后台：" + time.strftime("%Y-%m-%d %H:%M:%S") + " " + ean + " " + str(round(price, 2))
            url = "https://catalog.noon.partners/_svc/clapi-v1/catalog/items?limits=20&page=1&search=" + ean
            r = s.get(url)
            if r.status_code == 200:
                ret_json = json.loads(r.text)
                if ret_json is not None and "items" in ret_json:
                    if len(ret_json["items"]) == 1 and "partner_sku" in ret_json["items"][0]:
                        partner_sku = ret_json["items"][0]["partner_sku"]
                        partner_sku = partner_sku.replace('.', '').replace('-', '')
                        if len(partner_sku) > 0:
                            url = "https://catalog.noon.partners/_svc/clapi-v1/catalog/item/details?psku_canonical=" + partner_sku
                            r = s.get(url)
                            if r.status_code == 200:
                                ret_json = json.loads(r.text)
                                try:
                                    ret_json = self.prepareJSON(ret_json, partner_sku, price)
                                    url = "https://catalog.noon.partners/_svc/clapi-v1/psku"
                                    r = s.post(url, data=ret_json, headers={"Referer": "https://catalog.noon.partners/en-sa/catalog/" + partner_sku})
                                    if r.status_code == 200:
                                        print(out + "\t改价成功")
                                    else:
                                        print(out + "\t改价失败\t[6]")
                                except:
                                    print(out + "\t改价失败\t[5]")
                                    raise
                            else:
                                print(out + "\t改价失败\t[4]")
                        else:
                            print(out + "\t改价失败\t[3]")
                    else:
                        print(out + "\t改价失败\t[2]")
                else:
                    print(out + "\t改价失败\t[1]")
            else:
                print(out + "\t改价失败\t[0]")

            conn.execute("DELETE from 'item' where ean=? and price=?;", (ean, price))
            conn.commit()
            conn.close()
            self.chrome.back()

    def OperateProductSelenium(self):
        print("后台：开始改价")
        url_bak = ""
        while True:
            time.sleep(1)
            conn = sqlite3.connect(self.sql_name)
            ret = conn.execute("SELECT * FROM 'item' LIMIT 1;").fetchall()
            if len(ret) <= 0:
                conn.close()
                continue
            ean = ret[0][0]
            price = ret[0][1]
            out = time.strftime("%Y-%m-%d %H:%M:%S") + " " + ean + " " + str(round(price, 2))
            self.chrome.switch_to.window(self.inventoryHandler)
            try:
                xpath = './/div[@class="jsx-3807287210 searchWrapper"]'
                WebDriverWait(self.chrome, 60, 0.5).until(EC.presence_of_element_located((By.XPATH, xpath)))
                elemSearch = self.chrome.find_element_by_xpath('.//div[@class="jsx-3807287210 searchWrapper"]//input')
                elemSearch.clear()
                elemSearch.send_keys(ean)
                while 1:
                    time.sleep(1)
                    elemProduct = self.chrome.find_elements_by_xpath(
                        ".//div['jsx-448933760 ctr']/table/tbody/tr[1]/td[1]//a")
                    if len(elemProduct) == 1:
                        product_url = str(elemProduct[0].get_attribute("href"))
                        if product_url != url_bak:
                            url_bak = product_url
                            break
                    elemProduct = self.chrome.find_elements_by_xpath(
                        ".//table[@class='jsx-3498568516 table']//td[@class='jsx-3498568516 td']"
                        "//div[@class='jsx-3793681198 text']")
                    if len(elemProduct) == 1:
                        product_url = ""
                        url_bak = ""
                        break
            except:
                conn.close()
                raise
            if product_url == "":
                print("后台：" + out + "\t没找到这个产品")
                conn.close()
                continue
            if len(elemProduct) != 1:
                print("后台：" + out + "\t没找到这个产品")
                conn.close()
                continue
            self.chrome.execute_script("arguments[0].click()", elemProduct[0])
            # self.chrome.switch_to.window(self.loginHandler)
            # js = 'window.location.replace("' + product_url + '")'
            # self.chrome.execute_script(js)
            try:
                xpath = ".//div[@class='jsx-509839755 priceInputWrapper']//input[@name='sale_price_sa']"
                WebDriverWait(self.chrome, 40, 0.5).until(EC.presence_of_element_located((By.XPATH, xpath)))
                elemInput = self.chrome.find_element_by_xpath(xpath)
                value = elemInput.get_attribute("value")
                if value is None or value == "value" or len(value) == 0:
                    xpath = ".//div[@class='jsx-509839755 priceInputWrapper']//input[@name='price_sa']"
                    elemInput = self.chrome.find_element_by_xpath(xpath)
                elemInput.clear()
                elemInput.send_keys(str(price))
                xpath = ".//div[@class='jsx-509839755 fixedBottom']/button"
                WebDriverWait(self.chrome, 20, 0.5).until(EC.presence_of_element_located((By.XPATH, xpath)))
                elemBtn = self.chrome.find_element_by_xpath(xpath)
                self.chrome.execute_script("arguments[0].click()", elemBtn)
                print("后台：" + out + "\t改价成功")
                # time.sleep(10)
            except:
                print("后台：" + out + "\t改价失败")
            conn.execute("DELETE from 'item' where ean=? and price=?;", (ean, price))
            conn.commit()
            conn.close()
            self.chrome.back()

    def checkPage(self, driver):
        checkPageFinishScript = "try {if (document.readyState !== 'complete') {return false;} if (window.jQuery) { if (" \
                                "window.jQuery.active) { return false; } else if (window.jQuery.ajax && " \
                                "window.jQuery.ajax.active) { return false; } } if (window.angular) { if (!window.qa) { " \
                                "window.qa = {doneRendering: false }; } var injector = window.angular.element(" \
                                "'body').injector(); var $rootScope = injector.get('$rootScope'); var $http = " \
                                "injector.get('$http'); var $timeout = injector.get('$timeout'); if ($rootScope.$$phase " \
                                "=== '$apply' || $rootScope.$$phase === '$digest' || $http.pendingRequests.length !== 0) " \
                                "{ window.qa.doneRendering = false; return false; } if (!window.qa.doneRendering) { " \
                                "$timeout(function() { window.qa.doneRendering = true;}, 0); return false;}} return " \
                                "true;} catch (ex) {return false;} "
        return driver.execute_script(checkPageFinishScript)


def pre_test(name):
    sql_name = name + ".db"
    conn = sqlite3.connect(sql_name)
    c = conn.cursor()
    c.execute("DROP TABLE IF EXISTS 'item';")
    c.execute('''CREATE TABLE 'item'
                       (ean       TEXT    PRIMARY KEY   NOT NULL,
                        price     DOUBLE  NOT NULL,
                        shop      TEXT    NOT NULL);''')
    c.execute("DROP TABLE IF EXISTS 'notice';")
    c.execute('''CREATE TABLE 'notice'
                               (ean    TEXT    PRIMARY KEY   NOT NULL,
                                memo   TEXT);''')
    c.execute("DROP TABLE IF EXISTS 'record';")
    c.execute('''CREATE TABLE 'record'
                           (ean       TEXT    PRIMARY KEY   NOT NULL,
                            price     DOUBLE  NOT NULL,
                            shop      TEXT    NOT NULL);''')
    conn.commit()
    conn.close()


def skr(name):
    # 初始化数据库
    if __name__ == '__main__':
        freeze_support()
        os.chdir(os.path.split(os.path.realpath(__file__))[0])
        pre_test(name)
        p1 = OperateProcess(name)
        p1.start()
        while True:
            p = SpiderProcess(name=name)
            p.start()
            p.join()
            return


skr("BuyMore")
# 'untreated' ean reason
# 'CPComplexAttr' shop ean least_price max_times
# 'CPAttr' shop minute max_times max_percent percent lowwer control

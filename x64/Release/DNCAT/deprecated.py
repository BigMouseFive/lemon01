# -*- coding: utf-8 -*-
import multiprocessing
from multiprocessing import freeze_support
import threading
import time
import sqlite3
import os
import scrapy.spiderloader
import scrapy.statscollectors
import scrapy.logformatter
import scrapy.dupefilters
import scrapy.squeues
 
import scrapy.extensions.spiderstate
import scrapy.extensions.corestats
import scrapy.extensions.telnet
import scrapy.extensions.logstats
import scrapy.extensions.memusage
import scrapy.extensions.memdebug
import scrapy.extensions.feedexport
import scrapy.extensions.closespider
import scrapy.extensions.debug
import scrapy.extensions.httpcache
import scrapy.extensions.statsmailer
import scrapy.extensions.throttle
 
import scrapy.core.scheduler
import scrapy.core.engine
import scrapy.core.scraper
import scrapy.core.spidermw
import scrapy.core.downloader
 
import scrapy.downloadermiddlewares.stats
import scrapy.downloadermiddlewares.httpcache
import scrapy.downloadermiddlewares.cookies
import scrapy.downloadermiddlewares.useragent
import scrapy.downloadermiddlewares.httpproxy
import scrapy.downloadermiddlewares.ajaxcrawl
import scrapy.downloadermiddlewares.decompression
import scrapy.downloadermiddlewares.defaultheaders
import scrapy.downloadermiddlewares.downloadtimeout
import scrapy.downloadermiddlewares.httpauth
import scrapy.downloadermiddlewares.httpcompression
import scrapy.downloadermiddlewares.redirect
import scrapy.downloadermiddlewares.retry
import scrapy.downloadermiddlewares.robotstxt
 
import scrapy.spidermiddlewares.depth
import scrapy.spidermiddlewares.httperror
import scrapy.spidermiddlewares.offsite
import scrapy.spidermiddlewares.referer
import scrapy.spidermiddlewares.urllength
 
import scrapy.pipelines
 
import scrapy.core.downloader.handlers.http
import scrapy.core.downloader.contextfactory

from scrapy.crawler import CrawlerProcess
from scrapy.utils.project import get_project_settings
from multiprocessing import Process
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC


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
        self.my_shop = []
        self.lock = threading.Lock()
        self.minute = 0
        self.max_times = 0
        self.max_percent = 0
        self.percent = 0
        self.lowwer = 0
        self.control = 0
        self.firstInitAttr = False
        self.start_urls = ['https://uae.souq.com/ae-en/' + self.shop_name + '/p/?section=2&page=1']
        out = "抓取数据：" + self.start_urls[0]
        print(out)

    def parse(self, response):
        for quote in response.xpath(".//div[@class='column column-block block-grid-large single-item']"):
            # 对fbs的产品不处理
            fbs = quote.xpath(".//div[@class='flag flag-fbs']")
            if len(fbs) > 0:
                continue
            data_id = quote.xpath(".//a[@class='img-link quickViewAction sPrimaryLink']/@data-id").extract()[0] + "/u/"
            data_img = str(quote.xpath(".//a[@class='img-link quickViewAction sPrimaryLink']/@data-img").extract()[0]). \
                           split("item_L_")[-1].split("_")[0] + "/i/?ctype=dsrch"
            uri = str(quote.xpath(".//a[@class='img-link quickViewAction sPrimaryLink']/@href").extract()[0]). \
                replace(data_id, data_img)
            if uri is not None:
                yield response.follow(uri, callback=self.prase1)

        # 获取下一页的url, （DEL::如果没有就从头开始）
        next_page = response.xpath(".//li[@class='pagination-next goToPage']/a/@href").extract()
        if next_page is not None and len(next_page) > 0:
            next_page = next_page[0].replace("page=", "section=2&page=")
            yield response.follow(next_page, callback=self.parse)

    def prase1(self, response):
        # 黄金购物车店铺名
        gold_shop = response.xpath('.//span[@class="unit-seller-link"]/a/b//text()').extract()[0]
        gold_shop = gold_shop.lower()
        # 产品的ean
        ean = response.xpath('.//div[@id="productTrackingParams"]/@data-ean').extract()[0]

        next_page = response.xpath(".//a[@class='show-for-medium bold-text']/@href").extract()
        if next_page is not None and len(next_page) > 0:
            yield response.follow(next_page[0], callback=self.prase2, meta={'ean': ean, 'gold_shop': gold_shop})

    def prase2(self, response):
        gold_shop = response.meta["gold_shop"]  # 黄金购物车店铺名
        ean = response.meta["ean"]  # EAN
        self_least_price = self.refreshAttr(ean)  # 获取改价参数，并返回此ean限制的最低价
        infos = self.getAllPirce(response)  # 获取所有的价格并以此形式返回{shop_name:[price, rating, fullfilled], ...}
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

    def refreshAttr(self, ean):
        self_least_price = 0
        self.lock.acquire()
        conn = sqlite3.connect("../../../DataBase.db")
        # 本店铺本产品限制的最低价
        try:
            ret = conn.execute("select least_price from 'CPConplexAttr' where ean=? and shop=?;",
                               (ean, self.name)).fetchall()
            if len(ret) > 0:
                self_least_price = ret[0][0]
        except:
            self_least_price = 0

        # 实时改价参数
        try:
            ret = conn.execute("select minute,max_times,max_percent,percent,lowwer,control,my_shop from 'CPAttr' where shop=?;",
                               (self.name,)).fetchall()
            if len(ret) > 0:
                self.control = ret[0][5]
                self.firstInitAttr = True
                if self.minute != ret[0][0] or self.max_times != ret[0][1] or self.max_percent != ret[0][2] or \
                        self.percent != ret[0][3] or self.lowwer != ret[0][4]:
                    self.minute = ret[0][0]
                    self.max_times = ret[0][1]
                    self.max_percent = ret[0][2]
                    self.percent = ret[0][3]
                    self.lowwer = ret[0][4]
                    self.my_shop = ret[0][6].lower().strip().split(",")
                    out_str = "改价参数：降价[" + str(self.lowwer) + "]\t降价比[" + str(self.percent * 100) + "%]\t降价次数[" \
                              + str(self.max_times) + "]\t最大降价比[" + str(self.max_percent * 100) + "%]\t自己的店铺[" + str(ret[0][6]) + "]"
                    print(out_str)


        except:
            if not self.firstInitAttr:
                self.firstInitAttr = True
                self.control = 0
                if self.minute != 0 or self.max_times != 5 or self.max_percent != 0.2 or self.percent != 0.1 or self.lowwer != 0:
                    self.minute = 0
                    self.max_times = 5
                    self.max_percent = 0.2
                    self.percent = 0.1
                    self.lowwer = 0
                    self.my_shop = []
                    out_str = "改价参数：降价[" + str(self.lowwer) + "]\t降价比[" + str(self.percent * 100) + "%]\t降价次数[" \
                          + str(self.max_times) + "]\t最大降价比[" + str(self.max_percent * 100) + "%]\t自己的店铺[]"
                    print(out_str)
        conn.close()
        self.lock.release()
        return self_least_price

    def getAllPirce(self, response):
        infos = {}
        rows = response.xpath(".//div[@id='condition-all']/div[@class='row']")
        for row in rows:
            price = row.xpath(".//div[@class='field price-field']//text()").extract()[0]
            price = round(float(price.strip().split('\n')[-1].split("AED")[0]), 2)
            shop_name = row.xpath(".//div[@class='field seller-name']//a//text()").extract()[0].lower()
            ret = row.xpath(".//div[@class='field clearfix labels']//div[@class='fullfilled']")
            fullfilled = False
            rating = 100
            if ret:
                fullfilled = True
            else:
                rating = row.xpath(".//div[@class='field seller-rating']//a//text()").extract()
                if rating:
                    rating = round(float(rating[0].split('%')[0].split("(")[-1]), 2)
                else:
                    rating = 0  # no rating yet
            infos[shop_name] = [price, rating, fullfilled]
        return infos

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
            gold_price) + "]\t其他店铺[" + gold_shop + "]\t差价比[" + str(percent*100) + "%]"
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
        out_str = "#6\t" + ean + ":" + "价格[" + str(self_price) + "]\t购物车[" + str(
            gold_price) + "]\t自家店铺[" + gold_shop + "]\t修改"
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
                out_str += "改价为[" + to_price + "]"
                self.needToChangePrice(ean, to_price, gold_shop)
        print(out_str)

    def handler8(self, ean, self_price, gold_price, gold_shop, percent, self_least_price):
        out_str = "#8\t" + ean + ":" + "价格[" + str(self_price) + "]\t购物车[" + str(
            gold_price) + "]\t其他店铺[" + gold_shop + "]\t差价比[" + str(percent*100) + "%]\t"
        to_price = self_price - self.lowwer
        if to_price < self_least_price:
            out_str += "改价[" + str(to_price) + "]" + "低于此EAN最低价限制[" + self_least_price + "]"
        else:
            out_str += "改价为[" + to_price + "]"
            self.needToChangePrice(ean, to_price, gold_shop)
        print(out_str)

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
        print("start SpiderProcess")
        process = CrawlerProcess(get_project_settings())
        process.crawl(QuotesSpider, shop_name=self.name)
        process.start()
        process.join()
        print("抓取数据一轮完成")
        time.sleep(2)


class OperateProcess(multiprocessing.Process):
    def __init__(self, name):
        multiprocessing.Process.__init__(self)  # 重构了Process类里面的构造函数
        conn = sqlite3.connect("../../../DataBase.db")
        ret = conn.execute("select account, password from 'shopInfo' where shop=?;", (name,)).fetchall()
        self.account = ""
        self.password = ""
        if len(ret) > 0:
            self.account = ret[0][0]
            self.password = ret[0][1]
        conn.close()
        self.sql_name = name + ".db"

    def run(self):  # 固定用run方法，启动进程自动调用run方法
        print("start OperateProcess")
        while 1:
            option = webdriver.ChromeOptions()
            # option.add_argument("headless")
            option.add_argument('--ignore-certificate-errors')
            option.add_argument('log-level=3')
            option.add_argument('lang=zh_CN.UTF-8')
            prefs = {
                'profile.default_content_setting_values': {
                    'images': 2,
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
        print("登录账户")
        self.chrome.get('https://uae.souq.com/ae-en/login.php')
        elemNewAccount = self.chrome.find_element_by_id("email")
        elemNewLoginBtn = self.chrome.find_element_by_id("siteLogin")
        elemNewAccount.send_keys(self.account)
        elemNewLoginBtn.click()
        try:
            cssSelectText = "#continue"
            WebDriverWait(self.chrome, 10, 0.5).until(EC.presence_of_element_located((By.CSS_SELECTOR, cssSelectText)))
            elemContinue = self.chrome.find_element_by_id("continue")
            elemContinue.click()
            cssSelectText = "#ap_password"
            WebDriverWait(self.chrome, 20, 0.5).until(EC.presence_of_element_located((By.CSS_SELECTOR, cssSelectText)))
            elemPassword = self.chrome.find_element_by_id("ap_password")
            elemLoginBtn = self.chrome.find_element_by_id("signInSubmit")
            elemPassword.send_keys(self.password)
            elemLoginBtn.click()
        except:
            cssSelectText = "#password"
            WebDriverWait(self.chrome, 20, 0.5).until(EC.presence_of_element_located((By.CSS_SELECTOR, cssSelectText)))
            elemPassword = self.chrome.find_element_by_id("password")
            elemLoginBtn = self.chrome.find_element_by_id("siteLogin")
            elemPassword.send_keys(self.password)
            elemLoginBtn.click()

        cssSelectText = "#search_box"
        WebDriverWait(self.chrome, 20, 0.5).until(EC.presence_of_element_located((By.CSS_SELECTOR, cssSelectText)))
        while 1:
            try:
                self.NewInventory()
            except:
                raise
                continue

    def NewInventory(self):
        print("打开改价页面")
        self.loginHandler = self.chrome.current_window_handle
        handlers = self.chrome.window_handles
        self.unknownHandler = ""
        for handler in handlers:
            if handler != self.loginHandler:
                self.unknownHandler = handler
                break
        js = 'window.open("https://sell.souq.com/inventory/inventory-management?tab=live")'
        self.chrome.execute_script(js)
        handlers = self.chrome.window_handles
        for handler in handlers:
            if handler != self.loginHandler and handler != self.unknownHandler:
                self.inventoryHandler = handler
                break
        while 1:
            try:
                self.chrome.switch_to.window(self.inventoryHandler)
                self.chrome.refresh()
                # 循环处理每一页
                self.OperateProduct()
            except:
                raise
                # time.sleep(minute * 60)
                continue

    def OperateProduct(self):
        # 从数据库中获取
        time.sleep(10)
        while True:
            conn = sqlite3.connect(self.sql_name)
            ret = conn.execute("SELECT * FROM 'item' LIMIT 1;").fetchall()
            if len(ret) <= 0:
                conn.close()
                time.sleep(1)
                continue
            ean = ret[0][0]
            price = ret[0][1]
            string = ean + str(price)
            print(string)
            try:
                xpathText = '//*[@id="main"]/section/div[1]/div[1]/div[3]/div[1]/div[1]/div/form/fieldset/div/div[1]/ul/li[2]/input'
                WebDriverWait(self.chrome, 20, 0.5).until(EC.presence_of_element_located((By.XPATH, xpathText)))
                elemSearch = self.chrome.find_element_by_xpath(xpathText)
                elemSearch.clear()
                elemSearch.send_keys(ean)
                xpathText = ".//a[@class='button postfix']"
                WebDriverWait(self.chrome, 20, 0.5).until(EC.presence_of_element_located((By.XPATH, xpathText)))
                elemClick = self.chrome.find_element_by_xpath(xpathText)
                self.chrome.execute_script("arguments[0].click()", elemClick)
            except:
                conn.close()
                raise

            try:
                WebDriverWait(self.chrome, 20, 0.5).until(self.checkPage)
                cssText = "#table-inventory>tbody>tr>td:nth-child(4)"
                WebDriverWait(self.chrome, 20, 0.5).until(EC.presence_of_element_located((By.CSS_SELECTOR, cssText)))
                elemPrice = self.chrome.find_element_by_css_selector(cssText)
                self.chrome.execute_script("arguments[0].click()", elemPrice)
                elemInput = elemPrice.find_element_by_css_selector("form:first-child sc-dynamic-input input")
                elemBtn = elemPrice.find_element_by_css_selector("form:first-child sc-dynamic-input+a")
                elemInput.clear()
                elemInput.send_keys(str(price))
                self.chrome.execute_script("arguments[0].click()", elemBtn)
            except:
                print("修改失败")
                conn.close()
                time.sleep(1)
                continue
            conn.execute("DELETE from 'item' where ean=? and price=?;", (ean, price))
            conn.commit()
            conn.close()
            print("修改成功")
            time.sleep(1)

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
            time.sleep(2)
        

skr("readygo")
# 'untreated' ean reason
# 'CPComplexAttr' shop ean least_price max_times
# 'CPAttr' shop minute max_times max_percent percent lowwer control

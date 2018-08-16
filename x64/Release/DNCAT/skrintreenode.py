# -*- coding: utf-8 -*-
from selenium import webdriver
from selenium.common import exceptions
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.wait import WebDriverWait
import re
import time
import os


def mkdir(path):
    path = path.strip()
    path = path.rstrip("\\")
    isExists = os.path.exists(path)
    if not isExists:
        os.makedirs(path)
        return True
    else:
        return False


def LoginAccount(chrome, loginUri, account, password, percent, lowwer, currentDir):
    # 获取email 和 password 控件
    print("登录账户")
    chrome.get(loginUri)
    elemAccount = chrome.find_element_by_name("email")
    elemPassword = chrome.find_element_by_name("password")
    elemLoginBtn = chrome.find_element_by_id("siteLogin")
    elemAccount.send_keys(account)
    elemPassword.send_keys(password)
    elemLoginBtn.click()
    cssSelectText = "footer.wide-screen div.first-row div div:nth-child(3) ul:nth-child(2) li:last-child a"
    WebDriverWait(chrome, 20, 0.5).until(EC.presence_of_element_located((By.CSS_SELECTOR, cssSelectText)))

    while 1:
        try:
            NewInventory(chrome, percent, lowwer, currentDir, account)
        except:
            continue


def NewInventory(chrome, percent, lowwer, currentDir, account):
    print("打开改价页面")
    global inventoryHandler
    path = currentDir + "\\data\\" + account
    mkdir(path)
    filepath = path + "\\UpdateRecord.txt"
    try:
        record = open(filepath, "a")
    except:
        record = open(filepath, "x")
    filepath = path + "\\Attention.txt"
    try:
        attention = open(filepath, "a")
    except:
        attention = open(filepath, "x")
    loginHandler = chrome.current_window_handle
    readyUri = "https://sell.souq.com/inventory/inventory-management?tab=live"
    js = 'window.open("' + readyUri + '")'
    time.sleep(2)
    chrome.execute_script(js)
    handlers = chrome.window_handles
    for handler in handlers:
        # print(handler)
        if handler != loginHandler:
            inventoryHandler = handler
            break
    while 1:
        chrome.switch_to_window(inventoryHandler)
        chrome.refresh()
        # 循环处理每一页
        print("循环处理每一页")
        index = 0
        pageCount = 1  # 起始页为第一页
        try:
            OperateProduct(chrome, record, attention, index, pageCount, percent, lowwer)
        except:
            continue


def OperateProduct(chrome, record, attention, index, pageCount, percent, lowwer):
    global elemLeastDiv, elemSelf, elemLeast, elem

    def checkPage(driver):
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
    while 1:
        # 处理当前这一页
        # time.sleep(10)
        WebDriverWait(chrome, 50, 0.5).until(checkPage)
        cssText = "table#table-inventory tbody"
        try:
            WebDriverWait(chrome, 20, 0.5).until(EC.presence_of_element_located((By.CSS_SELECTOR, cssText)))
        except exceptions.TimeoutException:
            chrome.refresh()
            pageCount = 1
            continue
        elemT = chrome.find_element_by_css_selector(cssText)
        elems = elemT.find_elements_by_tag_name("tr")
        rowCount = 0
        for elemIter in elems:
            rowCount = rowCount + 1
            elem = elemIter.find_element_by_css_selector("td:nth-child(4)")
            elemSelf = elem.find_element_by_css_selector("div:first-child span")
            elemLeastDiv = elem.find_element_by_css_selector("div:first-child span+div")
            elemLeast = elem.find_element_by_css_selector("div:first-child span+div i")
            elemLeastDiv.is_displayed()
            elemLeast.get_attribute("popover")

            if not elemLeastDiv.is_displayed():
                continue
            currentPrice = float(elemSelf.text)
            ret = re.findall(r"\d+\.?\d*", elemLeast.get_attribute("popover"))
            if len(ret) <= 0:
                continue
            else:
                leastPrice = float(ret[0])
            subPrice = float(currentPrice - leastPrice)
            divPrice = float(subPrice / currentPrice)
            preleast = leastPrice;
            leastPrice = round(leastPrice - lowwer, 2)
            if leastPrice >= currentPrice or leastPrice == 0 or currentPrice == 999:
                continue

            if divPrice < percent:
                # 修改数据
                chrome.execute_script("arguments[0].click()", elem)
                try:
                    elemInput = elem.find_element_by_css_selector("form:first-child sc-dynamic-input input")
                    elemBtn = elem.find_element_by_css_selector("form:first-child sc-dynamic-input+a")
                    elemInput.clear()
                    elemInput.send_keys(str(leastPrice))
                    chrome.execute_script("arguments[0].click()", elemBtn)
                except:
                    continue
            persentPrice = divPrice * 100
            out = "第" + str(pageCount) + "页, " + "第" + str(rowCount) + "项\t" + "\t原价:" + str(currentPrice) \
                  + "最低价:" + str(preleast) + "\t差价比:" + str(round(persentPrice, 2)) + "%\t修改后:" + str(leastPrice)
            if divPrice >= percent:
                out = "第" + str(pageCount) + "页, " + "第" + str(rowCount) + "项\t" + "\t原价:" + str(currentPrice) \
                  + "最低价:" + str(preleast) + "\t差价比:" + str(round(persentPrice, 2)) + "%\t未修改"
            print(out)
            elemProduct = elemIter.find_element_by_css_selector("td:nth-child(3) span")
            out = str(pageCount) + "\t" + str(rowCount) + "\t" + str(currentPrice) + "\t" + str(preleast) + '\t' + \
                  str(leastPrice) + "\t" + str(round(subPrice, 2)) + "\t" + str(round(persentPrice, 2)) + "%\t" + elemProduct.text
            if divPrice >= percent:
                out = out + "\t未修改"
            out = out + "\r\n"
            if divPrice >= percent:
                attention.write(out)
                attention.flush()
            else:
                record.write(out)
                record.flush()
        # 点击上一页/下一页
        cssText = "div#main section:first-child div#liveId div:nth-child(4) div.group"
        try:
            WebDriverWait(chrome, 20, 0.5).until(EC.presence_of_element_located((By.CSS_SELECTOR, cssText)))
        except exceptions.TimeoutException:
            chrome.refresh()
            pageCount = 1
            continue
        elemF = chrome.find_element_by_css_selector(cssText)
        elem0 = elemF.find_element_by_css_selector("a:first-child")
        elem1 = elemF.find_element_by_css_selector("a:last-child")
        elemChoose = elem1
        if index == 0:
            elemChoose = elem0
        if elemChoose.get_attribute("class") == "button-disabled":
            index = 1 - index
        if index == 0:
            chrome.execute_script("arguments[0].click()", elem0)
            pageCount = pageCount - 1
        else:
            chrome.execute_script("arguments[0].click()", elem1)
            pageCount = pageCount + 1


def skr(account, password, currentDir, percent0 = 0.01, lowwer0 = 0):
    if not any(account) or not any(password):
        exit(0)
    percent = round(float(percent0), 2)
    lowwer = round(float(lowwer0), 2)
    print("----------------------")
    print("账  号\t", account)
    print("百分比\t", percent * 100)
    print("降  价\t", lowwer)
    print("----------------------")
    while 1:
        option = webdriver.ChromeOptions()
        option.add_argument("headless")
        option.add_argument('--ignore-certificate-errors')
        option.add_argument(
            'user-agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko)'
            ' Chrome/67.0.3396.99 Safari/537.36"')
        option.add_argument('log-level=3')
        option.add_argument('lang=zh_CN.UTF-8')
        chromePath = currentDir + "\\chromedriver.exe"
        chrome = webdriver.Chrome(executable_path=chromePath, chrome_options=option)
        chrome.maximize_window()
        try:
            LoginAccount(chrome,
                         'https://uae.souq.com/ae-en/login.php',
                         account,
                         password,
                         percent,
                         lowwer,
                         currentDir)
        except:
            chrome.close()
            continue
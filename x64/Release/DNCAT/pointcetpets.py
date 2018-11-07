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


def LoginAccount(chrome, loginUri, account, password, percent, lowwer, currentDir, dictean, maxtimes, maxtprice, minute):
    # 获取email 和 password 控件
    print("登录账户")
    chrome.get(loginUri)
    elemAccount = chrome.find_element_by_name("email")
    elemPassword = chrome.find_element_by_name("password")
    elemLoginBtn = chrome.find_element_by_id("siteLogin")
    elemAccount.send_keys(account)
    elemPassword.send_keys(password)
    elemLoginBtn.click()
    cssSelectText = "#search_box"
    WebDriverWait(chrome, 20, 0.5).until(EC.presence_of_element_located((By.CSS_SELECTOR, cssSelectText)))
    while 1:
        try:
            NewInventory(chrome, percent, lowwer, currentDir, account, dictean, maxtimes, maxtprice, minute)
        except:
            continue


def NewInventory(chrome, percent, lowwer, currentDir, account, dictean, maxtimes, maxtprice, minute):
    print("打开改价页面")
    global inventoryHandler, unknownHandler, searchHandler
    path = currentDir + "\\data\\" + account
    mkdir(path)
    filepath = path + "\\UpdateRecord.txt"
    try:
        record = open(filepath, "a", encoding='utf-8')
    except:
        record = open(filepath, "x", encoding='utf-8')
    filepath = path + "\\Attention.txt"
    try:
        attention = open(filepath, "a", encoding='utf-8')
    except:
        attention = open(filepath, "x", encoding='utf-8')
    loginHandler = chrome.current_window_handle
    handlers = chrome.window_handles
    unknownHandler = ""
    for handler in handlers:
        if handler != loginHandler:
            unknownHandler = handler
            break
    readyUri = "https://sell.souq.com/inventory/inventory-management?tab=live"
    js = 'window.open("' + readyUri + '")'
    chrome.execute_script(js)
    handlers = chrome.window_handles
    for handler in handlers:
        if handler != loginHandler and handler != unknownHandler:
            inventoryHandler = handler
            break
    while 1:
        try:
            chrome.switch_to_window(inventoryHandler)
            chrome.refresh()
            # 循环处理每一页
            print("循环处理每一页")
            index = 0
            pageCount = 1  # 起始页为第一页
            OperateProduct(chrome, record, attention, index, pageCount, percent, lowwer, dictean, maxtimes, maxtprice, minute)
        except:
            time.sleep(minute*60)
            continue


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


def OperateProduct(chrome, record, attention, index, pageCount, percent, lowwer, dictean, maxtimes, maxtprice, minute):
    global elemLeastDiv, elemSelf, elemLeast, elem
    xpathText = '//*[@id="liveId"]/div[1]/div[1]/div/ng-include/div/div/div[3]/select/option[5]'
    WebDriverWait(chrome, 40, 0.5).until(EC.presence_of_element_located((By.XPATH, xpathText)))
    chrome.find_element_by_xpath(xpathText).click()
    while 1:
        # 处理当前这一页
        showPage = "处理第" + str(pageCount) + "页"
        print(showPage)
        time.sleep(1)
        WebDriverWait(chrome, 60, 0.5).until(checkPage)
        cssText = "table#table-inventory tbody>tr"
        elems = chrome.find_elements_by_css_selector(cssText)
        rowCount = 0
        elemsize = len(elems)
        for i in range(elemsize):
            rowCount = rowCount + 1
            elemIter = elems[i]
            elem = elemIter.find_element_by_css_selector("td:nth-child(4)")
            elemSelf = elem.find_element_by_css_selector("div:first-child span")
            elemLeastDiv = elem.find_element_by_css_selector("div:first-child span+div")
            elemLeast = elem.find_element_by_css_selector("div:first-child span+div i")
            elemProductName = elemIter.find_element_by_css_selector("td:nth-child(3) span")
            strean = elemProductName.text
            try:
                currentPrice = float(elemSelf.text)
            except:
                continue
            #
            #
            # 获取最低价格
            if not elemLeastDiv.is_displayed():
                leastPrice = currentPrice
            else:
                ret = re.findall(r"\d+\.?\d*", elemLeast.get_attribute("popover"))
                if len(ret) <= 0:
                    continue
                else:
                    leastPrice = round(float(ret[0]), 2)
            if leastPrice == 0:
                continue
            subPrice = round(float(currentPrice - leastPrice), 2)
            divPrice = round(float(subPrice / currentPrice), 4)
            leastPrice = round(float(leastPrice - lowwer), 2)
            # 获取最低价格
            #
            #
            if leastPrice == currentPrice or leastPrice == 0 or currentPrice == 999:
                continue
            #
            #                        修改次数       原价          总差价比        当前价格
            #   dictean:  {"ean0": {"ts": num, "fp": fprice, "tp": tpric, "lprice": lprice},
            flag = True
            if strean in dictean:
                if dictean[strean]["times"] > maxtimes or dictean[strean]["tprice"] > maxtprice:
                    flag = False
            if divPrice <= percent and flag:
                # 修改数据
                chrome.execute_script("arguments[0].click()", elem)
                try:
                    time.sleep(0.5)
                    elemInput = elem.find_element_by_css_selector("form:first-child sc-dynamic-input input")
                    elemBtn = elem.find_element_by_css_selector("form:first-child sc-dynamic-input+a")
                    elemInput.clear()
                    elemInput.send_keys(str(leastPrice))
                    time.sleep(0.5)
                    chrome.execute_script("arguments[0].click()", elemBtn)
                    if strean in dictean:
                        dictean[strean]["times"] += 1
                        fprice = dictean[strean]["fprice"]
                        dictean[strean]["tprice"] = round(float((fprice-leastPrice)/fprice*100), 2)
                        dictean[strean]["change"] = 1
                    else:
                        dictean[strean] = {"times": 1,
                                           "fprice": currentPrice,
                                           "tprice": round(float((currentPrice-leastPrice)/currentPrice*100), 2),
                                           }
                except:
                    continue
            persentPrice = divPrice * 100
            timestr = time.strftime("%Y-%m-%d %H:%M:%S")
            out = timestr + \
                  "\t原价:" + str(currentPrice) + \
                  "\t差价比:" + str(round(persentPrice, 2)) + \
                  "%\t修改后:" + str(leastPrice)
            if divPrice >= percent:
                out = timestr + \
                      "\t原价:" + str(currentPrice) + \
                      "\t差价比:" + str(round(persentPrice, 2)) + \
                      "%\t未修改"
            if strean in dictean:
                out += "\t修改次数:" + str(dictean[strean]["times"])
                out += "\t原始价格:" + str(dictean[strean]["fprice"])
                out += "\t总降价比:" + str(dictean[strean]["tprice"]) + "%"
            # eansize = len(strean)
            # if eansize > 30:
            #    eansize = 30
            # out += "\t" + strean[0:eansize]
            print(out)

            out = timestr + "\t" + \
                  str(currentPrice) + "\t" + \
                  str(leastPrice) + "\t" + \
                  str(round(subPrice, 2)) + "\t" + \
                  str(round(persentPrice, 2)) + "%"
            if divPrice >= percent:
                out = out + "\t未修改"
            if strean in dictean:
                out += "\t" + str(dictean[strean]["times"])
                out += "\t" + str(dictean[strean]["fprice"])
                out += "\t" + str(dictean[strean]["tprice"])
            out += "\t" + strean + "\r\n"
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


def skr(account, password, currentDir, percent0=0.1, lowwer0=0.01, maxtimes=1, maxtprice0=0.3, minute=1):
    if not any(account) or not any(password):
        exit(0)
    percent = round(float(percent0), 2)
    lowwer = round(float(lowwer0), 2)
    maxtprice = round(float(maxtprice0), 2)
    print("----------------------")
    print("账  号\t", account)
    print("百分比\t", percent * 100, "%")
    print("降  价\t", lowwer, "AED")
    print("最大降价次数\t", maxtimes, "次")
    print("最大降价百分比\t", maxtprice * 100, "%")
    print("切换下一页等待时间\t", minute, "分钟")
    print("模式2：如果产品价格显示的地方有黄色三角感叹号，\n\t那么最低价格就是黄色三角感叹号中提示的价格。\n\t如果没有显示黄色三角感叹号，那么最低价就是该产品当前的价格。")
    print("----------------------")
    dictean = {}
    while 1:
        option = webdriver.ChromeOptions()
        option.add_argument("headless")
        option.add_argument('--ignore-certificate-errors')
        option.add_argument('log-level=3')
        option.add_argument('lang=zh_CN.UTF-8')
        prefs = {
            'profile.default_content_setting_values': {
                'images': 2,
            }
        }
        option.add_experimental_option('prefs', prefs)
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
                         currentDir,
                         dictean,
                         maxtimes,
                         maxtprice,
                         minute)
        except:
            chrome.quit()
            continue


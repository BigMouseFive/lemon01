# -*- coding: utf-8 -*-
from selenium import webdriver
from selenium.common import exceptions
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.wait import WebDriverWait
import re
import time


def LoginAccount(chrome, loginUri, account, password, record, attention):
    # 获取email 和 password 控件
    # print("登录 SouqSellers 系统")
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
            NewInventory(chrome, record, attention)
        except:
            continue


def NewInventory(chrome, record, attention):
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
    # chrome.switch_to_window(loginHandler)
    # chrome.close()
    chrome.switch_to_window(inventoryHandler)
    while 1:
        try:
            OperateProduct(chrome, record, attention)
        except:
            continue


def OperateProduct(chrome, record, attention):
    # 循环处理每一页
    print("循环处理每一页")
    index = 0
    pageCount = 1  # 起始页为第一页
    # 打开两个页面 一个用来获取EAN码 一个用来搜索EAN码

    while 1:
        # 处理当前这一页
        time.sleep(10)
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
            if not elemLeastDiv.is_displayed():
                continue
            elemLeast = elem.find_element_by_css_selector("div:first-child span+div i")
            currentPrice = float(elemSelf.text)
            ret = re.findall(r"\d+\.?\d*", elemLeast.get_attribute("popover"))
            if len(ret) <= 0:
                continue
            else:
                leastPrice = float(ret[0])
            subPrice = float(currentPrice - leastPrice)
            divPrice = float(subPrice / currentPrice)
            if leastPrice >= currentPrice or leastPrice == 0 or currentPrice == 999:
                continue

            if divPrice < 0.1:
                # 修改数据
                chrome.execute_script("arguments[0].click()", elem)
                time.sleep(1)
                try:
                    elemInput = elem.find_element_by_css_selector("form:first-child sc-dynamic-input input")
                    elemBtn = elem.find_element_by_css_selector("form:first-child sc-dynamic-input+a")
                    elemInput.clear()
                    elemInput.send_keys(str(leastPrice))
                    chrome.execute_script("arguments[0].click()", elemBtn)
                except:
                    continue
            persentPrice = divPrice * 100
            out = "第" + str(pageCount) + "页, " + "第" + str(rowCount) + "项\t\t" + \
                  "原价:" + str(currentPrice) + "\t差价比:" + str(round(persentPrice, 2)) + "%\t修改后:" + str(leastPrice)
            if divPrice >= 0.1:
                out = "第" + str(pageCount) + "页, " + "第" + str(rowCount) + "项\t\t" + \
                      "原价:" + str(currentPrice) + "\t差价比:" + str(round(persentPrice, 2)) + "%\t未修改"
            print(out)
            time.sleep(1)
            elemProduct = elemIter.find_element_by_css_selector("td:nth-child(3) span")
            out = str(pageCount) + "\t" + str(rowCount) + "\t" + str(currentPrice) + "\t" + \
                  str(leastPrice) + "\t" + str(subPrice) + "\t" + str(round(persentPrice, 2)) + "%\t" + elemProduct.text
            if divPrice >= 0.1:
                out = out + "\t未修改"
            out = out + "\r\n"
            if divPrice >= 0.1:
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


def skr(account, password):
    if not any(account) or not any(password):
        print("请输入用户名和密码")
        exit(0)
    try:
        record = open("UpdateRecord.txt", "a")
    except:
        record = open("UpdateRecord.txt", "x")
    try:
        attention = open("Attention.txt", "a")
    except:
        attention = open("Attention.txt", "x")

    while 1:
        option = webdriver.ChromeOptions()
        # option.add_argument("headless")
        chrome = webdriver.Chrome(chrome_options=option)
        chrome.maximize_window()
        try:
            LoginAccount(chrome,
                         'https://uae.souq.com/ae-en/login.php',
                         account,
                         password,
                         record,
                         attention)
        except:
            chrome.close()
            continue

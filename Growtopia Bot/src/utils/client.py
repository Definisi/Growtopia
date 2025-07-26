# BIG THANKYOU TO amertabyte for fixing my script!!!

# NOT SUPPORTED YET!!!
AUTO_CREATE_ACCOUNT = False # for the google login. it will automatically input randomly generated growid 

import logging
import requests
import bs4
import urllib
import enum 
import orjson
from types import UnionType
import time
import random
from fake_useragent import UserAgent
import string

def pretty_print_cookies(cookies: requests.cookies.RequestsCookieJar):
    for item in cookies:
        print(item.name + ": " + item.value)

def print_content_and_cookies(http_res):
    print("COOKIES: ")
    pretty_print_cookies(http_res.cookies)

    print("RESPONSE BODY: ")
    print(http_res.content.decode("utf-8"))

class LoginType(enum.StrEnum):
    Google = "/google/",
    Legacy = "/growid/"

def extract_login_url(page_source: str, login_type: LoginType):
    page_source_bs4 = bs4.BeautifulSoup(page_source, "html.parser")

    for a in page_source_bs4.find_all("a"):
        if login_type.value in a["href"]:
            return a["href"]
    
    return ""

def get_login_form_url(session: requests.Session, login_type: LoginType):
    # Get server data first to get meta
    headers = {
        "User-Agent": "UbiServices_SDK_2022.Release.9_PC64_ansi_static",
        "Content-Type": "application/x-www-form-urlencoded"
    }
    
    server_data = requests.post(
        "https://www.growtopia1.com/growtopia/server_data.php",
        headers=headers,
        data="version=5.23&platform=0&protocol=216"
    ).text

    # Extract meta from server data
    meta = ""
    for line in server_data.split('\n'):
        if line.startswith('meta|'):
            meta = line.split('|')[1]
            break

    login_data_xx = f"""tankIDName|\ntankIDPass|\nrequestedName|\nf|1\nprotocol|216\ngame_version|5.23\nfz|21905432\ncbits|0\nplayer_age|25\nGDPR|2\ncategory|_-5100\ntotalPlaytime|0\nklv|2d1eb1eaa4c67a886b0cc3823f3548fb3b4328ec55fa6dbaa1ef39d897914c6a\nhash2|-399932779\nmeta|{meta}\nfhash|-716928004\nrid|0240037D0B9AAD0B052BD3FA1EA8700E\nplatformID|0,1,1\ndeviceVersion|0\ncountry|us\nhash|-658895475\nmac|a8:7e:ea:88:2c:eb\nwk|64E3BDCA88C4CAAD79FC37CC8115799F\nzf|-1623530258"""
    login_data = urllib.parse.quote(login_data_xx, safe='')

    session.headers["User-Agent"] = "UbiServices_SDK_2022.Release.9_PC64_ansi_static"

    print("REQUESTING DATA FROM LOGIN DASHBOARD...")

    try:
        dashboard_res = session.post("https://login.growtopiagame.com/player/login/dashboard", login_data, verify=True, timeout=25)
        print(dashboard_res.text)
    except requests.exceptions.Timeout:
        return ""

    return extract_login_url(dashboard_res.text, login_type)

class LoginStatus(enum.Enum):
    Success = 0, # returns login token
    Failed = 1, # usually means the server returned status for anything except success. check second returned value to get the status. 
                # also could be the requests/selenium related exceptions.
    InvalidResponse = 2, # the server returned invalid response
    WrongCreds = 3, # wrong password and/or growid
    Timeout = 4,
    GenericHttpError = 5, #returns the http request result as the second value

    CaptchaRequested = 6, # usually from google
    UnsafeBrowser = 7, # from google too. Use GSuite emails

    CreateAccountRequested = 8,

def growid_login(session: requests.Session, link: str, growid: str, password: str) -> (LoginStatus, None | str | int):
    print("GETTING LOGIN FORM PAGE")

    login_form_res = session.get(link, verify=True)
    login_form_bs4 = bs4.BeautifulSoup(login_form_res.content.decode("utf-8"), "html.parser")
    print(login_form_bs4.find("input", {"name": "_token"}))
    login_form_token = login_form_bs4.find("input", {"name": "_token"})["value"]

    print("REQUESTING TO /player/login/validate")

    try:
        validate_login_res = session.post("https://login.growtopiagame.com/player/growid/login/validate", {
            "_token": login_form_token,
            "growId": growid,
            "password": password
        }, allow_redirects=False)
    except Exception as e:
        return LoginStatus.Failed, e.__class__.__name__

    if validate_login_res.status_code == 302:
        print("WRONG CREDENTIAL : ", growid, password)
        return LoginStatus.WrongCreds
    elif validate_login_res.status_code == 200:
        try:
            account_json: dict = orjson.loads(validate_login_res.content)
        except orjson.JSONDecodeError:
            return LoginStatus.InvalidResponse, validate_login_res.content

        if account_json["status"] != "success":
            return LoginStatus.Failed, account_json["status"]

        if not ("token" in account_json):
            return LoginStatus.InvalidResponse, validate_login_res.content

        return LoginStatus.Success, account_json["token"]
    else:
        return LoginStatus.GenericHttpError, validate_login_res

def check_wrong_creds(page_source: str):
    wrong_creds_list = ["find your Google Account", "Wrong password", "password was changed"]

    if any(substring in page_source for substring in wrong_creds_list):
        return True

    return False

def check_captcha(page_source: str):
    return "Type the text you hear" in page_source

def check_unsafe_browser(page_source: str):
    return "different browser" in page_source
    
def check_should_create_growid(page_source: str):
    return "Choose your name in Growtopia" in page_source or "Oops! The name is unavailable. Please choose a different name." in page_source

def growid_login_mode(session: requests.Session):
    print("[GROWID/LEGACY LOGIN MODE]")
    growid = input("Input your GrowID: ")
    password = input("Input your password: ")

    dashboard_data = get_login_form_url(session, LoginType.Legacy) 

    if dashboard_data == "":
        print("LOGIN DASHBOARD TIMEOUT")
        exit(1)

    login_status, token = growid_login(session, dashboard_data, growid, password)
    print("REQUESTING TO /player/login/validate")
    print(login_status, token)

    pass

if __name__ == "__main__":
    print("TOKEN GETTER MADE BY https://www.github.com/badewen")

    session = requests.session()

    mode = input()

    growid_login_mode(session)
    print("FINISHED!!")
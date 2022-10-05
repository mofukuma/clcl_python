# CLCLで選択中の文字を和訳 DeepL API free
# Pythonサンプルです。 DeepLのAPI keyが必要

import sys, os, time, json
import urllib.request
import shutil

DEEPL_ENDPOINT_TRANSLATE = 'https://api-free.deepl.com/v2/translate'
DEEPL_AUTH_KEY = "ここにYOUR API KEY"

def postAPI(url, params, headers):
    req = urllib.request.Request(
        url, 
        method='POST', 
        data=urllib.parse.urlencode(params).encode('utf-8'), 
        headers=headers
    )
        
    with urllib.request.urlopen(req, timeout=6) as res:
        res = json.loads(res.read().decode('utf-8'))

    return res


def deepl_translate(text, s_lang='EN', t_lang='JA'):
    headers = {
        'Content-Type': 'application/x-www-form-urlencoded; utf-8'
    }

    params = {
        'auth_key': DEEPL_AUTH_KEY,
        'text': text,
        'target_lang': t_lang
    }

    if s_lang != '':
        params['source_lang'] = s_lang

    r = postAPI(DEEPL_ENDPOINT_TRANSLATE, params, headers)
    
    return r["translations"][0]["text"]

# デフォルトではmainが実行される　input_textはクリップボードの中身
def main(input_text=""):
    ret = deepl_translate(input_text, "", "JA")  #和訳
    return ret #CLCLへ戻す

    
# 英訳したいときの関数。
def eiyaku(input_text=""):
    ret = deepl_translate(input_text, "JA", "EN")  #英訳
    return ret


#改行を空白に変換
def kaigyo_to_kuuhaku(input_text=""):
    return input_text.replace("\n", " ")


#改行を削除
def kaigyo_kesu(input_text=""):
    return input_text.replace("\n", "")


#!/usr/bin/env python

import json
import cgi
import cgitb
import SOAPpy
from SOAPpy.Types import simplify
from datetime import datetime
import time
import os
import os.path

import StringIO
import base64
cgitb.enable()


try:
    f = open("/etc/biosymbol/webserver.conf.json", "r")
    data = f.read()
    f.close()
    CONFIG = json.reads(f)
except:
    CONFIG = {
    "sufpref_soap_server_uri": "http://server2.lpm.org.ru:8051/",
    "sufpref_max_upload_filesize": 1024*1024
    }
    
form = cgi.FieldStorage()
method = form.getvalue("method","")

server = SOAPpy.SOAPProxy(CONFIG["sufpref_soap_server_uri"])

if method=="echo":
    limit = CONFIG["sufpref_max_upload_filesize"]
    fileitem = form["file"]
    data = fileitem.file.read(limit)
    data = data.replace("<", "&lt;").replace(">", "&gt;").replace(" ", "&nbsp;")
    response = {
        "success": True,
        "data": data
    }
    print "Content-Type: text/html"
    print
    print json.dumps(response)
elif method=="add_new_task":
    if os.environ.has_key("REMOTE_HOST"):
        ip = cgi.escape(os.environ["REMOTE_HOST"])
    else:
        ip = cgi.escape(os.environ["REMOTE_ADDR"])
    if form.has_key("comment"):
        comment = form["comment"].value
    else:
        comment = ""
    alphabet = form["alphabet"].value
    textlength = int(form["textlength"].value)
    occurences = int(form["occurences"].value)
    model_data = form.getvalue("model")
    pattern_data = form.getvalue("pattern")
    id = server.start_new_job(ip, alphabet, textlength, occurences, pattern_data, model_data, comment)
    print "Content-Type: application/json"
    print
    print json.dumps({"status": "OK", "id": int(id)})
elif method=="get_report_as_string":
    id = int(form["id"].value)
    res = server.get_result(id)
    simplify(res)
    report = unicode(res["report"]).encode('utf-8')
    print "Content-Type: text/plain"
    print
    print report
elif method=="get_report_as_file":
    id = int(form["id"].value)
    res = server.get_result(id)
    simplify(res)
    report = unicode(res["report"]).encode('utf-8')
    filename = "sufpref_id"+str(id)+".txt"
    print "Content-Type: text/plain"
    print "Content-Disposition: attachment; filename="+filename
    print "Content-Length: "+str(len(report))
    print
    print report


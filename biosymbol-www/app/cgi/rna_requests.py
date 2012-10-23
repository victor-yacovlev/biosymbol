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
    "rna_soap_server_uri": "http://server2.lpm.org.ru:8052/",
    "rna_max_upload_filesize": 70 * 1024*1024 # 70 megabytes
    }
    
form = cgi.FieldStorage()
method = form.getvalue("method","")

server = SOAPpy.SOAPProxy(CONFIG["rna_soap_server_uri"])

if method=="add_new_job":
    if os.environ.has_key("REMOTE_HOST"):
        ip = cgi.escape(os.environ["REMOTE_HOST"])
    else:
        ip = cgi.escape(os.environ["REMOTE_ADDR"])
    if form.has_key("comment"):
        comment = form["comment"].value
    else:
        comment = ""
    limit = CONFIG["rna_max_upload_filesize"]
    fileitem = form["file"]
    data = fileitem.file.read(limit)
    error = ""
    frame_size = int(form["frame_size"].value)
    frame_shift = int(form["frame_shift"].value)
    seed = float(form["seed"].value)
    if form.has_key("start_pos") and len(form["start_pos"].value)>0:
        start_pos = int(form["start_pos"].value)
    else:
        start_pos = 0
    if form.has_key("end_pos") and len(form["end_pos"].value)>0:
        end_pos = int(form["end_pos"].value)
    else:
        end_pos = -1
    penalty = form["penalty"].value
    try:
        error, id = server.start_new_job(ip, data, fileitem.filename, comment,penalty,start_pos,end_pos,frame_size,frame_shift,seed)
    except:
        id = 0
        error = "Can't connect to SOAP service"
    error = error.replace("<", "&lt;").replace(">", "&gt;").replace(" ", "&nbsp;")
    response = {
        "success": len(error)==0,
        "error": error,
        "id": id
    }
    print "Content-Type: text/html"
    print
    print json.dumps(response)
elif method=="get_report":
    id = int(form["id"].value)
    res = server.get_result(id)
    simplify(res)
    report = unicode(res["result"])
    print "Content-Type: application/json"
    print
    print report
elif method=="get_report_as_file":
    id = int(form["id"].value)
    res = server.get_result(id)
    res = json.loads(simplify(res)["result"])
    data = "start;end;score\r\n"
    for row in res:
        data += "%d;%d;%d\r\n" % (row["start"], row["end"], row["score"])
    filename = "rna_id"+str(id)+".csv"
    print "Content-Type: text/csv"
    print "Content-Disposition: attachment; filename="+filename
    print
    print data



#!/usr/bin/env python

import json
import cgi
import cgitb
import SOAPpy
from datetime import datetime
import time
import os
import logging
cgitb.enable()

#logging.basicConfig(filename="/var/log/biosymbol/www.log", format='%(asctime)-6s: %(levelname)s - %(message)s')
#log = logging.getLogger("Symbol")
#log.setLevel(logging.INFO)

try:
    f = open("/etc/biosymbol/webserver.conf.json", "r")
    data = f.read()
    f.close()
    CONFIG = json.reads(f)
except:
    CONFIG = {
    "parca_soap_server_uri": "http://server2.lpm.org.ru:8050/",
    "sufpref_soap_server_uri": "http://server2.lpm.org.ru:8051/",
    "rna_soap_server_uri": "http://server2.lpm.org.ru:8052/"
    }
    
form = cgi.FieldStorage()
method = form.getvalue("method","")

try:
    parca_server = SOAPpy.SOAPProxy(CONFIG["parca_soap_server_uri"])
except:
    parca_server = None
    
try:
    sufpref_server = SOAPpy.SOAPProxy(CONFIG["sufpref_soap_server_uri"])
except:
    sufpref_server = None
    
try:
    rna_server = SOAPpy.SOAPProxy(CONFIG["rna_soap_server_uri"])
except:
    rna_server = None

def compare(a, b):
    if a["start"]==b["start"]:
        return cmp(b["job_id"], a["job_id"])
    else:
        return cmp(b["start"], a["start"])

def get_jobs_for_table(ip=None):
    result = []
    servers = [
        (sufpref_server, "SufPref"),
        (parca_server, "PARCA"),
        (rna_server, "RNA")
    ]
    for server, toolname in servers:
        if not server is None:
            try:
                if ip is None:
                    jobs = server.list_all_jobs()
                else:
                    jobs = server.list_jobs_for_ip(ip)
                for job in jobs:
                    st = datetime.fromtimestamp((job["start"]))
                    result += [{
                        "tool": toolname,
                        "job_id": job["id"],
                        "comment": job["comment"],
                        "start": "%4d-%02d-%02d, %02d:%02d" % (st.year, st.month, st.day, st.hour, st.minute),
                        "status": job["status"],
                        "error": job["error_string"]
                    }]
            except:
                pass
    result.sort(cmp=compare)
    return result
    

if method=="list_all_jobs":
    result = get_jobs_for_table()
    print "Content-Type: application/json"
    print
    print json.dumps(result)
elif method=="list_my_jobs":
    if os.environ.has_key("REMOTE_HOST"):
        ip = cgi.escape(os.environ["REMOTE_HOST"])
    else:
        ip = cgi.escape(os.environ["REMOTE_ADDR"])
    result = get_jobs_for_table(ip)
    print "Content-Type: application/json"
    print
    print json.dumps(result)
elif method=="update_statuses":
    if form.has_key("tools_ids"):
        alist = json.loads(form["tools_ids"].value)
    else:
        alist = []
    result = []
    for item in alist:
        id = int(item["id"])
        tool = item["tool"]
        if tool=="PARCA" and not parca_server is None:
            error = ""
            status = parca_server.get_job_status(id)
            if status=="ERROR":
                error = parca_server.get_error(id)
            result += [ {"id": id, "tool": tool, "status": status, "error": error}]
    print "Content-Type: application/json"
    print
    print json.dumps(result)

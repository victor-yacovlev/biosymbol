#!/usr/bin/env python

import json
import cgi
import cgitb
import SOAPpy
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
    "parca_soap_server_uri": "http://server2.lpm.org.ru:8050/",
    "parca_max_upload_filesize": 4*1024*1024
    }
    
form = cgi.FieldStorage()
method = form.getvalue("method","")

server = SOAPpy.SOAPProxy(CONFIG["parca_soap_server_uri"])

if method=="format_by_filename":
    filename = form.getvalue("filename")
    format = server.format_by_filename(filename)
    print "Content-Type: text/plain"
    print
    print format
elif method=="parse_sequences":
    limit = CONFIG["parca_max_upload_filesize"]
    result = []
    success = True
    totalSequences = 0
    ALPHABET = "ARNDCEQGHILKMFPSTWYV"
    for key in form.keys():
        if key.startswith("file"):
            fileitem = form[key]
            filename = fileitem.filename
            if filename:
                data = fileitem.file.read(limit)
                limit -= len(data)
                format = server.format_by_filename(filename)
                error, sqs = server.parse_sequences(data, format)
                sequences = []
                for name, s in sqs:
                    sequences += [ {
                        "name": unicode(name),
                        "sequence": unicode(s.upper())} ]
                    totalSequences += 1
                    for ch in s.upper():
                        if not ch in ALPHABET:
                            error = "One of the sequence in file contains symbol out of alphabet"
                            break
                result += [{
                    "filename": filename,
                    "error": unicode(error),
                    "sequences": sequences
                }]
                if len(unicode(error))>0:
                    success = False
                    result[-1]["sequences"] = []
    response = {
        "success": success,
        "result": result,
        "totalSequences": totalSequences
        }
    print "Content-Type: text/html"
    print
    print json.dumps(response)
elif method=="list_all_jobs":
    all_jobs = server.list_all_jobs()
    result = []
    for job in all_jobs:
        st = datetime.fromtimestamp((job["start"]))
        result += [{
            "id": job["id"],
            "comment": job["comment"],
            "start": "%02d.%02d.%4d, %2d:%02d" % (st.day, st.month, st.year, st.hour, st.minute),
            "status": job["status"],
            "error": job["error_string"]
        }]
    print "Content-Type: application/json"
    print
    print json.dumps(result)
elif method=="list_my_jobs":
    if os.environ.has_key("REMOTE_HOST"):
        ip = cgi.escape(os.environ["REMOTE_HOST"])
    else:
        ip = cgi.escape(os.environ["REMOTE_ADDR"])
    all_jobs = server.list_jobs_for_ip(ip)
    result = []
    for job in all_jobs:
        st = datetime.fromtimestamp((job["start"]))
    result = []
    for job in all_jobs:
        st = datetime.fromtimestamp((job["start"]))
        result += [{
            "id": job["id"],
            "comment": job["comment"],
            "start": "%02d.%02d.%4d, %2d:%02d" % (st.day, st.month, st.year, st.hour, st.minute),
            "status": job["status"],
            "error": job["error_string"]
        }]
    print "Content-Type: application/json"
    print
    print json.dumps(result)
elif method=="add_new_job":
    if os.environ.has_key("REMOTE_HOST"):
        ip = cgi.escape(os.environ["REMOTE_HOST"])
    else:
        ip = cgi.escape(os.environ["REMOTE_ADDR"])
    if form.has_key("comment"): comment = form["comment"].value
    else: comment = ""
    if form.has_key("name1"): name1 = form["name1"].value
    else: name1 = ""
    if form.has_key("name2"): name2 = form["name2"].value
    else: name2 = ""
    if form.has_key("seq1"): seq1 = form["seq1"].value
    else: seq1 = ""
    if form.has_key("seq2"): seq2 = form["seq2"].value
    else: seq2 = ""
    if form.has_key("matrix"): matrix = form["matrix"].value
    else: matrix = "blosum62"
    if form.has_key("limit"):
        limit = form["limit"].value
        limit = int(limit)
    else:
        limit = 40
    if form.has_key("gep"):
        gep = form["gep"].value
        gep = float(gep)
    else:
        gep = 1.0
    seq1 = seq1.replace(" ","").replace("\n","").replace("\t","")
    seq2 = seq2.replace(" ","").replace("\n","").replace("\t","")
    id = int(server.start_new_job(ip, seq1.upper(), seq2.upper(), name1, name2, comment, limit, gep, matrix))
    print "Content-Type: application/json"
    print
    print json.dumps({"status": "OK", "id": id})
elif method=="get_job_details":
    id = int(form["id"].value)
    r = server.get_alignments_summary(id)
    result = {
        "id": id,
        "comment": r["comment"],
        "name1": r["name1"],
        "name2": r["name2"],
        "matrix": r["matrix"],
        "gep": r["gep"],
        "limit": r["gap_limit"],
        "alignments": [],
        "points": []
    }
    als = r["alignments"]
    for no, al in enumerate(als):
        if al["Rating"] is None:
            rating = "N/A"
            rating2 = 9999
        else:
            rating2 = rating = al["Rating"]
        if al["MinGOP"] is None:
            mingop = "N/A"
            maxgop = "N/A"
        else:
            mingop = str(round(al["MinGOP"],2))
            maxgop = round(al["MaxGOP"],2)
            if maxgop>=99999:
                maxgop = "INFINITY"
            else:
                maxgop = str(maxgop)
        if al["Score"] is None:
            score = 0.0
        else:
            score = round(al["Score"],2)
        a = {
            "no": al["no"],
            "g": al["g"],
            "m": al["m"],
            "score": score,
            "rating": rating,
            "mingop": mingop,
            "maxgop": maxgop,
            "no": no
        }
        result["alignments"].append(a)
        b = {
            "no": no,
            "m": al["m"],
            "g": al["g"],
            "rating": rating2
        }
        result["points"].append(b)
    result["alignments"].sort(cmp=lambda x,y: cmp(x["rating"], y["rating"]))
    result["points"].sort(cmp=lambda x,y: cmp(x["g"], y["g"]))
    print "Content-Type: application/json"
    print
    print json.dumps(result)
    
elif method=="get_alignment_as_string":
    fmt = form["format"].value
    id = int(form["id"].value)
    no = int(form["no"].value)
    res = server.get_alignment_as_string(id, no, fmt)[1]
    print "Content-Type: text/plain"
    print
    print res
elif method=="get_alignment_as_file":
    fmt = form["format"].value.lower()
    id = int(form["id"].value)
    no = int(form["no"].value)
    res = (server.get_alignment_as_string(id, no, fmt)[1]).encode('utf-8')
    
    filename = "parca_id"+str(id)+"_no"+str(no)+".txt"
    
    print "Content-Type: text/plain"
    print "Content-Disposition: attachment; filename="+filename
    print "Content-Length: "+str(len(res))
    print
    print res

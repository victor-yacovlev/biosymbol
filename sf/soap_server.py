#!/bin/env python

import SOAPpy
import sufpref
import json
import subprocess
import datetime
import sys
import os
import os.path
from sqlalchemy import *
from sqlalchemy.orm import sessionmaker
from sqlalchemy.ext.declarative import declarative_base
import sqlalchemy.exc
import tempfile
import time
import signal
import select
import logging
import socket
import bioformats
import warnings
import time
from SOAPpy.Types import simplify

warnings.filterwarnings("ignore")

DATABASE = "sqlite:///"
HOST = "*" # listen on all interfaces
PORT = 8051
TEMPDIR = tempfile.gettempdir()
WORKERS = 4
WORKER_CHECK_TIMEOUT = 2
PIDFILE = None
LOGFILE = ""

for arg in sys.argv:
    if arg.startswith("--db="):
        DATABASE = arg[5:]
    elif arg.startswith("--host="):
        HOST = arg[7:]
    elif arg.startswith("--port="):
        PORT = int(arg[7:])
    elif arg.startswith("--tempdir="):
        TEMPDIR = arg[10:]
    elif arg.startswith("--pid="):
        PIDFILE = arg[6:]

log = None

if PIDFILE:
    f = open(PIDFILE,"w")
    f.write(str(os.getpid()))
    f.close()

if __name__=="__main__" and not "--init" in sys.argv:
    logname = None
    for arg in sys.argv:
        if arg.startswith("--log="):
            LOGFILE = arg[6:]
            if LOGFILE:
                logname = arg[6:]
    logging.basicConfig(filename=logname, format='%(asctime)-6s: %(levelname)s - %(message)s')
    if not "--worker" in sys.argv:
        log = logging.getLogger()
        log.setLevel(logging.INFO)
        log.info("Using database "+DATABASE)
        log.info("Running at "+HOST+" on port "+str(PORT))
        log.info("Using PID file "+str(PIDFILE))
    else:
        log = logging.getLogger("worker")
        log.setLevel(logging.INFO)
    
    
engine = create_engine(DATABASE, echo=False)
Session = sessionmaker()
Session.configure(bind=engine)
session = Session()

Base = declarative_base()

ST_WAITING = 0
ST_WORKING = 1
ST_DONE = 2
ST_ERROR = 3

class SufprefJob(Base):
    __tablename__ = 'sufpref_jobs'

    id = Column(Integer, primary_key=True)
    # IP-address of owner client 
    ip = Column(String(20))
    # any string
    comment = Column(String(100))
    # date/time of job start and finish and status
    accept_datetime = Column(DateTime)
    start_datetime = Column(DateTime)
    finish_datetime = Column(DateTime)
    status = Column(Integer)
    error_string = Column(String(30))
    
    # input data
    alphabet = Column(String(256))
    n = Column(Integer)
    p = Column(Integer)
    H = Column(Text) # encoded as JSON
    dist = Column(Text) # encoded as JSON
    
    # output data
    result = Column(Float)
    report = Column(Text)
    res_words = Column(Text)

    def __init__(self, ip, alphabet, n, p, H, dist, comment=""):
        self.ip = ip
        self.alphabet = alphabet
        self.n = n
        self.p = p
        self.H = json.dumps(H)
        self.dist = json.dumps(dist)
        self.comment = comment
        self.accept_datetime = datetime.datetime.now()
        self.status = ST_WAITING
        self.error_string = ""
    
def process_job(job, session):
    H = json.loads(job.H)
    dist = json.loads(job.dist)
    if log:
        log.info("Processing job %d by worker %d" % (job.id, os.getpid()))
        #log.info("Job: %s, %d, %d, %s, %s" % (job.alphabet, job.n, job.p, H, dist))
    error_string, result, report, res_words = sufpref.calculate(job.alphabet, job.n, job.p, H, dist)
    job.error_string, job.result, job.report, job.res_words = error_string.strip(), result, report, json.dumps(res_words)
    job.finish_time = datetime.datetime.now()
    if len(job.error_string)>0:
        job.status = ST_ERROR
    else:
        job.status = ST_DONE
    session.commit()
    log.info("Worker %d has finished job %d" % (os.getpid(), job.id))
    

def start_new_job(ip, alphabet, n, p, H, dist, comment=""):
    session = Session()
    H = simplify(H)
    dist = simplify(dist)
    log.info("SOAP start new job: %s, %s, %d, %d, %s, %s, %s" % ( ip, alphabet, n, p, H, dist, comment ))
    job = SufprefJob( ip, alphabet, n, p, json.loads(H), json.loads(dist), comment)
    session.add(job)
    session.commit()
    return job.id

def get_job_status(job_id):
    session = Session()
    query = session.query(SufprefJob.status).filter(SufprefJob.id==job_id)
    if query.count()==0:
        return "WRONG ID"
    status, = query.first()
    if status==ST_DONE: return "DONE"
    elif status==ST_WAITING: return "WAITING"
    elif status==ST_WORKING: return "WORKING"
    else: return "ERROR"
    
def get_error(job_id):
    session = Session()
    query = session.query(SufprefJob.error_string).filter(SufprefJob.id==job_id)
    if query.count()==0:
        return "WRONG ID"
    error_string, = query.first()
    return error_string

def list_jobs_for_ip(ip):
    session = Session()
    query = session.query(
        SufprefJob.id,
        SufprefJob.comment,
        SufprefJob.accept_datetime,
        SufprefJob.status,
        SufprefJob.error_string
        ).filter(SufprefJob.ip==ip).all()
    res = []
    for id, comment, accept_datetime, status, error_string in query:
        if status==ST_ERROR:
            status = "ERROR"
        elif status==ST_DONE:
            status = "DONE"
        elif status==ST_WAITING:
            status = "WAITING"
        else:
            status = "WORKING"
        st = accept_datetime.timetuple()
        res += [ {
            "id": id,
            "comment": comment,
            "start": time.mktime(st),
            "status": status,
            "error_string": error_string
            }]
    return res

def list_all_jobs():
    session = Session()
    query = session.query(
        SufprefJob.id,
        SufprefJob.comment,
        SufprefJob.accept_datetime,
        SufprefJob.status,
        SufprefJob.error_string
        ).all()
    res = []
    for id, comment, accept_datetime, status, error_string in query:
        if status==ST_ERROR:
            status = "ERROR"
        elif status==ST_DONE:
            status = "DONE"
        elif status==ST_WAITING:
            status = "WAITING"
        else:
            status = "WORKING"
        st = accept_datetime.timetuple()
        res += [ {
            "id": id,
            "comment": comment,
            "start": time.mktime(st),
            "status": status,
            "error_string": error_string
            }]
    return res

def get_result(id):
    session = Session()
    query = session.query(
        SufprefJob.status,
        SufprefJob.result,
        SufprefJob.report,
        SufprefJob.res_words
        ).filter(SufprefJob.id==id)
    if query.count()==0:
        result, report, res_words = 0.0, "", []
    else:
        status, result, report, res_words = query.first()
        res_words = json.loads(res_words)
    if status!=ST_DONE:
        result, report, res_words = 0.0, "", []
    return {
        "result": result,
        "report": report,
        "res_words": res_words
    }

import threading

PROCS = []
PROCS_LOCK = threading.Lock()
autorestarter = None
server = None
worker_job_id = None

import fcntl
DB_FILE = None

def lock_database(session):
    global DB_FILE
    if DATABASE.startswith("mysql://"):
        session.query(RnaJob).from_statement("LOCK TABLES %s WRITE" % RnaJob.__tablename__)
        session.commit();
    elif DATABASE.startswith("sqlite://"):
        filename = DATABASE[9:]
        DB_FILE = open(filename)
        fcntl.flock(DB_FILE.fileno(), fcntl.LOCK_EX)
        
        
def unlock_database(session):
    global DB_FILE
    if DATABASE.startswith("mysql://"):
        session.query(RnaJob).from_statement("UNLOCK TABLES %s" % RnaJob.__tablename__)
        session.commit();
    elif DATABASE.startswith("sqlite://"):
        filename = DATABASE[9:]
        fcntl.flock(DB_FILE.fileno(), fcntl.LOCK_UN)
        DB_FILE.close()
        DB_FILE = None

def check_for_dead_workers():
    global PROCS
    global PROCS_LOCK
    script_path = os.path.abspath(__file__)
    python_path = sys.executable
    args = [python_path, script_path, "--worker", "--db="+DATABASE, "--log="+LOGFILE]
    stop = False
    while not stop:
        PROCS_LOCK.acquire()
        TO_REMOVE = []
        TO_ADD = []
        for proc in PROCS:
            retcode = proc.poll()
            if retcode is None:
                pass # process not finished
            else:
                TO_REMOVE += [proc]
                proc = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
                log.info("Restarted worker: "+str(proc.pid))
                TO_ADD += [proc]
        for proc in TO_REMOVE:
            PROCS.remove(proc)
        PROCS += TO_ADD
        stop = len(PROCS)==0
        PROCS_LOCK.release()
        time.sleep(WORKER_CHECK_TIMEOUT)

def exit_handler_main(a,b):
    global PIDFILE
    global PROCS
    global PROCS_LOCK
    if PIDFILE and os.path.exists(PIDFILE):
        os.unlink(PIDFILE)
    log.info("Exiting...")
    log.info("Stopping server...")
    PROCS_LOCK.acquire()
    for proc in PROCS:
        log.info("Terminating worker "+str(proc.pid)+"...")
        proc.terminate()
    PROCS = []
    PROCS_LOCK.release()
    log.info("Bye!")

def exit_handler_worker(a,b):
    if not worker_job_id is None:
        session = Session()
        query = session.query(SufprefJob).filter(SufprefJob.id==worker_job_id).order_by(SufprefJob.accept_datetime).limit(1)
        if query.count()>0:
            job.status = ST_WAITING
            session.commit()
    sys.exit(0)

if __name__=="__main__" and "--init" in sys.argv:
    SufprefJob.metadata.create_all(engine)

elif __name__=="__main__" and "--worker" in sys.argv:
    log.info("Worker %d boostrapping" % os.getpid())
    try:
        SufprefJob.metadata.create_all(engine)
    except sqlalchemy.exc.OperationalError as e:
        sys.exit(1)
        log.error("Worker %d: can't open database: %s" % (os.getpid(), str(e)))
    signal.signal(signal.SIGTERM, exit_handler_worker)
    signal.signal(signal.SIGINT, exit_handler_worker)
    log.info("Worker %d is waiting for tasks" % os.getpid())
    mustStop = False
    while not mustStop:
        session = Session()
        lock_database(session)
        query = session.query(SufprefJob).filter(SufprefJob.status==ST_WAITING).order_by(SufprefJob.accept_datetime).limit(1)
        if query.count()==0:
            session.commit()
            unlock_database(session)
            time.sleep(WORKER_CHECK_TIMEOUT)
        else:
            job = query.first()
            if not job is None:
                job.status = ST_WORKING
                job.start_datetime = datetime.datetime.now()
                worker_job_id = job.id
                session.commit()
                unlock_database(session)
                process_job(job, session)
                worker_job_id = None
                mustStop = True
                log.info("Worked %d stopped" % os.getpid())
        del session
    
            
elif __name__=="__main__" and not "--worker" in sys.argv:
    try:
        SufprefJob.metadata.create_all(engine)
    except sqlalchemy.exc.OperationalError as e:
        log.error("Can't open database: "+str(e)+". Exiting...")
        if PIDFILE and os.path.exists(PIDFILE):
            os.unlink(PIDFILE)
        sys.exit(2)
    HOST = HOST.replace("*","")
    try:
        server = SOAPpy.ThreadingSOAPServer(addr=(HOST, PORT))
    except socket.error as e:
        log.error("Can't create server: "+str(e)+". Exiting...")
        if PIDFILE and os.path.exists(PIDFILE):
            os.unlink(PIDFILE)
        sys.exit(1)
    script_path = os.path.abspath(__file__)
    python_path = sys.executable
    args = [python_path, script_path, "--worker", "--db="+DATABASE, "--log="+LOGFILE]
    PROCS_LOCK.acquire()
    for i in range(0, WORKERS):
        proc = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        log.info("Started worker: "+str(proc.pid))
        PROCS += [proc]
    PROCS_LOCK.release()
    signal.signal(signal.SIGTERM, exit_handler_main)
    signal.signal(signal.SIGINT, exit_handler_main)
    autorestarter = threading.Thread(target=check_for_dead_workers)
    autorestarter.start()
    server.registerFunction(start_new_job)
    server.registerFunction(get_job_status)
    server.registerFunction(list_jobs_for_ip)
    server.registerFunction(list_all_jobs)
    server.registerFunction(get_result)
    server.registerFunction(get_error)
    try:
        server.serve_forever()
    except select.error:
        server.shutdown()
        if PIDFILE and os.path.exists(PIDFILE):
            os.unlink(PIDFILE)
  
  
    



    

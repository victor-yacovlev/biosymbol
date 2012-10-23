#!/bin/env python

import SOAPpy
import _rna
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

warnings.filterwarnings("ignore")

DATABASE = "sqlite:///"
HOST = "*" # listen on all interfaces
PORT = 8052
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

class RnaJob(Base):
    __tablename__ = 'rna_jobs'

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
    
    # source sequences
    rna_data = Column(Text)
    name1 = Column(String(40))
    
    # Gap Elongation Penalty
    gap = Column(Float)

    #Start pos
    start= Column(Integer)

    #End pos
    end= Column(Integer)

    #Frame size
    wlen= Column(Integer)

    #Frame shift
    wshift= Column(Integer)

    #Seed len
    seed= Column(Integer)

    
    # Alignments information stored in JSON format:
    # [ { "m", "g", "Acc", "Conf", "MinGOP", "MaxGOP" } ]
    result = Column(Text)

    def __init__(self, ip, rna_data,  name1, comment,  gap=0.25,start=0,end=-1,wlen=3000,wshift=1000,seed=4):
        self.ip = ip
        self.rna_data = rna_data
        self.name1 = name1
        self.comment = comment
        self.gap = gap
        self.start = start
        self.end = end
        self.wlen = wlen
        self.wshift = wshift
        self.seed = seed

        self.accept_datetime = datetime.datetime.now()
        self.status = ST_WAITING
        self.error_string = ""

def process_job(job, session):
    if log:
        log.info("Processing job %d by worker %d" % (job.id, os.getpid()))
        #log.info("Job: %s, %d, %d, %s, %s" % (job.alphabet, job.n, job.p, H, dist))
    result = _rna.doAll(job.rna_data.encode("ascii"),job.wlen,job.wshift,job.gap,job.start,job.end,job.seed)
    sres = []
    for a in result:
        if a.score>=20:
            sres += [{"start":a.start,"end":a.end,"score":a.score}]
    job.result=json.dumps(sres)
    job.finish_time = datetime.datetime.now()
    if len(job.error_string)>0:
        job.status = ST_ERROR
    else:
        job.status = ST_DONE
    session.commit()
    log.info("Worker %d has finished job %d" % (os.getpid(), job.id))
    
def format_by_filename(filename):
    return bioformats.detect_format(filename)

def parse_sequences(buff, fmt):
    err = ""
    result = []
    try:
        result = bioformats.read_sequences_from_buffer(buff, fmt)
    except bioformats.BioFormatException as e:
        result = []
        err = e.text
    result = map(lambda x: {"id": x[0], "seq": x[1]}, result)
    return err, result


def start_new_job(ip, file_data, name1,comment,gap,start,end,wlen,wshift,seed):
    session = Session()
    log.info("SOAP start new job: %s, %s, %d, %d, %d, %d, %d" % ( ip, name1,start,end,wlen,wshift,seed ))
    ftype=format_by_filename(name1)
    err, sequences = parse_sequences(file_data,ftype)
    ALPHABET = "ATGCUNatgcun"
    id = 0
    if len(err)==0:
        name = sequences[0]["id"]
        seq = sequences[0]["seq"]
        for index, ch in enumerate(seq):
            if not ch in ALPHABET:
                err = "Sequence contains symbol out of alphabet: "+ch+" (pos "+str(index)+") "+seq[index-10:index+10]
                break
        if len(err)==0:
            job = RnaJob( ip, seq, name, comment,gap,start,end,wlen,wshift,seed )
            session.add(job)
            session.commit()
            id = job.id
    return err, id

def get_job_status(job_id):
    session = Session()
    query = session.query(RnaJob.status).filter(RnaJob.id==job_id)
    if query.count()==0:
        return "WRONG ID"
    status, = query.first()
    if status==ST_DONE: return "DONE"
    elif status==ST_WAITING: return "WAITING"
    elif status==ST_WORKING: return "WORKING"
    else: return "ERROR"
    
def get_error(job_id):
    session = Session()
    query = session.query(RnaJob.error_string).filter(RnaJob.id==job_id)
    if query.count()==0:
        return "WRONG ID"
    error_string, = query.first()
    return error_string

def list_jobs_for_ip(ip):
    session = Session()
    query = session.query(
        RnaJob.id,
        RnaJob.comment,
        RnaJob.accept_datetime,
        RnaJob.status,
        RnaJob.error_string
        ).filter(RnaJob.ip==ip).all()
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
        RnaJob.id,
        RnaJob.comment,
        RnaJob.accept_datetime,
        RnaJob.status,
        RnaJob.error_string
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
        RnaJob.status,
        RnaJob.result,
        ).filter(RnaJob.id==id)
    if query.count()==0:
        result  =  ""
    else:
        status, result = query.first()
    if status!=ST_DONE:
        result= ""
    return {
        "result": result,
    }

PROCS = []
server = None
worker_job_id = None


def exit_handler_main(a,b):
    if PIDFILE and os.path.exists(PIDFILE):
        os.unlink(PIDFILE)
    log.info("Exiting...")
    log.info("Stopping server...")
    for proc in PROCS:
        log.info("Terminating worker "+str(proc.pid)+"...")
        proc.terminate()
    log.info("Bye!")

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
        
        


def exit_handler_worker(a,b):
    if not worker_job_id is None:
        session = Session()
        query = session.query(RnaJob).filter(RnaJob.id==worker_job_id).order_by(RnaJob.accept_datetime).limit(1)
        if query.count()>0:
            job.status = ST_WAITING
            session.commit()
    sys.exit(0)

if __name__=="__main__" and "--init" in sys.argv:
    RnaJob.metadata.create_all(engine)

elif __name__=="__main__" and "--worker" in sys.argv:
    log.info("Worker %d boostrapping" % os.getpid())
    try:
        RnaJob.metadata.create_all(engine)
    except sqlalchemy.exc.OperationalError as e:
        sys.exit(1)
        log.error("Worker %d: can't open database: %s" % (os.getpid(), str(e)))
    signal.signal(signal.SIGTERM, exit_handler_worker)
    signal.signal(signal.SIGINT, exit_handler_worker)
    log.info("Worker %d is waiting for tasks" % os.getpid())
    while True:
        session = Session()
        lock_database(session)
        query = session.query(RnaJob).filter(RnaJob.status==ST_WAITING).order_by(RnaJob.accept_datetime).limit(1)
        if query.count()==0:
            unlock_database(session)
            session.commit()
            time.sleep(WORKER_CHECK_TIMEOUT)
        else:
            job = query.first()
            if not job is None:
                job.status = ST_WORKING
                job.start_datetime = datetime.datetime.now()
                worker_job_id = job.id
                session.commit()
                unlock_database(session)
                if not job is None:
                    process_job(job, session)
                worker_job_id = None
        del session

elif __name__=="__main__" and not "--worker" in sys.argv:
    try:
        RnaJob.metadata.create_all(engine)
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
    for i in range(0, WORKERS):
        proc = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        log.info("Started worker: "+str(proc.pid))
        PROCS += [proc]
    signal.signal(signal.SIGTERM, exit_handler_main)
    signal.signal(signal.SIGINT, exit_handler_main)
    server.registerFunction(start_new_job)
    server.registerFunction(get_job_status)
    server.registerFunction(list_jobs_for_ip)
    server.registerFunction(list_all_jobs)
    server.registerFunction(get_result)
    server.registerFunction(format_by_filename)
    server.registerFunction(parse_sequences)
    server.registerFunction(get_error)


    try:
        server.serve_forever()
    except select.error:
        server.shutdown()
        if PIDFILE and os.path.exists(PIDFILE):
            os.unlink(PIDFILE)
  
  
    



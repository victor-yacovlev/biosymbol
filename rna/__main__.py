#!/usr/bin/python2.7
import _rna
import bioformats
import sys
import argparse

if "--selftest" in sys.argv:
    import _test
    # Module _test should exit program after tesing complete

parser = argparse.ArgumentParser(prog="rna", description='ARG PRC');

parser.add_argument("FILE",
                    help="File with RNA data.")

parser.add_argument("-f", "--startfrom",type=int ,
                    help="Start pos.")

parser.add_argument("-t", "--to",type=int ,
                    help="End pos.")

parser.add_argument("-s", "--seed",type=int ,
                    help="Start seed.")

parser.add_argument("-d", "--dist",type=int ,
                    help="Max distance.")

parser.add_argument("-l", "--wlen",type=int ,
                    help="Frame size.")

parser.add_argument("-w", "--wshift",type=int ,
                    help="Frame shift.")
                    
parser.add_argument("-m", "--minscore",type=float,
                    help="Minimal score.")


args = parser.parse_args()
sfrom=0
end=-1
seed=4
dist=50;
wlen=3000;
wshift=1000;
minscore=0.25;


if args.startfrom:
        sfrom=args.startfrom;

if args.to:
        end=args.to;

if args.seed:
        seed=args.seed;

if args.dist:
        dist=args.dist;

if args.wlen:
        wlen=args.wlen;
        
if args.wshift:
        wshift=args.wshift;

if args.minscore:
        minscore=args.minscore;


try:
    sequences = bioformats.read_sequences_from_file(args.FILE)
except bioformats.BioFormatException as e:
    sys.stderr.write("Error: "+e.text+"\n")
    sys.exit(1)

if not sequences:
    sys.stderr.write("Error: file does not contain any sequence\n")
    sys.exit(2)

name, seq = sequences[0]
res = _rna.doAll(seq.encode('ascii'),wlen,wshift,minscore,sfrom,10000,seed)

import csv

writer = csv.writer(sys.stdout, dialect="excel")
writer.writerow(["start", "end", "score"])
for a in res:
    writer.writerow([a.start, a.end, a.score])

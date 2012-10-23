#!/usr/bin/python2.7
import _rna
import bioformats
import sys
import argparse

if "--selftest" in sys.argv:
    import _test
    # Module _test should exit program after tesing complete

parser = argparse.ArgumentParser(description='ARG PRC');

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
                    
parser.add_argument("-m", "--minscore",type=int ,
                    help="Minimal score.")


args = parser.parse_args()
sfrom=0
end=-1
seed=4
dist=50;
wlen=3000;
wshift=1000;
minscore=19;


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
        minscore=args.minscore-1;




SS = "AAATTT"
name, seq = bioformats.read_sequences_from_file(args.FILE)[0]
res = _rna.doAll(seq.encode('ascii'),wlen,wshift,0.25,sfrom,10000,seed)
#print seq
print "Count", len(res)
for a in res:
    if a.score>minscore :
          print "%d,%d,%d" % (a.start, a.end, a.score)

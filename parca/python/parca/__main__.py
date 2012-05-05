import _base
import _util
import MatrixInfo
import _custom_matrices

import sys
import os.path
import bioformats
from bioformats import BioFormatException
from datetime import datetime

def _print_usage_and_exit(code):
    sys.stderr.write("parca [--gep=REAL] [--matrix=STRING] [--out=FILENAME] [--outfmt=STRING] FILE1 [FILE2]\n")
    sys.stderr.write("\t--gep\t\tGEP value (default: 1.0)\n")
    sys.stderr.write("\t--matrix\tmatrix to use (default: blosum62)\n")
    sys.stderr.write("\t--out\t\toutput filename (by default output to console)\n")
    sys.stderr.write("\t--outfmt\toutput format (fasta, clustal, emboss) (default: emboss)\n")
    sys.exit(code)
    
if __name__=="__main__":
    file1name = None
    file2name = None
    gep = 1.0
    mn = "blosum62"
    out = sys.stdout
    outfmt = "emboss"
    outname = None
    rundate = datetime.now()
    for arg in sys.argv[1:]:
        if arg.startswith("--gep="):
            try:
                gep = float(arg[6:])
            except:
                sys.stderr.write("Error: --gep value is not real number\n")
                sys.exit(1)
            if gep<0:
                sys.stderr.write("Error: --gep value can not be negative\n")
                sys.exit(1)
        elif arg.startswith("--matrix="):
            mn = arg[9:].lower()
            if not (mn in MatrixInfo.available_matrices) or (mn in ["pam240", "pam360", "pam480"]):
                sys.stderr.write("Error: unknown matrix\n")
                sys.exit(3)
        elif arg.startswith("--out="):
            outname = arg[6:]
            out = f.open(outname, 'w')
        elif arg.startswith("--outfmt="):
            outfmt = arg[9:]
            if not outfmt in ["fasta","clustal","emboss"]:
                sys.stderr.write("Error: unknown output format\n")
                sys.exit(4)
        elif arg.startswith("-"):
            _print_usage_and_exit(0)
        else:
            if file1name is None:
                file1name = arg
            else:
                file2name = arg
           
    if file1name is None:
        _print_usage_and_exit(0)
    seqs = []
    try:
        seqs += bioformats.read_sequences_from_file(file1name)
    except BioFormatException as e:
        sys.stderr.write("Error: "+e.text+"\n")
        sys.exit(5)
    if not file2name is None:
        try:
            seqs += bioformats.read_sequences_from_file(file1name)
        except BioFormatException as e:
            sys.stderr.write("Error: "+e.text+"\n")
            sys.exit(5)

    if len(seqs)<2:
        sys.stderr.write("Error: requires at least two sequences\n")
        sys.exit(6)
    
    name1, seq1 = seqs[0]
    name2, seq2 = seqs[1]
    if not mn in ["pam360", "pam480", "pam240"]:
        matrix = MatrixInfo.__dict__[mn]
    else:
        matrix = _custom_matrices.__dict__[mn]
    
    sys.stderr.write("Please wait...\n")
    als = _base.get_pareto_alignments(seq1,seq2,gep,matrix,40)
    err = _base.get_last_error()
    if len(err)>0:
        sys.stderr.write("Error: "+err+"\n")
        sys.exit(7)
    primary = _util.get_primary_alignments(als)
    for pa in primary:
        #out.write("\n# Score = %i, Deletions = %i, Gaps = %i \n" % (pa.m, pa.d, pa.g))
        s = bioformats.proteins_alignment_to_string(
            pa.data,
            seq1,
            seq2,
            name1,
            name2,
            outfmt,
            matrix,
            mn,
            gep,
            rundate,
            "parca",
            outname,
            pa.m
            )
        #s = _util.alignment_to_string(seq1,seq2,pa.data,cps,matrix,60)
        out.write(s+"\n")
    out.close()
        

class BioFormatException(Exception):
    text = str()
    def __init__(self, text):
        self.text = text
    


# --------------- READERS ------------

from Bio import SeqIO, AlignIO
import os.path
import string
from cStringIO import StringIO
import warnings
warnings.filterwarnings("ignore")

FORMATS = [ "fasta", "fastq", "fastcd", "clustal", "genbank", "embl", "bed", "gff" ]

def __read_fastcd__(buff):
    lines = buff.split("\n")
    lines = filter(lambda line: len(line)>0, lines)
    BAD = "`~!@#$%^&*;:'/\\"
    lines = filter(lambda line: not line[0] in BAD, lines)
    try:
        result = read_sequences_from_buffer(string.join(lines,"\n"), "fasta")
    except:
        raise BioFormatException("Data not in format 'fastcd'")
    return result

def __read_bed__(buff):
    lines = buff.split("\n")
    lines = filter(lambda line: len(line)>0, lines)
    for i in range(0, len(lines)):
        line = lines[i]
        if line.startswith("//") or line.startswith("\\\\"):
            lines = lines[0:i]
            break
    try:
        result = read_sequences_from_buffer(string.join(lines,"\n"), "fasta")
    except:
        raise BioFormatException("Data not in format 'bed'")
    return result

def __read_gff__(buff):
    lines = buff.split("\n")
    lines = filter(lambda line: len(line)>0, lines)
    for i in range(0, len(lines)):
        line = lines[i]
        if line.startswith("//") or line.startswith("\\\\"):
            lines = lines[0:i]
            break
    try:
        result = read_sequences_from_buffer(string.join(lines,"\n"), "fasta")
    except:
        raise BioFormatException("Data not in format 'gff")
    return result

def detect_format(filename):
    "Detect format by filename"
    fn, ext = os.path.splitext(filename)
    ext = ext.lower()
    if ext in [".fasta", ".fa", ".seq", ".fsa", ".fna", ".ffn", ".faa", ".frn"]:
        return "fasta"
    elif ext in [".fastcd"]:
        return "fastcd"
    elif ext in [".fq", ".fastq"]:
        return "fastq"
    elif ext in [".clustal", ".clustalw", ".clustalx", ".aln"]:
        return "clustal"
    elif ext in [".genbank", ".gb"]:
        return "genbank"
    elif ext in [".embl"]:
        return "embl"
    elif ext in [".bed"]:
        return "bed"
    elif ext in [".gff"]:
        return "gff"
    else:
        return "unknown"

def read_sequences_from_buffer(buff, fmt):
    """Parse a string.
    Returns a list of (unicode, unicode), where first string is
    an ID, and second id sequence itself"""
    fmt = fmt.lower()
    if not fmt in FORMATS:
        raise BioFormatException("Unsupported format: "+fmt)
    if not fmt in ["fastcd", "bed", "gff"]:
        stream = StringIO(buff)
        try:
            it = SeqIO.parse(stream, fmt)
        except:
            raise BioFormatException("Data not in format '"+fmt+"'")
        result = []
        for record in it:
            result += [(unicode(record.id), unicode(record.seq).upper())]
        return result
    elif fmt=="fastcd":
        return __read_fastcd__(buff)
    elif fmt=="bed":
        return __read_bed__(buff)
    elif fmt=="gff":
        return __read_gff__(buff)
    

def read_sequences_from_file(filename, fmt=None):
    """Read file.
    Returns a list of (unicode, unicode), where first string is
    an ID, and second id sequence itself"""
    if not fmt:
        fmt = detect_format(filename)
    fmt = fmt.lower()
    if not fmt in FORMATS:
        raise BioFormatException("Unsupported format: "+fmt)
    if fmt in ["fastcd", "bed", "gff"]:
        try:
            f = open(filename, "r")
        except:
            raise BioFormatException("Can't open file: "+filename)
        data = f.read()
        f.close()
        return read_sequences_from_buffer(data, fmt)
    try:
        f = open(filename, "r")
    except:
        raise BioFormatException("Can't open file: "+filename)
    result = []
    try:
        it = SeqIO.parse(f, fmt)
        for record in it:
            result += [(unicode(record.id), unicode(record.seq).upper())]
    except:
        raise BioFormatException("File not in format '"+fmt+"': "+filename)        
    finally:
        f.close()
    return result

def read_sequence_from_biopython_seq(s):
    """Convert BioPython Seq into our internal format.
    Returns (unicode, unicode), where first string is
    an ID, and second id sequence itself"""
    return unicode(s.id), unicode(s.seq).upper()

# --------------- WRITERS ------------

from Bio.Alphabet import IUPAC, Gapped
from Bio.Align.Generic import Alignment
from datetime import datetime

OUT_FORMATS = ["fasta", "fastq", "clustal", "emboss"]

def __alignment_to_fastq_string__(
    source,
    mutant,
    alignment,
    name1,
    name2
):
    S1 = ""
    S1S = ""
    S2 = ""
    S2S = ""
    for a, b in alignment:
        ch1 = "-"
        ch2 = "-"
        if a!=-1:
            ch1 = source[a]
        if b!=-1:
            ch2 = source[b]
        S1 += ch1
        S2 += ch2
        S1S += ";"
        S2S += ";"
    return "@"+name1+"\n"+S1+"\n+\n"+S1S+"\n@"+name2+"\n"+S2+"\n+\n"+S2S+"\n"

def __alignment_to_emboss_string__(
    source,
    mutant,
    alignment,
    name1=None,
    name2=None,
    weights=None,
    matrixname=None,
    gep=None,
    gop=(None, None),
    rundate=None,
    program="parca",
    reportfile=None,
    score=None
    ):
    line1 = ""
    line2 = ""
    line3 = ""
    for i1, i2 in alignment:
        assert i1!=-1 or i2!=-1
        if i1!=-1 and i2!=-1:
            line1 += source[i1]
            line3 += mutant[i2]
            if source[i1]==mutant[i2]:
                line2 += "|"
            else:
                a = source[i1]
                b = source[i2]
                w = None
                if not weights is None:
                    if weights.has_key((a,b)):
                        w = weights[(a,b)]
                    elif weights.has_key((b,a)):
                        w = weights[(b,a)]
                if not w is None:
                    if w>=0:
                        line2 += ":"
                    else:
                        line2 += "."
                else:
                    line2 += " "
                
        elif i1==-1:
            line1 += "-"
            line2 += " "
            line3 += mutant[i2]
        elif i2==-1:
            line1 += source[i1]
            line2 += " "
            line3 += "-"


    result = ""

    fpe = 0
    spe = 0
    fp = 0
    sp = 0
    index = 0
    while len(line1)>0:
        line1_part = line1[0:50]
        line2_part = line2[0:50]
        line3_part = line3[0:50]
            
        fp = fpe + 1
        sp = spe + 1
        fpe = fp + len(line1_part)-line1_part.count("-")
        spe = sp + len(line3_part)-line3_part.count("-")
        result += str(fp).rjust(7)+" "+line1_part + \
            "  "+ str(fpe)+"\n"
        result += " "*8+line2_part.ljust(50) + "\n"
        result += str(sp).rjust(7)+" "+line3_part + \
            "  "+ str(spe)+"\n"
        result += "\n"
        sp = spe
        fp = fpe
        line1 = line1[50:]
        line2 = line2[50:]
        line3 = line3[50:]
        index += 50

    header = "#"*40+"\n"
    if not program is None:
        header += "# Program:  "+program+"\n"
    if not rundate is None:
        WEEKDAYS = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"]
        MONTHS = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
        header += "# Rundate:  %s %s %2d %2d:%2d:%2d %4d\n" % (
            WEEKDAYS[rundate.weekday()],
            MONTHS[rundate.month-1],
            rundate.day,
            rundate.hour,
            rundate.minute,
            rundate.second,
            rundate.year
            )
    if not reportfile is None:
        header += "# Report_file: "+reportfile+"\n"
    header += "#"*40 +"\n"+"#"+"="*39+"\n#\n"
    header += "# Aligned_sequences: 2\n"
    header += "# 1: "+name1+"\n"
    header += "# 2: "+name2+"\n"
    if not matrixname is None:
        header += "# Matrix: E"+matrixname.upper()+"\n"
    if not gop is None and not gop[0] is None:
        header += "# Gap_penalty: from %4.1f to %4.1f\n" % gop
    else:
        header += "# Gap_penalty: n/a\n"
    if not gep is None:
        header += "# Extend_penalty: "+str(gep)+"\n"
    header += "#\n"
    le = len(list(alignment))
    matches = len(filter(lambda x: x[0]!=-1 and x[1]!=-1, list(alignment)))
    header += "# Length: "+str(le)+"\n"
    ident = float(matches)/float(le)*100
    gaps = float(le-matches)/float(le)*100
    a = str(matches)+"/"+str(le)+" (%4.1f%%)" % (ident)
    b = str(le-matches)+"/"+str(le)+" (%4.1f%%)" % (gaps)
    header += "# Identity:   "+a.rjust(20)+"\n"
    header += "# Similarity: "+a.rjust(20)+"\n"
    header += "# Gaps:       "+b.rjust(20)+"\n"
    if not score is None:
        header += "# Score: %4.1f\n" % float(score)
    header += "#\n#\n#"+"="*39+"\n\n"
    return header+result+"\n"+"#"+"-"*39+"\n#"+"-"*39+"\n"

def proteins_alignment_to_biopython(al, seq1, seq2, name1, name2):
    "Convert our internal alignment format into BioPython Alignment"
    s1 = ""
    s2 = ""
    align = Alignment(Gapped(IUPAC.protein, "-"))
    for a, b in al:
        if a!=-1:
            s1 += seq1[a].upper()
        else:
            s1 += "-"
        if b!=-1:
            s2 += seq2[b].upper()
    align.add_sequence(name1, s1)
    align.add_sequence(name2, s2)
    return align

def proteins_alignment_to_string(
    al,
    seq1,
    seq2,
    name1,
    name2,
    fmt="emboss",
    weights=None,
    matrixname=None,
    gep=None,
    gop=(None, None),
    rundate=None,
    program="parca",
    reportfile=None,
    score=None
    ):   
    "Convert our internal alignment format into standard string representation"
    fmt = fmt.lower()
    if not fmt in OUT_FORMATS:
        raise BioFormatException("Unknown output format '"+fmt+"'")
    if fmt!="emboss" and fmt!="fastq":
        align = proteins_alignment_to_biopython(al, seq1, seq2, name1, name2)
        return align.format(fmt)
    elif fmt=="fastq":
        return __alignment_to_fastq_string__(seq1, seq2, al, name1, name2)
    else:
        return __alignment_to_emboss_string__(seq1, seq2, al, name1, name2,
                                              weights, matrixname, gep, gop,
                                              rundate, program, reportfile, score)

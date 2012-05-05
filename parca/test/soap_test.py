import SOAPpy
import time

HOST = "localhost"
PORT = 8050
CHECK_TIMEOUT = 1
FASTA1 = "test1.fasta"
FASTA2 = "test2.fasta"

server = SOAPpy.SOAPProxy("http://"+HOST+":"+str(PORT)+"/")

print("Processing plain text task")

S1 = u"ELTESTRTIPLDEAGGTTTLTARQFTNGQKIFVDTCT" + \
    "QCHLQGKTKTNNNVSLGLADLAGAEPRRDNVLALVEF" + \
    "LKNPKSYDGEDDYSELHPNISRPDIYPEMRNYTEDDI" + \
    "FDVAGYTLIAPKLDERWGGTIYF"
S2 = "EADLALGKAVFDGNCAACHAGGGNNVIPDHTLQKAAI" + \
    "EQFLDGGFNIEAIVYQIENGKGAMPAWDGRLDEDEIA" + \
    "GVAAYVYDQAAGNKW"

id = server.start_new_job(
    "127.0.0.1", S1, S2,
    "asequence", "bsequence",
    "plaintext-test", 40, 1.0, "blosum62"
    )

finished = False
while not finished:
    status = server.get_job_status(id)
    print("Status: "+status)
    if status=="DONE" or status=="ERROR":
        break
    else:
        time.sleep(1)

if status=="ERROR":
    print(server.get_error(id))

count = server.get_alignments_count(id)
print ("Have "+str(count)+" alignments")
for i in range(0,count):
    err, s = server.get_alignment_as_string(id,i,"emboss")
    print(err)
    print(s)
for i in range(0,count):
    err, s = server.get_alignment_as_string(id,i,"clustal")
    print(err)
    print(s)
for i in range(0,count):
    err, s = server.get_alignment_as_string(id,i,"fasta")
    print(err)
    print(s)
        

print("Processing fasta input task")
fmt1 = server.format_by_filename(FASTA1)
fmt2 = server.format_by_filename(FASTA2)
f1 = open(FASTA1, 'r')
data1 = f1.read()
f1.close()
f2 = open(FASTA2, 'r')
data2 = f2.read()
f2.close()
err1, seqs1 = server.parse_sequences(data1, fmt1)
err2, seqs2 = server.parse_sequences(data2, fmt1)
print(err1)
print(err2)

firstseq = seqs1[0]
secondseq = seqs2[0]
seq1 = firstseq["seq"]
seq2 = secondseq["seq"]
name1 = firstseq["id"]
name2 = secondseq["id"]

id = server.start_new_job(
    "127.0.0.1", seq1, seq2,
    name1, name2, "fasta-test", 40, 1.0, "blosum62"
    )

finished = False
while not finished:
    status = server.get_job_status(id)
    print("Status: "+status)
    if status=="DONE" or status=="ERROR":
        break
    else:
        time.sleep(1)

if status=="ERROR":
    print(server.get_error(id))

count = server.get_alignments_count(id)

print ("Have "+str(count)+" alignments")

for i in range(0,count):
    err, s = server.get_alignment_as_string(id,i,"emboss")
    print(err)
    print(s)
for i in range(0,count):
    err, s = server.get_alignment_as_string(id,i,"clustal")
    print(err)
    print(s)
for i in range(0,count):
    err, s = server.get_alignment_as_string(id,i,"fasta")
    print(err)
    print(s)

print("All tests passed.")


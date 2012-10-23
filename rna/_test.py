import sys

failed = [False] * 3
_ = "Test 1. Check system environment"
sys.stderr.write(_ + "... ")

try:
    import _rna
    import os
    import csv
    import gzip
    import StringIO
    assert os.name == "nt" or os.name == "posix"
    assert sys.version_info.major == 2
except:
    failed[0] = True
if failed[0]:
    sys.stderr.write("FAILED\n")
    sys.exit(30)
else:
    sys.stderr.write("PASSED\n")

RESOURCE_PATH = os.path.dirname(__file__)+"/test_data"

_ = "Test 2. Check for module working"
sys.stderr.write(_ + " (please wait, it takes some time)... ")

f = gzip.open(RESOURCE_PATH+"/in.txt.gz", "r")
SS = f.read().strip()
f.close()
sfrom=0
end=-1
seed=4
dist=50
wlen=3000
wshift=1000
minscore=19
try:
    res = _rna.doAll(SS,wlen,wshift,0.25,sfrom,10000,seed)
except:
    failed[1] = True

if failed[1]:
    sys.stderr.write("FAILED\n")
    sys.exit(30)
else: sys.stderr.write("PASSED\n")

_ = "Test 3. Check for result correctness"
sys.stderr.write(_ + "... ")

result_data = str()

csvfile = StringIO.StringIO(result_data)
writer = csv.writer(csvfile, dialect="excel")
writer.writerow(["start", "end", "score"])
for a in res:
    writer.writerow([a.start, a.end, a.score])


f = open(RESOURCE_PATH+"/reference.csv", "rb")
reference_data = f.read()
f.close()

try:
    assert reference_data==csvfile.getvalue();
except:
    failed[2] = True

if failed[2]: sys.stderr.write("FAILED\n")
else: sys.stderr.write("PASSED\n")



if True in failed:
    sys.stderr.write("SUMMARY: Some of tests failed!\n")
    sys.exit(30)
else:
    sys.stderr.write("SUMMARY: All tests passed successfully.\n")
    sys.exit(0)


import sys
failed = [False] * 5
_ = "Test 1. Check system environment"
sys.stderr.write(_+"... ")
try:
    import _base
    import _util
    import Bio.SubsMat.MatrixInfo as MatrixInfo
    import _custom_matrices
    import os.path
    import bioformats
    import json
    from bioformats import BioFormatException
    assert os.name=="nt" or os.name=="posix"
    assert sys.version_info.major==2
except:
    failed[0] = True
if failed[0]: sys.stderr.write("FAILED\n")
else: sys.stderr.write("PASSED\n")

RESOURCE_PATH = os.path.dirname(__file__)+"/test_data"

_ = "Test 2. Parse correct sequences"
sys.stderr.write(_+"... ")
test_items = os.listdir(RESOURCE_PATH+"/test01/in/")
f = open(RESOURCE_PATH+"/test01/references/reference.json", "r")
reference = json.load(f)
f.close()
for item in test_items:
    try:
        sqs = bioformats.read_sequences_from_file(RESOURCE_PATH+"/test01/in/"+item)
        seqs = dict()
        for name, value in sqs:
            seqs[name.lower()] = value
        assert len(seqs.keys())==len(reference.keys())
        for key, value in reference.items():
            assert key in seqs
            assert seqs[key]==value
    except:
        failed[1] = True
if failed[1]: sys.stderr.write("FAILED\n")
else: sys.stderr.write("PASSED\n")

_ = "Test 3. Try to parse incorrect sequences"
sys.stderr.write(_+"... ")
test_items = os.listdir(RESOURCE_PATH+"/test01/in/")
has_test2_error = False
for item in test_items:
    try:
        bioformats.read_sequences_from_file(RESOURCE_PATH+"/test02/in/"+item)
        has_test2_error = False
    except:
        has_test2_error = True
failed[2] = not has_test2_error
if failed[2]: sys.stderr.write("FAILED\n")
else: sys.stderr.write("PASSED\n")

_ = "Test 4. Build alignments"
sys.stderr.write(_+" (please wait, it takes some time)... ")
try:
    matrix = MatrixInfo.blosum62
    seq_names = reference.keys()
    alns = _base.get_pareto_alignments(reference[seq_names[0]], reference[seq_names[1]], 1.0, matrix, 40)
    err = _base.get_last_error()
    assert len(err)==0
    gops = _util.calculate_gops(alns)
except:
    failed[3] = True
if failed[3]: sys.stderr.write("FAILED\n")
else: sys.stderr.write("PASSED\n")

_ = "Test 5. Check output formats"
sys.stderr.write(_+"... ")
try:
    for outfmt in ["CLUSTAL", "FASTA", "FASTQ", "EMBOSS"]:
        for index, al in enumerate(alns):
            gop = gops[index]
            s = bioformats.proteins_alignment_to_string(
                al.data,
                reference[seq_names[0]],
                reference[seq_names[1]],
                seq_names[0],
                seq_names[1],
                outfmt,
                matrix,
                "BLOSUM62",
                1.0,
                gop,
                None,
                "selftest",
                None,
                al.m
            )
            f = open(RESOURCE_PATH+"/test03/ref/reference"+str(index)+"."+outfmt.lower(), "r")
            ref_s = f.read()
            f.close()
            assert s==ref_s
except:
    failed[4] = True
if failed[4]: sys.stderr.write("FAILED\n")
else: sys.stderr.write("PASSED\n")

if True in failed:
    sys.stderr.write("SUMMARY: Some of tests failed!\n")
    sys.exit(30)
else:
    sys.stderr.write("SUMMARY: All tests passed successfully.\n")
    sys.exit(0)

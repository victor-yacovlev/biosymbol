Ext.require('Ext.panel.*');
Ext.require('Ext.form.*');
Ext.require('Ext.data.*');
Ext.require('Ext.*');
Ext.require('Ext.window.*');

Ext.define('Sufpref.FilePreload',{
    extend: 'Ext.form.Panel',
    targetValueObject: null,
    width: 200,
    border: 0,
    items: [
        {
            xtype: 'filefield',
            fieldLabel: 'From file',
            buttonOnly: true,
            buttonText: 'Load...',
            name: 'file',
            targetValueObject: this.targetValueObject,
            listeners: {
                change: function(field, value, opts) {
                    var formCmp = field.up('form');
                    Ext.getCmp(formCmp.targetValueObject).setLoading(true);
                    
                    formCmp.submit({
                        url: 'cgi/sufpref_requests.py?method=echo',
                        targetValueObject: formCmp.targetValueObject,
                        btn: field,
                        success: function(form, action) {
                            var target = Ext.getCmp(this.targetValueObject);
                            target.setValue(action.result.data.replace(/&lt;/g, "<").replace(/&gt;/g, ">").replace(/&nbsp;/g, " "));
                            target.setLoading(false);
                            
                        },
                        failure: function(form, action) {
                            var target = Ext.getCmp(this.targetValueObject);
                            target.setLoading(false);
                            
                        }
                    })
                }
            }
        }
    ]
});

Ext.define('Sufpref.NewTaskWidget', {
    extend: 'Ext.form.Panel',
    layout: { type: 'vbox', align: 'center' },
    title: 'SufPref',
    id: 'sufpref_newtaskForm',
    reserveScrollbar: true,
    manageOverflow: 1,
    autoScroll: true,
    items: [
        {
            xtype: 'textfield',
            fieldLabel: "Job comment",
            name: 'comment'
        },
        {
            xtype: 'combo',
            store: Ext.create('Ext.data.Store', {
                fields: ["name", "alphabet"],
                data: [
                    { name: "DNA", alphabet: "ATGC" },
                    { name: "Amino-acid", alphabet: "ARNDCEQGHILKMFPSTWYV" }
                ]
            }),
            fieldLabel: 'Alphabet',
            displayField: 'name',
            valueField: 'alphabet',
            value: 'ATGC',
            editable: false,
            name: 'alphabet'
        },
        {
            xtype: 'numberfield',
            fieldLabel: 'Text length',
            value: 100,
            minValue: 1,
            allowDecimals: false,
            name: 'textlength'
        },
        {
            xtype: 'numberfield',
            fieldLabel: 'Number of occurences',
            value: 1,
            minValue: 1,
            allowDecimals: false,
            name: 'occurences'
        },
        {
            xtype: 'combo',
            store: Ext.create('Ext.data.Store', {
                fields: ["name", "value"],
                data: [
                    { name: "Hidden Markov Model (HMM)", value: "hmm" },
                    { name: "Deterministic HMM", value: "dhmm" },
                    { name: "Bernoulli", value: "bernoulli" },
                    { name: "Markovian Model", value: "markov" }
                ]
            }),
            fieldLabel: 'Probability model',
            displayField: 'name',
            valueField: 'value',
            value: 'hmm',
            editable: false,
            name: 'model',
            listeners: {
                change: function(combo, newValue, oldValue, opts) {
                    var id = "sufpref_model_"+newValue;
                    var modelDetails = Ext.getCmp("sufpref_modeldetails");
                    var page = Ext.getCmp(id);
                    modelDetails.getLayout().setActiveItem(page);
                }
            }
        },
        {
            xtype: 'combo',
            store: Ext.create('Ext.data.Store', {
                fields: ["name", "value"],
                data: [
                    { name: "List of words", value: "list" },
                    { name: "Random generated pattern", value: "random" },
                    { name: "PSSM and cut-off", value: "pssm/cutoff" },
                    { name: "PSSM and footprints", value: "pssm/footprints" },
                    { name: "Word and mismatches", value: "word/mismatches" },
                    { name: "Consensus", value: "consensus" }
                ]
            }),
            fieldLabel: 'Pattern',
            displayField: 'name',
            valueField: 'value',
            editable: false,
            value: 'list',
            name: 'pattern',
            listeners: {
                change: function(combo, newValue, oldValue, opts) {
                    var id = "sufpref_pattern_"+newValue;
                    var details = Ext.getCmp("sufpref_patterndetails");
                    var page = Ext.getCmp(id);
                    details.getLayout().setActiveItem(page);
                }
            }
        },
        {
            id: "sufpref_modeldetails",
            xtype: 'fieldset',
            title: 'Model details',
            collapsible: true,
            width: 260,
            layout: { type: 'card' },
            items: [
                {
                    id: "sufpref_model_hmm",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            id: 'sufpref_hmm_probs',
                            xtype: 'textarea',
                            name: 'hmm_probs',
                            fieldLabel: 'Probability distribution for HHM'
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_hmm_probs'
                        })
                    ]
                },
                {
                    id: "sufpref_model_dhmm",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            id: 'sufpref_dhmm_probs',
                            xtype: 'textarea',
                            name: 'dhmm_probs',
                            fieldLabel: 'Probabilities'
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_dhmm_probs'
                        }),
                        {
                            id: 'sufpref_dhmm_trans',
                            xtype: 'textarea',
                            name: 'dhmm_trans',
                            fieldLabel: 'Transitions'
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_dhmm_trans'
                        })
                    ]
                },
                {
                    id: "sufpref_model_bernoulli",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            id: 'sufpref_bernoulli_probs',
                            xtype: 'textfield',
                            name: 'bernoulli_probs',
                            fieldLabel: 'Probabilities'
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_bernoulli_probs'
                        })
                    ]
                },
                {
                    id: "sufpref_model_markov",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            xtype: 'numberfield',
                            allowDecimals: false,
                            value: 1,
                            name: 'markov_order',
                            fieldLabel: 'Custom order value'
                        },
                        {
                            id: 'sufpref_markov_probs',
                            xtype: 'textarea',
                            name: 'markov_probs',
                            fieldLabel: 'Probability distribution for Markovian model'
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_markov_probs'
                        }),
                        {
                            id: 'sufpref_markov_initials',
                            xtype: 'textfield',
                            name: 'markov_initials',
                            fieldLabel: 'Initial probabilities for Markovian model:'
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_markov_initials'
                        })
                    ]
                }
            ]
        },
        {
            id: 'sufpref_patterndetails',
            xtype: 'fieldset',
            title: 'Pattern details',
            collapsible: true,
            width: 260,
            layout: { type: 'card' },
            items: [
                {
                    id: "sufpref_pattern_list",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            id: 'sufpref_list_probs',
                            xtype: 'textarea',
                            name: "list_words",
                            fieldLabel: "List of words"
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_list_probs'
                        })
                    ]
                },
                {
                    id: "sufpref_pattern_random",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            xtype: 'numberfield',
                            name: 'random_count',
                            fieldLabel: "Number of words",
                            allowDecimals: false,
                            value: 10,
                            minValue: 1
                        },
                        {
                            xtype: 'numberfield',
                            name: 'random_length',
                            fieldLabel: "Length of words",
                            allowDecimals: false,
                            value: 4,
                            minValue: 1
                        },
                        {
                            id: 'sufpref_random_probs',
                            xtype: 'textfield',
                            name: "random_probs",
                            fieldLabel: "Probabilities of letters in the pattern (Bernoulli)"
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_random_probs'
                        })
                    ]
                },
                {
                    id: "sufpref_pattern_pssm/cutoff",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            xtype: 'numberfield',
                            name: 'pssm_cutoff_cutoff',
                            fieldLabel: "Cut-off",
                            allowDecimals: false,
                            value: 5,
                            minValue: 0
                        },
                        {
                            id: 'sufpref_pssm_cutoff_pssm',
                            xtype: 'textarea',
                            name: 'pssm_cutoff_pssm',
                            fieldLabel: "PSSM"
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_pssm_cutoff_pssm'
                        })
                    ]
                },
                {
                    id: "sufpref_pattern_pssm/footprints",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            id: 'sufpref_pssm_footprints_footprints',
                            xtype: 'textarea',
                            name: 'pssm_footprints_footprints',
                            fieldLabel: "Footprints"
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_pssm_footprints_footprints'
                        }),
                        {
                            id: 'sufpref_pssm_footprints_pssm',
                            xtype: 'textarea',
                            name: 'pssm_footprints_pssm',
                            fieldLabel: "PSSM"
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_pssm_footprints_pssm'
                        })
                    ]
                },
                {
                    id: "sufpref_pattern_word/mismatches",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            xtype: 'textfield',
                            name: 'word_mismatches_word',
                            fieldLabel: "Word"
                        },
                        {
                            xtype: 'numberfield',
                            name: 'word_mismatches_mismatches',
                            fieldLabel: "Maximum number of mismatches",
                            allowDecimals: false,
                            minValue: 0
                        },
                        {
                            xtype: 'textfield',
                            name: 'word_mismatches_constpositions',
                            fieldLabel: "Unchangeable positions in the word"
                        }
                    ]
                },
                {
                    id: "sufpref_pattern_consensus",
                    xtype: 'container',
                    layout: 'form',
                    items: [
                        {
                            id: 'sufpref_consensus_alphabet',
                            xtype: 'textarea',
                            name: 'consensus_alphabet',
                            fieldLabel: "Consensus alphabet"
                            
                        },
                        Ext.create('Sufpref.FilePreload', {
                            targetValueObject: 'sufpref_consensus_alphabet'
                        }),
                        {
                            xtype: 'textfield',
                            name: 'consensus_consensus',
                            fieldLabel: "Consensus (a word in consensus alphabet)"
                        }
                    ]
                }
            ]
        }
        ,
        {
            xtype: 'button',
            text: 'Submit new task',
            width: 260,
            parse2Dmatrix: function(text, is_integer) {
                var matrix = [];
                var error = false;
                var lines = text===undefined? [] : text.split("\n");
                for (var i=0; i<lines.length; i++) {
                    var line = lines[i];
                    if (error)
                        break;
                    if (line.trim().length==0)
                        continue;
                    var terms = line.split(/\s+/);
                    var row = [];
                    for (var j=0; j<terms.length; j++) {
                        if (terms[j].trim().length==0)
                            continue;
                        var term = terms[j];
                        if (error)
                            break;
                        var value = NaN;
                        if (is_integer)
                            value = parseInt(term);
                        else
                            value = parseFloat(term);
                        if (isNaN(value))
                            error = true;
                        row.push(value);
                    }
                    matrix.push(row);
                }
                return { error: error, data: matrix };
            },
            checkHMM: function() {
                var form = this.up('form').getForm().getValues();
                var hmm_lines = form.hmm_probs.split("\n");
                if (hmm_lines.length==0) {
                    return { error: "HMM probabilities is empty", data: null };
                }
                var statesNo = parseInt(hmm_lines[0]);
                if (isNaN(statesNo) || statesNo<0) {
                    return { error: "Invalid HMM probabilities format", data: null };
                }
                var states = [];
                var lineNo = 1;
                var curState = [];
                var error = "";
                for (var i=1; i<hmm_lines.length; i++) {
                    var curLine = hmm_lines[i].trim();
                    if (error.length>0)
                        break;
                    if (curLine.length==0)
                        continue;
                    var terms = curLine.split(/\s+/);
                    if (terms.length==1 && terms[0].charAt(0)=='-') {
                        if (curState.length>0) {
                            states.push(curState);
                            curState = [];
                        }
                        continue;
                    }
                    var row = [];
                    for (var j=0; j<terms.length; j++) {
                        if (error.length>0)
                            break;
                        var val = parseFloat(terms[j]);
                        if (isNaN(val)) {
                            error = "Probability distribution of HMM contains non floating-point value";
                            break;
                        }
                        row.push(val);
                    }
                    curState.push(row);
                }
                if (curState.length>0)
                    states.push(curState);
                // Check correctness of data
                var as = form.alphabet.length;
                for (var k=0; k<states.length; k++) {
                    if (error.length>0)
                        break;
                    if (states[k].length!=states.length) {
                        error = "HMM Probs: matrix "+(k+1)+" rows count doesn't match states count";
                        break;
                    }
                    var matrix = states[k];
                    for (var i=0; i<matrix.length; i++) {
                        var row = matrix[i];
                        if (row.length!=as) {
                            error = "HMM Probs: matrix "+(k+1)+" columns count doesn't match alphabet size";
                            break;
                        }
                    }
                }
                return { error: error, data: states };
            },
            checkDHMM: function() {
                var form = this.up('form').getForm().getValues();
                var error = "";
                var dhmm_probs = this.parse2Dmatrix(form.dhmm_probs, false);
                if (dhmm_probs.error) {
                    error = "Probabilities of Determenistic HMM contains non floating-point value";
                    return { error: error, probs: undefined, trans: undefined };
                }
                var dhmm_trans = this.parse2Dmatrix(form.hmm_trans, true);
                if (dhmm_trans.error) {
                    error = "Transitions of Determenistic HMM contains non integer value";
                    return { error: error, probs: undefined, trans: undefined };
                }
                // Check:
                // 1) dhhm_p and dhhm_t sizes are equal
                // 2) dhhm_p/dhhm_t columns count == alphabet size
                if (dhmm_probs.data.length!=dhmm_trans.data.length) {
                    return {
                        error: "Determenistic HMM: Probabilities and transitions matrices have different rows count",
                        probs: undefined,
                        trans: undefined
                    }
                }
                var as = form.alphabet.length;
                for (var i=0; i<dhmm_probs.data.length; i++) {
                    var probs_row = dhmm_probs.data[i];
                    var trans_row = dhmm_trans.data[i];
                    if (probs_row.length!=as) {
                        return {
                            error: "Determenistic HMM: Probabilities matrix columns count doesn't match alphabet size",
                            probs: undefined,
                            trans: undefined
                        }
                    }
                    if (trans_row.length!=as) {
                        return {
                            error: "Determenistic HMM: Transitions matrix columns count doesn't match alphabet size",
                            probs: undefined,
                            trans: undefined
                        }
                    }
                }
                return {
                    error: "",
                    probs: dhmm_probs.data,
                    trans: dhmm_trans.data
                };
            },
            checkMarkov: function() {
                var form = this.up('form').getForm().getValues();
                var error = "";
                var mark = this.parse2Dmatrix(form.markov_probs, false);
                if (mark.error) {
                    return {
                        error: "Probability distribution of Markovian model contains non floating-point value",
                        probs: undefined,
                        initial_probs: undefined
                    };
                }
                var initials = [];
                var terms = [];
                if (form.markov_initials)
                    terms = form.markov_initials.split(/\s+/);
                for (var i=0; i<terms.length; i++) {
                    if (terms[i].trim().length==0)
                        continue;
                    var value = parseFloat(terms[i]);
                    if (isNaN(value)) {
                        return {
                            error: "Initial probabilities of Markovian model contains non floating-point value",
                            probs: undefined,
                            initial_probs: undefined
                        };
                    }
                    initials.push(value);
                }
                // Check
                // 1) columns count == alphabet size
                // 2) rows count == alphabet size ** order value
                // 3) mark_ini_probs (if not empty) size = AlpSize**order
                var as = form.alphabet.length;
                var order = form.markov_order;
                var pwr = Math.pow(as, order);
                if (mark.data.length!=pwr) {
                    return {
                        error: "Markovian Model: probability distribution matrix rows count doesn't equal alphabet size in power of order value",
                        probs: undefined,
                        initial_probs: undefined
                    };
                }
                for (var i=0; i<mark.data.length; i++) {
                    var row = mark.data[i];
                    if (row.length!=as) {
                        return {
                            error: "Markovian Model: probability distribution matrix columns count doesn't match alphabet size",
                            probs: undefined,
                            initial_probs: undefined
                        };
                    }
                }
                if (initials.length>0 && initials.length!=pwr) {
                    return {
                        error: "Markovian Model: initial probabilities count doesn't equal alphabet size in power of order value",
                        probs: undefined,
                        initial_probs: undefined
                    };
                }
                return {
                    error: "",
                    probs: mark.data,
                    initial_probs : initials
                };
            },
            checkModel: function() {
                var form = this.up('form').getForm().getValues();
                var error = "";
                var model = {
                    "type": form.model
                }
                if (model.type=="hmm") {
                    hmm = this.checkHMM();
                    if (hmm.error.length>0) {
                        error = hmm.error;
                    }
                    else {
                        model.probs = hmm.data; 
                    }
                }
                if (model.type=="dhmm") {
                    dhmm = this.checkDHMM();
                    if (dhhm.error.length>0) {
                        error = dhmm.error;
                    }
                    else {
                        model.probs = dhmm.probs;
                        model.trans = dhmm.trans;
                    }
                }
                if (model.type=="bernoulli") {
                    var terms = [];
                    if (form.bernoulli_probs)
                        terms = form.bernoulli_probs.split(/\s+/);
                    var vals = [];
                    for (var i=0; i<terms.length; i++) {
                        if (terms[i].trim().length==0)
                            continue;
                        var value = parseFloat(terms[i]);
                        if (isNaN(value)) {
                            error = "Probability distribution of Bernoulli contains non floating-point value";
                            break;
                        }
                        else {
                            vals.push(value);
                        }
                    }
                    if (error.length==0) {
                        var as = form.alphabet.length;
                        if (vals.length!=as) {
                            error = "Bernoulli: Probability distribution elements count doesn't match alphabet size";
                        }
                        else {
                            model.probs = vals;
                        }
                    }
                }
                if (model.type=="markov") {
                    var mark = this.checkMarkov();
                    if (mark.error.length>0) {
                        error = mark.error;
                    }
                    else {
                        model.probs = mark.probs;
                        model.initial_probs = mark.initial_probs;
                        model.order = form.markov_order;
                    }
                }
                return {
                    error : error,
                    model : model
                };
            },
            checkPattern: function() {
                var form = this.up('form').getForm().getValues();
                var error = "";
                var pattern = {
                    "type": form.pattern
                };
                if (pattern.type=="list") {
                    pattern.words = [];
                    if (form.list_words)
                        pattern.words = form.list_words.split("\n")
                    else
                        error = "List of words pattern: must be not empty";
                }
                if (pattern.type=="random") {
                    var terms = form.random_probs.split(/\s+/);
                    vals = []
                    for (var i=0; i<terms.length; i++) {
                        var value = parseFloat(terms[i]);
                        if (isNaN(value)) {
                            error = "Random generated pattern: not floating-point value in list";
                            break;
                        }
                    }
                    var as = form.alphabet.length;
                    if (vals.length!=as) {
                        error = "Random generated pattern: Probability distribution elements count doesn't match alphabet size";
                    }
                    pattern.probs = vals;
                    pattern.word_length = form.random_length;
                    pattern.word_count = form.random_count;
                }
                if (pattern.type=="pssm/cutoff") {
                    pattern.cutoff = form.pssm_cutoff_cutoff;
                    pssm = this.parse2Dmatrix(form.pssm_cutoff_pssm, false);
                    if (pssm.error) {
                        error = "PSSM and cut-off: PSSM contains non floating-point value";
                    }
                    else {
                        pattern.pssm = pssm.data;
                        if (pssm.data.length==0) {
                            error = "PSSM and cut-off: PSSM matrix is empty";
                        }
                        else {
                            var as = form.alphabet.length;
                            for (var i=0; i<pssm.data.length; i++) {
                                var row = pssm.data[i];
                                if (row.length!=as) {
                                    error = "PSSM and cut-off: PSSM columns count doesn't match alphabet size";
                                    break;
                                }
                            }
                        }
                    }
                }
                if (pattern.type=="pssm/footprints") {
                    pssm = this.parse2Dmatrix(form.pssm_footprints_pssm, false);
                    if (pssm.error) {
                        error = "PSSM and footprints: PSSM contains non floating-point value";
                    }
                    else {
                        pattern.pssm = pssm.data;
                        if (pssm.data.length==0) {
                            error = "PSSM and footprints: PSSM matrix is empty";
                        }
                        else {
                            var as = form.alphabet.length;
                            for (var i=0; i<pssm.data.length; i++) {
                                var row = pssm.data[i];
                                if (row.length!=as) {
                                    error = "PSSM and footprints: PSSM columns count doesn't match alphabet size";
                                    break;
                                }
                            }
                        }
                    }
                    pattern.footprints = []
                    var fplines = form.pssm_footprints_footprints.split("\n");
                    for (var i=0; i<fplines.length; i++) {
                        var line = fplines[i].replace(/\s+/g, "").toUpperCase();
                        var alph = form.alphabet;
                        for (var j=0; j<line.length; j++) {
                            var ch = line.charAt(j);
                            if (alph.indexOf(ch)==-1) {
                                error = "PSSM and footprints: footprint at line "+(i+1)+" contains a symbol out of alphabet";
                                break;
                            }
                        }
                        if (line.length>0 && error.length==0)
                            pattern.footprints.push(line);
                    }
                }
                if (pattern.type=="word/mismatches") {
                    pattern.word = form.word_mismatches_word.replace(/\s+/g, "").toUpperCase();
                    var alph = form.alphabet;
                    for (var j=0; j<pattern.word.length; j++) {
                        var ch = pattern.word.charAt(j);
                        if (alph.indexOf(ch)==-1) {
                            error = "Word and mismatches: word contains a symbol out of alphabet";
                            break;
                        }
                    }
                    pattern.mismatches = form.word_mismatches_mismatches;
                    pattern.constpositions = [];
                    var cps = []
                    if (form.word_mismatches_constpositions)
                        cps = form.word_mismatches_constpositions.split(/\s+/);
                    for (var i=0; i<cps.length; i++) {
                        var value = parseInt(cps[i]);
                        if (isNaN(value)) {
                            error = "Word and mismatches: unchangeable positions contains non-integer value";
                            break;
                        }
                        pattern.constpositions.push(value);
                    }
                }
                if (pattern.type=="consensus") {
                    var consalp_lines = form.consensus_alphabet.replace(/\[/g,"{").replace(/\(/g,"{").replace(/\]/g,"}").replace(/\)/g,"}").toUpperCase().split("\n");
                    pattern.alphabet = new Hash();
                    for (var i=0; i<consalp_lines.length; i++) {
                        var lexems = consalp_lines[i].split("=");
                        if (lexems.length!=2)
                            continue;
                        var left = lexems[0].trim();
                        var right = lexems[1].replace(/\s+/g,"").replace(/\{/g,"").replace(/\}/g,"");
                        var terms = right.split(",");
                        pattern.alphabet[left] = right;
                    }
                    var alp = form.alphabet;
                    for (var i=0; i<pattern.alphabet.keys().length; i++) {
                        if (error.length>0)
                            break;
                        var key = pattern.alphabet.keys()[i];
                        if (key.length!=1) {
                            error = "Illegal symbol in left part of consensus alphabet: '"+key+"'";
                            break;
                        }
                        var right = pattern.alphabet[key];
                        for (var j=0; j<right.length; j++) {
                            var symbol = right[j];
                            if (symbol.length!=1) {
                                error = "Illegal symbol in right part of consensus alphabet: '"+symbol+"'";
                                break;
                            }
                            if (alp.indexOf(symbol)==-1) {
                                error = "Symbol in right part of consensus out of alphabet: '"+symbol+"'";
                                break;
                            }
                        }
                        var allsyms = right.join("");
                        for (var j=0; j<allsyms.length; j++) {
                            var sym = allsyms[j];
                            if (allsyms.count(sym)>1) {
                                error = "Duplicate symbol in right part of consensus out of alphabet: '"+sym+"'";
                                break;
                            }
                        }
                    }
                }
                return {
                    error : error,
                    pattern: pattern
                };
            },
            handler: function() {
                var model = this.checkModel();
                var pattern = this.checkPattern();
                var error = "";
                var form = this.up('form').getForm().getValues();
                if (model.error.length>0) {
                    error = model.error;
                }
                else if (pattern.error.length>0) {
                    error = pattern.error;
                }
                var params = {
                    method: "add_new_task",
                    comment: form.comment,
                    alphabet: form.alphabet,
                    textlength: form.textlength,
                    occurences: form.occurences,
                    model: Ext.encode(model.model),
                    pattern: Ext.encode(pattern.pattern)
                };
                if (error.length>0) {
                    Ext.Msg.show({
                        title: 'ERROR: Input data not valid',
                        msg: error,
                        width: 300,
                        buttons: Ext.Msg.OK,
                        icon: Ext.window.MessageBox.ERROR
                    });
                }
                else {
                    //this.up('form').setLoading(true);
                    Ext.Ajax.request({
                        url: "cgi/sufpref_requests.py",
                        method: "POST",
                        params: params,
                        formCmp: this.up('form'),
                        callback: function(options, success, response) {
                            //this.formCmp.setLoading(false);
                            var allOK = success;
                            var error = "";
                            var resp = null;
                            if (allOK) {
                                resp = Ext.decode(response.responseText);
                                allOK = allOK && resp.status=="OK";
                            }
                            else {
                                error = "Can't connect to sever";
                            }
                            var id = null;
                            if (allOK) {
                                id = resp.id;
                            }
                            else {
                                error = "Error at server";
                            }
                            
                            if (allOK) {
                                Ext.Msg.alert("Task created",
                                              "Task successfully added to server with ID = "+id+".");
                            }
                            else {
                                Ext.Msg.alert("Error", error);
                            }
                            var store = Ext.data.StoreManager.lookup("tasksSummary");
                            store.load();
                        }
                    })
                }
                
            }
        }
    ]
});
Ext.require("Ext.window.*")
Ext.require("Ext.panel.*")
Ext.require("Ext.form.*")
Ext.require("Ext.button.*")
Ext.require("Ext.container.*")
Ext.require("Ext.data.*")
Ext.require("Ext.Ajax.*")
Ext.require("Ext.Msg.*")
Ext.require("Ext.History");

Ext.define("Parca.NewTaskWindow", {
    extend: 'Ext.form.Panel',
    id: 'parcaNewTaskWindow',
    title: 'PARCA',
    reserveScrollbar: true,
    manageOverflow: 1,
    autoScroll: true,
    width: 300,
    layout: { type: 'vbox', align: 'center' },
    defaultType: 'textfield',
    padding: 0,
    defaults: {
        width: 230
    },
    items: [
        Ext.create('Ext.form.Label', {
            forId: 'parca_comment',
            text: "Job comment:"
        }),
        Ext.create('Ext.form.field.Text', {
            id: 'parca_comment',
            name: 'comment'
        }),
        Ext.create('Ext.form.Label', {
            forId: 'parca_name1',
            text: "First sequence name:"
        }),
        Ext.create('Ext.form.field.Text', {
            id: 'parca_name1',
            name: 'name1'
        }),
        Ext.create('Ext.form.Label', {
            forId: 'parca_seq1',
            text: "First sequence text:"
        }),
        Ext.create('Ext.form.field.TextArea', {
            id: 'parca_seq1',
            name: 'seq1'
        }),
        Ext.create('Ext.form.Label', {
            forId: 'parca_name2',
            text: "Second sequence name:"
        }),
        Ext.create('Ext.form.field.Text', {
            id: 'parca_name2',
            name: 'name2'
        }),
        Ext.create('Ext.form.Label', {
            forId: 'parca_seq2',
            text: "Second sequence text:"
        }),
        Ext.create('Ext.form.field.TextArea', {
            id: 'parca_seq2',
            name: 'seq2'
        }),
        Ext.create('Ext.button.Button', {
            text: "Load sequences from file...",
            handler: function() { Ext.History.add("Parca.SequencesChooser:0"); }
        }),
        
        Ext.create('Ext.form.FieldSet', {
            title: "Advanced options",
            collapsible: true,
            collapsed: true,
            defaults: {
                width: 180
            },
            items: [
                Ext.create('Ext.form.Label', {
                    forId: 'parca_matrix',
                    text: "Substitution matrix to use:"
                }),
                Ext.create('Ext.form.field.ComboBox', {
                    id: "parca_matrix",
                    name: "matrix",
                    store: Ext.create('Ext.data.Store', {
                        
                        fields: ['name'],
                        data: [
                            {"name": "blosum62"},
                            {"name": "pam120"},
                            {"name": "pam360"},
                            {"name": "pam240"},
                            {"name": "pam480"}]
                        
                    }),
                    queryMode: 'local',
                    displayField: 'name',
                    valueField: 'name',
                    forceSelection: true,
                    value: "blosum62",
                    editable: false
                }),
                Ext.create('Ext.form.Label', {
                    forId: 'parca_limit',
                    text: "Gap Limit:"
                }),
                Ext.create('Ext.form.field.Number', {
                    id: "parca_limit",
                    name: "limit",
                    value: 40,
                    maxValue: 80,
                    minValue: 1
                }),
                Ext.create('Ext.form.Label', {
                    forId: 'parca_gep',
                    text: "Gap Extend Penalty:"
                }),
                Ext.create('Ext.form.field.Number', {
                    id: "parca_gep",
                    name: "gep",
                    value: 1,
                    maxValue: 3,
                    minValue: 0
                }),
            ]
        }),
        Ext.create('Ext.form.field.Hidden', {
            name: "method",
            value: "add_new_job"
        }),
        Ext.create('Ext.button.Button',{
            text: 'Submit New Task',
            handler: function() {
                var form = this.up('form').getForm();
                var vals = this.up('form').getForm().getValues();
                var isCorrect = true;
                var errorMessage = "";
                checkSequence = function(seq) {
                    var alphabet = "ARNDCEQGHILKMFPSTWYV";
                    var ss = seq.toUpperCase().trim();
                    for (var i=0; i<ss.length; i++) {
                        var ch = ss.charAt(i);
                        if (alphabet.indexOf(ch)==-1) {
                            return false;
                        }
                    }
                    return true;
                }
                var seq1 = vals.seq1;
                var seq2 = vals.seq2;
                if (!checkSequence(seq1)) {
                    errorMessage = "First sequence contains illegal character";
                    isCorrect = false;
                }
                else if (!checkSequence(seq2)) {
                    errorMessage = "Second sequence contains illegal character";
                    isCorrect = false;
                }
                else if (seq1.trim().length>500) {
                    errorMessage = "First sequence too long";
                    isCorrect = false;
                }
                else if (seq2.trim().length>500) {
                    errorMessage = "Second sequence too long";
                    isCorrect = false;
                }
                else if (seq1.trim().length==0) {
                    errorMessage = "First sequence is empty";
                    isCorrect = false;
                }
                else if (seq2.trim().length==0) {
                    errorMessage = "Second sequence is empty";
                    isCorrect = false;
                }
                if (isCorrect) {
                    this.up('form').setLoading(true);
                    Ext.Ajax.request({
                        url: "cgi/parca_requests.py",
                        method: "POST",
                        params: vals,
                        callback: function(options, success, response) {
                            Ext.getCmp('parcaNewTaskWindow').setLoading(false);
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
                else {
                    Ext.Msg.show({
                        title: 'ERROR: Input data not valid',
                        msg: errorMessage,
                        width: 300,
                        buttons: Ext.Msg.OK,
                        icon: Ext.window.MessageBox.ERROR
                    });
                }
            }
        })
        
    ]



})

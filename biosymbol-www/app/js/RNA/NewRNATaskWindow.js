Ext.require("Ext.window.*")
Ext.require("Ext.panel.*")
Ext.require("Ext.form.*")
Ext.require("Ext.button.*")
Ext.require("Ext.container.*")
Ext.require("Ext.data.*")
Ext.require("Ext.Ajax.*")
Ext.require("Ext.Msg.*")
Ext.require("Ext.History");

Ext.define("RNA.NewRNATaskWindow", {
    extend: 'Ext.form.Panel',
    
    title: 'RNA',
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
    standardSubmit: true,
    items: [
        Ext.create('Ext.form.Label', {
            forId: 'rna_comment',
            text: "Job comment:"
        }),
        Ext.create('Ext.form.field.Text', {
            id: 'rna_comment',
            name: 'comment'
        }),
        Ext.create('Ext.form.Label', {
            forId: 'rna_frame_size',
            text: "Frame size:"
        }),
        Ext.create('Ext.form.field.Number', {
            id: 'rna_frame_size',
            name: 'frame_size',
            minValue: 1000,
            maxValue: 10000,
            value: 3000,
            allowDecimals: false
        }),
        Ext.create('Ext.form.Label', {
            forId: 'rna_frame_shift',
            text: "Frame shift:"
        }),
        Ext.create('Ext.form.field.Number', {
            id: 'rna_frame_shift',
            name: 'frame_shift',
            minValue: 500,
            maxValue: 5000,
            value: 1000,
            allowDecimals: false
        }),
        Ext.create('Ext.form.Label', {
            forId: 'rna_seed',
            text: "Seed:"
        }),
        Ext.create('Ext.form.field.Number', {
            id: 'rna_seed',
            name: 'seed',
            minValue: 1,
            maxValue: 7,
            value: 4,
            allowDecimals: false
        }),
        Ext.create('Ext.form.Label', {
            forId: 'rna_start_pos',
            text: "Start pos:"
        }),
        Ext.create('Ext.form.field.Number', {
            id: 'rna_start_pos',
            name: 'start_pos',
            minValue: 0,
            allowDecimals: false
        }),
        Ext.create('Ext.form.Label', {
            forId: 'rna_end_pos',
            text: "End pos:"
        }),
        Ext.create('Ext.form.field.Number', {
            id: 'rna_end_pos',
            name: 'end_pos',
            allowDecimals: false
        }),
        Ext.create('Ext.form.Label', {
            forId: 'rna_penalty',
            text: "Penalty:"
        }),
        Ext.create('Ext.form.field.Number', {
            id: 'rna_penalty',
            name: 'penalty',
            allowDecimals: true,
            minValue: 0.0,
            maxValue: 1.0,
            value: 0.25
        }),
        Ext.create('Ext.form.Label', {
            forId: 'rna_file',
            text: "Sequence file"
        }),
        Ext.create('Ext.form.field.File', {
            id: 'rna_file',
            name: 'file'
        }),
        Ext.create('Ext.form.field.Hidden', {
            name: "method",
            value: "add_new_job"
        }),
        Ext.create('Ext.button.Button',{
            text: 'Submit New Task',
            handler: function() {
                var form = this.up('form');
                var fileField = Ext.getCmp('rna_file');
                if (fileField.getValue()==undefined || fileField.getValue().trim().length==0) {
                    Ext.Msg.alert("Error", "Sequence file not specified");
                }
                else {
                    form.setLoading(true);
                    form.submit({
                        url: 'cgi/rna_requests.py',
                        method: 'POST',
                        formCmp: form,
                        success: function(request, action) {
                            Ext.Msg.alert("Task created",
                                           "Task successfully added to server with ID = "+action.result.id+".");
                            this.formCmp.setLoading(false);
                        },
                        failure: function(request, action) {
                            var error = action.result.error;
                            Ext.Msg.alert("Error", error);
                            this.formCmp.setLoading(false);
                        }
                    });
                }
            }
        })
        
    ]
})


Ext.require('Ext.window.*');
Ext.require('Ext.form.*');
Ext.require('Ext.data.*');
Ext.require('Ext.grid.*');
Ext.require('Ext.tree.*');
Ext.require('Ext.view.*');
Ext.require('Ext.container.*');
Ext.require('Ext.tab.*');
Ext.require('Ext.*');

Ext.define("Parca.SequencesChooser.EntryModel", {
    extend: 'Ext.data.Model',
    fields: [
        {name: 'parseError', type: 'string'},
        {name: 'sequenceData', type: 'string'},
        {name: 'sequenceName', type: 'string'},
        {name: 'fileName', type: 'string'},
        {name: 'text', type: 'string'}
    ]
});

Ext.define("Parca.SequencesChooser", {
    extend: "Ext.window.Window",
    id: "parcaSequencesChooser",
    closable: false,
    draggable: false,
    resizable: false,
    modal: true,
    width: 500,
    height: 400,
    title: "Load sequences from file...",
    layout: { type: 'vbox', align: 'stretch', pack: 'end' },
    items: [
        Ext.create("Ext.panel.Panel", {
            id: "uploadSequencesComponent",
            layout: 'card',
            header: false,
            flex: 1,
            activeItem: 0,
            items: [
                Ext.create("Ext.container.Container", {
                    id: "uploadSequencesFileTab",
                    layout: { type: 'vbox', align: 'center' },
                    width: 480,
                    items: [
                        Ext.create("Ext.container.Container", {
                            width: 460,
                            height: 40,
                            html: "<b>Step&nbsp;1.</b> Browse one or more files containing sequenses.<br/>"+
                            "NOTICE: Maximum summary files size is limited to 1Mb.<br/>"
                        }),
                        Ext.create("Ext.form.Panel", {
                            id: "sequencesFileUploader",
                            width: 460,
                            flex: 1,
                            baseParams: {
                                method: "parse_sequences"
                            },
                            reader: { type: 'json' },
                            url: "cgi/parca_requests.py",
                            layout: {
                                type: 'table',
                                columns: 2
                            },
                            items: [
                                //Ext.create("Ext.form.fields.Hidden", {
                                //    name: "method",
                                //    value: "parse_sequences"
                                //})
                            ],
                            updateFileFields: function() {
                                var formItems = Ext.getCmp("sequencesFileUploader").items.items;
                                var fileFields = [];
                                var removeFields = [];
                                for (var i=0; i<formItems.length; i++) {
                                    var item = formItems[i];
                                    if (item.name.substring(0,4)=="file") {
                                        fileFields.push(item);
                                    }
                                    else if (item.name.substring(0,6)=="remove") {
                                        removeFields.push(item);
                                    }
                                }
                                var counter = 0;
                                for (var i=0; i<fileFields.length; i++) {
                                    var fileItem = fileFields[i];
                                    var removeItem = removeFields[i];
                                    var fileName = fileItem.getValue();
                                    if (fileItem.number==-1 || fileName=="") {
                                        fileItem.destroy();
                                        removeItem.destroy();
                                    }
                                    else {
                                        removeItem.setDisabled(false);
                                        removeItem.name = "remove"+counter;
                                        fileItem.name = "file"+counter;
                                        removeItem.number = counter;
                                        fileItem.number= counter;
                                        counter ++;
                                    }
                                }
                                var form = Ext.getCmp("sequencesFileUploader");
                                form.add(Ext.create("Ext.form.field.File", {
                                    name: "file"+counter,
                                    width: 360,
                                    border: 0,
                                    number: counter,
                                    listeners: {
                                        change: function(field, value, opts) {
                                            Ext.getCmp("sequencesFileUploader").updateFileFields();
                                        }
                                    }
                                }));
                                form.add(Ext.create("Ext.button.Button", {
                                    text: "Remove",
                                    name: "remove"+counter,
                                    disabled: true,
                                    number: counter,
                                    handler: function() {
                                        var form = this.up('form');
                                        for (var i=0; i<form.items.items.length; i++) {
                                            var item = form.items.items[i];
                                            if (item.name.substring(0,4)=="file" && item.number==this.number) {
                                                item.number = -1;
                                                break;
                                            }
                                        }
                                        Ext.getCmp("sequencesFileUploader").updateFileFields();
                                    }
                                }));
                            }
                        })
                    ]
                }),
                {
                    xtype: "container",
                    id: "selectSequencesTab",
                    layout: { type: 'vbox', align: 'center' },
                    width: 480,
                    items: [
                        {
                            xtype: 'container',
                            width: 460,
                            height: 40,
                            html: "<b>Step&nbsp;2.</b> Select one or two sequences"
                        },
                        {
                            xtype: 'panel',
                            width: 460,
                            layout: { type: 'hbox', align: 'stretch' },
                            flex: 1,
                            items: [
                                Ext.create('Ext.tree.Panel', {
                                    id: 'selectSequencesTree',
                                    width: 200,
                                    rootVisible: false,
                                    store: Ext.create('Ext.data.TreeStore', {
                                        id: 'selectSequencesTabData',
                                        autoLoad: false,
                                        model: "Parca.SequencesChooser.EntryModel",
                                        proxy: { type: 'memory' }
                                    }),
                                    listeners: {
                                        select: function(panel, record, index, opts) {
                                            var preview = Ext.getCmp('selectSequencesPreview');
                                            preview.update(record.data);
                                        },
                                        checkchange: function(node, checked, opts) {
                                            var store = Ext.data.StoreManager.lookup("selectSequencesTabData");
                                            var checkedNodes = [];
                                            node.data.checked = checked;
                                            for (var i=0; i<store.tree.root.childNodes.length; i++) {
                                                var fileNode = store.tree.root.childNodes[i];
                                                for (var j=0; j<fileNode.childNodes.length; j++) {
                                                    var seqNode = fileNode.childNodes[j];
                                                    if (seqNode.data.checked && seqNode!=node) {
                                                        checkedNodes.push(seqNode);
                                                    }
                                                }
                                            }
                                            if (checked) {
                                                checkedNodes.push(node);
                                                var somethingUnchecked = false;
                                                for (var i=0; i<checkedNodes.length-2; i++) {
                                                    checkedNodes[i].data.checked = false;
                                                    checkedNodes[i].setDirty();
                                                    somethingUnchecked = true;
                                                }
                                                if (somethingUnchecked) {
                                                    store.sync();
                                                }
                                            }   
                                        }
                                    }
                                }),
                                {
                                    xtype: 'panel',
                                    id: 'selectSequencesPreview',
                                    flex: 1,
                                    html: '<div class="seqchooser-message">Select a sequence from left tree to preview or check it to use.</div>',
                                    data: undefined,
                                    tpl: '\
                                        <tpl if="leaf">\
                                            <div class="seqchooser-sequence-name">{sequenceName}</div>\
                                            <div class="seqchooser-sequence-preview">{sequenceData}</div>\
                                        <tpl elseif="parseError.length &gt; 0">\
                                            <div class="seqchooser-message">Error in this file: <div class="seqchooser-file-error">{parseError}</div></div>\
                                        <tpl else>\
                                            <div class="seqchooser-message">Select a sequence from left tree to preview or check it to use.</div>\
                                        </tpl>\
                                    '
                                }
                            ]
                        }
                        
                    ]
                }
            ]
        })],
        bbar: [ '->',
                {
                    id: 'seqChooserCancel',
                    xtype: "button",
                    text: "Cancel",
                    handler: function() {
                        var backCount = 1;
                        for (var i=0; i<backCount; i++)
                            Ext.History.back();
                    }
                },
                {
                    id: 'seqChooserNext',
                    xtype: "button",
                    text: "Next",
                    handler: function() {
                        var panel = Ext.getCmp("uploadSequencesComponent");
                        if (panel.activeItem==0) {
                            var uploaderForm = Ext.getCmp("sequencesFileUploader").getForm();
                            var chooser = Ext.getCmp("parcaSequencesChooser");
                            uploaderForm.submit({
                                url: "cgi/parca_requests.py",
                                params: {
                                    method: "parse_sequences"
                                },
                                success: chooser.onParseFilesResponse,
                                failure: chooser.onParseFilesResponse
                            });
                            if (!Ext.isOpera)
                                Ext.History.back();
                        }
                        else if (panel.activeItem==1) {
                            var tree = Ext.getCmp('selectSequencesTree');
                            var checkedItems = tree.getChecked();
                            var name1field = Ext.getCmp('parca_name1');
                            var name2field = Ext.getCmp('parca_name2');
                            var seq1field = Ext.getCmp('parca_seq1');
                            var seq2field = Ext.getCmp('parca_seq2');
                            if (checkedItems.length>0) {
                                name1field.setValue(checkedItems[0].data.sequenceName)
                                seq1field.setValue(checkedItems[0].data.sequenceData);
                            }
                            if (checkedItems.length>1) {
                                name2field.setValue(checkedItems[1].data.sequenceName)
                                seq2field.setValue(checkedItems[1].data.sequenceData);
                            }
                            Ext.History.add("");
                        }
                    }
                }
            ],
    listeners: {
        beforeshow: function(wnd, opts) {
            var panel = Ext.getCmp("uploadSequencesComponent");
            if (panel.activeItem==0) {
                var uploader = Ext.getCmp("sequencesFileUploader");
                uploader.updateFileFields();
            }
        }
    },
    onParseFilesResponse: function(form, action) {
        var res = action.result;
        var panel = Ext.getCmp("uploadSequencesComponent");
        var step2 = Ext.getCmp("selectSequencesTab");
        var step2data = Ext.data.StoreManager.lookup("selectSequencesTabData");
        var data = [];
        for (var i=0; i<res.result.length; i++) {
            var node = res.result[i];
            var filename = node.filename;
            var error = node.error;
            var seqs = node.sequences;
            var filenode = {
                text: filename,
                fileName: filename,
                //cls: "folder",
                leaf: false,
                expandable: true,
                expanded: true,
                children: [],
                parseError: error
            }
            
            for (var j=0; j<seqs.length; j++) {
                var seq = seqs[j];
                var seqnode = {
                    text: seq.name,
                    sequenceName: seq.name,
                    leaf: true,
                    sequenceData: seq.sequence,
                    checked: false
                }
                filenode.children.push(seqnode);
            }
            data.push(filenode);
        }
        var proxy = {
            type: 'memory',
            data: data
        };
        step2data.setProxy(proxy);
        step2data.load();
        panel.getLayout().setActiveItem(step2);
        panel.activeItem = 1;
        Ext.History.add("Parca.SequencesChooser:1");
    },
    setActiveIndex: function(index) {
        var panel = Ext.getCmp("uploadSequencesComponent");
        panel.activeItem = index;
        var step1 = Ext.getCmp("uploadSequencesFileTab");
        var step2 = Ext.getCmp("selectSequencesTab");
        var prev = Ext.getCmp("seqChooserCancel");
        var next = Ext.getCmp("seqChooserNext");
        if (panel.activeItem==0) {
            panel.getLayout().setActiveItem(step1);
            next.setText("Next");
        }
        else if (panel.activeItem==1) {
            panel.getLayout().setActiveItem(step2);
            next.setText("Finish");
        }
    }
});
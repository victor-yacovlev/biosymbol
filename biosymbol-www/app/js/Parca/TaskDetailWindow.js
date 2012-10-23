Ext.require('Ext.window.*');
Ext.require('Ext.grid.*');
Ext.require('Ext.panel.*');
Ext.require('Ext.chart.*');
Ext.require('Ext.data.*');
Ext.require('Ext.container.*');
Ext.require('Ext.toolbar.*');
Ext.require('Ext.view.*');
Ext.require('Ext.selection.*');
Ext.require('Ext.XTemplate');
Ext.require('Ext.History.*');

Ext.define('Parca.TaskDetailWindow', {
    id: "parcaTaskDetailWindow",
    extend: 'Ext.window.Window',
    layout: 'border',
    maximized: true,
    draggable: false,
    resizable: false,
    modal: true,
    closable: false,
    titleAlign: "center",
    tools: [{
        type: 'prev',
        xtype: 'button',
        icon: "extjs/resources/themes/images/default/shared/left-btn.gif",
        text: 'Go back to task list',
        handler: function(event, toolEl, panel) {
            Ext.History.back();
        }
    }],
    loadTaskData: function(taskId) {
        Ext.Ajax
        var store = Ext.data.StoreManager.lookup("parcaTaskDetails");
        store.setProxy({
            type: "ajax",
            url: "cgi/parca_requests.py?method=get_job_details&id="+taskId,
            reader: {
                type: "json",
                root: "alignments"
            }
        });
        store.load();
        var store2 = Ext.data.StoreManager.lookup("parcaTaskSummary");
        store2.setProxy({
            type: "ajax",
            url: "cgi/parca_requests.py?method=get_job_details&id="+taskId,
            reader: {
                type: "json"
            }
        });
        store2.load();
        var store3 = Ext.data.StoreManager.lookup("parcaTaskChart");
        store3.setProxy({
            type: "ajax",
            url: "cgi/parca_requests.py?method=get_job_details&id="+taskId,
            reader: {
                type: "json",
                root: "points"
            }
        });
        store3.load();
        var preview = Ext.getCmp("parcaAlignmentPreview");
        preview.taskId = taskId;
    },
    items: [
        Ext.create('Ext.view.View', {
            minHeight: 54,
            store: Ext.create("Ext.data.JsonStore", {
                storeId: 'parcaTaskSummary',
                autoLoad: false,
                fields: [
                         {name:"name1", type:"string"},
                         {name:"name2", type:"string"},
                         {name:"comment", type:"string"},
                         {name:"matrix", type:"string"},
                         {name:"gep", type:"float"},
                         {name:"limit", type:"integer"}
                         ]
            }),
            tpl: new Ext.XTemplate(
                '<tpl for=".">',
                '<div class="summary-wrap">',
                '<div class="summary-title">{comment}</div>',
                '<div class="summary-info">',
                '<div class="summary-row"><span class="summary-label">Aligned:</span> <span class="summary-value">{name1}</span> <span class="summary-label">v.s.</span> <span class="summary-value">{name2}</span></div>',
                '<br/>',
                '<div class="summary-row"><span class="summary-label">Using substitution matrix</span> <span class="summary-value">{matrix}</span>, <span class="summary-label">GEP=</span><span class="summary-value">{gep}</span> <span class="summary-label">and limit of </span><span class="summary-value">{limit}</span> <span class="summary-label">gaps</span></div>',
                '</div></div>',
                '</tpl>'
            ),
            itemSelector: 'div.summary-wrap',
            region: "north"
        }),
        Ext.create('Ext.grid.Panel', {
            title: "Alignments Summary",
            id: "parcaAlignmentsTable",
            region: "center",
            selModel: Ext.create("Ext.selection.RowModel", {
                
            }),
            columns: [
                { header: "Rating", dataIndex: "rating", width: 50 },
                { header: "Bend<br>score", dataIndex: "score", width: 70 },
                { header: "Substitution<br>score", dataIndex: "m", width: 70},
                { header: "Gaps", dataIndex: "g", width: 50 },
                { header: "MinGOP", dataIndex: "mingop", width: 70 },
                { header: "MaxGop", dataIndex: "maxgop", width: 70 }
                
            ],
            store: Ext.create("Ext.data.JsonStore", {
                storeId: 'parcaTaskDetails',
                autoLoad: false,
                fields: [ {name: "rating", type: "string"},
                          {name: "score", type: "float" },
                          {name: "m", type: "integer"},
                          {name: "g", type: "integer"},
                          {name: "mingop", type: "string"},
                          {name: "maxgop", type: "string"},
                          {name: "no", type: "integer"}
                        ]
            }),
            listeners: {
                itemclick: function(view, record, item, index, e) {
                    var no = record.data.no;
                    var g = record.data.g;
                    var preview = Ext.getCmp("parcaAlignmentPreview");
                    preview.alignmentNo = no;
                    preview.loadAlignment();
                    var ser = Ext.getCmp("parcaGraph");
                    for (var i=0; i<ser.series.items[0].markerGroup.items.length; i++) {
                        var marker = ser.series.items[0].markerGroup.items[i];
                        var value = marker.value;
                        var mg = value.replace(" ","").replace("'","").replace('"',"").split(",");
                        var gg = mg[0];
                        var radius = gg==g? 9 : 5;
                        marker.setAttributes({ radius: radius });
                        marker.redraw();
                    }
                }
            }
        }),
        Ext.create('Ext.panel.Panel', {
            region: "south",
            title: "Chart",
            layout: 'fit',
            height: 280,
            collapsible: true,
            items: [Ext.create('Ext.chart.Chart', {
                id: "parcaGraph",
                style: 'background:#fff',
                axes: [
                    {
                        type: 'Numeric',
                        position: 'left',
                        title: 'Substitution score',
                        fields: ['m'],
                        minimum: 0/*,
                        maximum: 100*/
                    },
                    {
                        type: 'Numeric',
                        position: 'bottom',
                        title: 'Gaps',
                        fields: ['g'],
                        minimum: 0/*,
                        maximum: 10*/
                    }
                ],
                series: [ {
                    id: "parcaGraphSeries",
                    type: 'line',
                    highlight: {
                        fill: 'lightgray'
                        //radius: 7
                    },
                    axis: 'left',
                    xField: 'g',
                    yField: 'm',
                    markerConfig: {
                        type: 'circle',
                        size: 4,
                        radius: 5,
                        //'stroke-width': 0,
                        listeners: {
                            click: function(e, t, opts) {
                                var mg = e.value.replace(" ","").replace("'","").replace('"',"").split(",");
                                var g = mg[0];
                                // Find item with corresponding g-value
                                var store = Ext.data.StoreManager.lookup("parcaTaskDetails");
                                var item = null;
                                for (var i=0; i<store.data.length; i++) {
                                    var it = store.data.items[i];
                                    if (it.data.g==g) {
                                        item = it;
                                        break;
                                    }
                                }
                                if (item) {
                                    var ser = Ext.getCmp("parcaGraph");
                                    for (var i=0; i<ser.series.items[0].markerGroup.items.length; i++) {
                                        var marker = ser.series.items[0].markerGroup.items[i];
                                        marker.setAttributes({ radius: 5 });
                                        marker.redraw();
                                    }
                                    this.setAttributes({ radius: 9 });
                                    this.redraw();
                                    var no = item.data.no;
                                    var preview = Ext.getCmp("parcaAlignmentPreview");
                                    preview.alignmentNo = no;
                                    preview.loadAlignment();
                                    var table = Ext.getCmp("parcaAlignmentsTable");
                                    var selModel = table.getSelectionModel();
                                    selModel.select([item]);
                                }
                            }
                        }
                    }
                }],
                store: Ext.create("Ext.data.JsonStore", {
                    storeId: 'parcaTaskChart',
                    autoLoad: false,
                    fields: [ {name: "rating", type: "integer"},
                              {name: "m", type: "integer"},
                              {name: "g", type: "integer"},
                              {name: "no", type: "integer"}
                            ]
                })
            })]
        }),
        Ext.create('Ext.panel.Panel', {
            id: "parcaAlignmentPreview",
            autoScroll: true,
            region: "east",
            title: "Alignment Preview",
            layout: 'vbox',
            collapsible: true,
            width: 500,
            taskId: -1,
            alignmentNo: -1,
            alignmentFormat: "EMBOSS",
            loadAlignment: function() {
                if (this.taskId != -1 && this.alignmentNo != -1)
                Ext.Ajax.request({
                    url: "cgi/parca_requests.py",
                    method: "GET",
                    params: {
                        "method": "get_alignment_as_string",
                        "id": this.taskId,
                        "no": this.alignmentNo,
                        "format": this.alignmentFormat
                    },
                    callback: function(options, success, response) {
                        if (success) {
                            var textArea = Ext.get("parcaPreviewText");
                            textArea.dom.innerHTML = "<pre>"+response.responseText+"</pre>";
                            textArea = Ext.getCmp("parcaPreviewText");
                            textArea.updateLayout();
                            var btnSave = Ext.getCmp("parcaSaveAlignmentButton");
                            btnSave.setDisabled(false);
                        }
                    }
                });
            },
            tbar: [
                Ext.create('Ext.toolbar.Toolbar', {
                    id: "parcaPreviewToolbar",
                    border: 0,
                    items: [
                        Ext.create("Ext.form.Label", { text: "Output format:" }),
                        Ext.create("Ext.form.ComboBox", {
                            id: "parcaPreviewFormat",
                            store: Ext.create('Ext.data.Store', {
                                fields: ['name'],
                                data: [
                                    {"name": "CLUSTAL"},
                                    {"name": "FASTA"},
                                    {"name": "FASTQ"},
                                    {"name": "EMBOSS"}
                                ]
                            }),
                            listeners: {
                                select: function(combo, records, eopts) {
                                    var preview = Ext.getCmp("parcaAlignmentPreview");
                                    preview.alignmentFormat = combo.value;
                                    preview.loadAlignment();
                                }
                            },
                            queryMode: 'local',
                            displayField: 'name',
                            valueField: 'name',
                            forceSelection: true,
                            value: "EMBOSS",
                            editable: false
                        }),
                        Ext.create("Ext.toolbar.Separator"),
                        Ext.create("Ext.button.Button", {
                            id: "parcaSaveAlignmentButton",
                            text: "Save to file...",
                            disabled: true,
                            handler: function() {
                                var prev = Ext.getCmp("parcaAlignmentPreview");
                                var url = "cgi/parca_requests.py?method=get_alignment_as_file&id="+prev.taskId+"&no="+prev.alignmentNo+"&format="+prev.alignmentFormat;
                                window.open(url);
                            }
                        })
                    ]
                })
            ],
            items: [
                Ext.create('Ext.container.Container', {
                    id: "parcaPreviewText",
                    html: 'Select an alignment from list or pick a point at chart to show alignment preview.'
                })
            ]
        })
    ]
});

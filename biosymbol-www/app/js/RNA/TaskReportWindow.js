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

Ext.define('RNA.TaskReportWindow', {
    id: "rnaTaskReportWindow",
    extend: 'Ext.window.Window',
    layout: 'border',
    maximized: true,
    draggable: false,
    resizable: false,
    modal: true,
    closable: false,
    taskId: -1,
    titleAlign: "center",
    tools: [
            {
                type: 'prev',
                xtype: 'button',
                icon: "extjs/resources/themes/images/default/shared/left-btn.gif",
                text: 'Go back to task list',
                handler: function(event, toolEl, panel) {
                    Ext.History.back();
                }
            },
            {
                xtype: 'tbspacer'
            },
            {
                xtype: 'button',
                text: "Save to file...",
                handler: function() {
                    var cmp = Ext.getCmp("rnaTaskReportWindow");
                    var url = "cgi/rna_requests.py?method=get_report_as_file&id="+cmp.taskId;
                    window.open(url);
                }
            }
    ],
    loadTaskData: function(taskId) {
        this.taskId = taskId;
        var store = Ext.data.StoreManager.lookup("rnaReport");
        store.setProxy({
            type: "ajax",
            url: "cgi/rna_requests.py?method=get_report&id="+taskId,
            reader: {
                type: "json"
            }
        });
        store.load();
    },
    items: [
        Ext.create('Ext.grid.Panel', {
            id: "rnaReportPreview",
            autoScroll: true,
            region: "center",
            store: Ext.create("Ext.data.JsonStore", {
                id: "rnaReport",
                autoLoad: false,
                fields: [
                    {name:"start", type: "integer"},
                    {name:"end", type: "integer"},
                    {name:"score", type: "integer"}
                ]
            }),
            columns: [
                {header: "Start", dataIndex: "start"},
                {header: "End", dataIndex: "end"},
                {header: "Score", dataIndex: "score"}
            ]
        })
    ]
});

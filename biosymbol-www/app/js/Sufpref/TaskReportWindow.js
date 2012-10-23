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

Ext.define('SufPref.TaskReportWindow', {
    id: "sufprefTaskReportWindow",
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
        var prev = Ext.getCmp("sufPrefReportPreview");
        prev.taskId = taskId;
        prev.loadReport();
    },
    items: [
        Ext.create('Ext.panel.Panel', {
            id: "sufPrefReportPreview",
            autoScroll: true,
            region: "center",
            layout: 'vbox',
            taskId: -1,
            loadReport: function() {
                if (this.taskId != -1) {
                    Ext.Ajax.request({
                        url: "cgi/sufpref_requests.py",
                        method: "GET",
                        params: {
                            "method": "get_report_as_string",
                            "id": this.taskId
                        },
                        callback: function(options, success, response) {
                            if (success) {
                                var textArea = Ext.get("sufprefReportText");
                                textArea.dom.innerHTML = "<pre>"+response.responseText+"</pre>";
                                textArea = Ext.getCmp("sufprefReportText");
                                textArea.updateLayout();
                                var btnSave = Ext.getCmp("sufprefSaveReportButton");
                                btnSave.setDisabled(false);
                            }
                        }
                    });
                }
            },
            tbar: [
                Ext.create('Ext.toolbar.Toolbar', {
                    id: "sufPrefReportPreviewToolbar",
                    border: 0,
                    items: [
                        Ext.create("Ext.button.Button", {
                            id: "sufprefSaveReportButton",
                            text: "Save to file...",
                            disabled: true,
                            handler: function() {
                                var prev = Ext.getCmp("sufPrefReportPreview");
                                var url = "cgi/sufpref_requests.py?method=get_report_as_file&id="+prev.taskId;
                                window.open(url);
                            }
                        })
                    ]
                })
            ],
            items: [
                Ext.create('Ext.container.Container', {
                    id: "sufprefReportText",
                    html: 'Loading...'
                })
            ]
        })
    ]
});

Ext.require('Ext.container.*');
Ext.require('Ext.button.*');
Ext.require('Ext.grid.*');
Ext.require('Ext.panel.*');
Ext.require('Ext.toolbar.*');
Ext.require('Ext.data.*');
Ext.require('Ext.util.*');
Ext.require('Ext.TaskManager');
Ext.require('Parca.NewTaskWindow')
Ext.require('Ext.Ajax.*');
Ext.require('Ext.Msg.*');
Ext.require('Parca.TaskDetailWindow');
Ext.require('Parca.SequencesChooser');
Ext.require('Sufpref.NewTaskWidget');
Ext.require('RNA.NewRNATaskWindow');



Ext.define('Symbol.MainWidget', {
    id: "symbolMainWidget",
    extend: 'Ext.container.Viewport',
    title: 'Symbol',
    layout: { type: 'border' },
    checkWorker: {
        run: function() {
            var working = [];
            var store = Ext.data.StoreManager.lookup("tasksSummary");          
            store.load();
            //var data = store.data.items;
            //for (var i=0; i<data.length; i++) {
            //    var item = data[i].data;
            //    if (item.status!="WORKING") {
            //        working.push(item);
            //    }
            //}
            //Ext.Ajax.request({
            //    url: 'cgi/symbol_requests.py',
            //    method: 'POST',
            //    params: { "method": "update_statuses", "tools_ids" : Ext.encode(working) },
            //    callback: function(options, success, response) {
            //        if (success) {
            //            var resp = Ext.decode(response.responseText);
            //            for (var i=0; i<resp.length; i++) {
            //                var id = resp[i].id;
            //                var tool = resp[i].tool;
            //                var status = resp[i].status;
            //                var error = resp[i].error;
            //            }
            //        }
            //    }
            //});
        },
        interval: 5000
    },
    items: [
        Ext.create('Ext.grid.Panel', {
            title: 'Existing Tasks',
            id: 'symbolTasksGrid',
            region: 'center',
            viewConfig: {
                preserveScrollOnRefresh: true
            },
            savedScroll: { left: 0, top: 0 },
            columns: [
                { header: "Tool", dataIndex: "tool", width: 50 },
                { header: "ID", dataIndex: "job_id", width: 50 },
                { header: "Comment", dataIndex: "comment" , width: 300 },
                { header: "Accepted", dataIndex: "start", width: 120 },
                { header: "Status", dataIndex: "status" }
            ],
            store: Ext.create('Ext.data.JsonStore', {
                storeId: 'tasksSummary',
                autoLoad: true,
                fields: ["tool", "job_id", "comment", "start", "status", "error"],
                proxy: {
                    type: "ajax",                   
                    url: "cgi/symbol_requests.py?method=list_all_jobs",
                    reader: {
                        type: "json"
                    }
                }/*,
                listeners: {
                    beforeload: function(store, oper, opts) {
                        var view = Ext.getCmp("symbolTasksGrid");
                        var table = view.getView();
                        var tableEL = table.getEl();
                        if (tableEL) {
                            view.savedScroll = tableEL.getScroll();
                            Ext.log("SAVE: left: "+view.savedScroll.left+", top: "+view.savedScroll.top);
                        }
                        
                    },
                    datachanged: function(store, opts) {
                        var view = Ext.getCmp("symbolTasksGrid");
                        var table = view.getView();
                        var tableEL = table.getEl();
                        Ext.log("RESTORE: left: "+view.savedScroll.left+", top: "+view.savedScroll.top);
                        //tableEL.scroll("left", view.savedScroll.left);
                        //tableEL.scrollTo(view.savedScroll.left, view.savedScroll.top);

                    }
                }*/
            }),
            listeners: {
                itemclick: function(view, record, item, index, e) {
                    if (record.data.status=="ERROR") {
                        Ext.Msg.alert("Task has an error", record.data.error);
                    }
                    else if (record.data.status=="DONE") {
                        var windowClassName = null;
                        var taskId = record.data.job_id;
                        if (record.data.tool=="PARCA") {
                            windowClassName = "Parca.TaskDetailWindow";
                        }
                        else if (record.data.tool=="SufPref") {
                            windowClassName = "SufPref.TaskReportWindow";

                        }
                        else if (record.data.tool=="RNA") {
                            windowClassName = "RNA.TaskReportWindow";
			}
                        Ext.History.add(windowClassName+":"+taskId+":"+record.data.tool);
                    }
                }
            }
        }),
        Ext.create('Ext.panel.Panel', {
            layout: { type: 'accordion' },
            width: 300,
            title: "New Task",
            collapsible: true,
            region: 'east',
            items: [
                Ext.create('Parca.NewTaskWindow'),
                Ext.create('Sufpref.NewTaskWidget'),
                Ext.create('RNA.NewRNATaskWindow')
            ]
        }),
        
    ],
    showTaskDetailWindow: function(windowClassName, taskId, toolName) {
        // Generate window title
        var title = "Task "+taskId+" details (used tool: "+toolName+")";
        // Try to find existing item
        var id = null;
        if (windowClassName=="Parca.TaskDetailWindow") {
            id = Ext.getCmp("parcaTaskDetailWindow");
        }
        if (windowClassName=="SufPref.TaskReportWindow") {
            id = Ext.getCmp("sufprefTaskReportWindow");
        }
        if (windowClassName=="RNA.TaskReportWindow") {
            id = Ext.getCmp("rnaTaskReportWindow");
        }
        // If window created -- set title, load task and show
        if (id) {
            id.setTitle(title);
        }
        else {
            id = Ext.create(windowClassName, {
                title: title,
                jobId: taskId
            });
        }
        id.loadTaskData(taskId);
        id.show();
    },
    showDialogWindow: function(windowClassName, pageNo) {
        var id = null;
        if (windowClassName=="Parca.SequencesChooser") {
            id = Ext.getCmp("parcaSequencesChooser");
        }
        if (!id) {
            id = Ext.create(windowClassName);
        }
        id.setActiveIndex(pageNo);
        id.show();
    },
    hideAllModalWindows: function() {
        var IDS = [
            "parcaTaskDetailWindow",
            "parcaSequencesChooser",
            "sufprefTaskReportWindow",
            "rnaTaskReportWindow"
        ];
        for (var i=0; i<IDS.length; i++) {
            var id = IDS[i];
            var wnd = Ext.getCmp(id);
            if (wnd) {
                wnd.hide();
            }
        }
    }
});

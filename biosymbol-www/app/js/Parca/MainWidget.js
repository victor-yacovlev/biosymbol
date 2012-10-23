Ext.require('Ext.container.*');
Ext.require('Ext.button.*');
Ext.require('Ext.grid.*');
Ext.require('Ext.panel.*');
Ext.require('Ext.toolbar.*');
Ext.require('Ext.data.*');
Ext.require('Parca.NewTaskWindow')


Ext.define('Parca.MainWidget', {    
    extend: 'Ext.container.Viewport',
    title: 'PARCA',
    layout: { type: 'border' },
    items: [
        Ext.create('Parca.NewTaskWindow', {
            collapsible: true,
            id: 'newTaskForm',
            region: 'east'
        }),
        Ext.create('Ext.grid.Panel', {
            title: 'Existing Tasks',
            region: 'center',
            columns: [
                { header: "ID", dataIndex: "id" },
                { header: "Comment", dataIndex: "comment" , width: 300 },
                { header: "Accepted", dataIndex: "start" },
                { header: "Status", dataIndex: "status" },
            ],
            proxy: {
                type: "ajax",
                url: "cgi/parca_requests.py?method=list_all_jobs"
            }
        })
    ]
});

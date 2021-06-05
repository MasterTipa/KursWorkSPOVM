const electron = require('electron');
const dialog = electron.dialog;
const url = require('url');
const path = require('path');
const mail_client = require('./connection/build/Release/client');

const {app, BrowserWindow, Menu, ipcMain} = electron;

var mainWindow;
var showMessageWindow;
var sendMessageWindow;
var loginWindow;

process.env.NODE_ENV == 'production'

//Listen  for app to be ready
app.on('ready', ()=>{
    //Create new window
    mainWindow = new BrowserWindow({
        title: "LoliMailer",
        webPreferences:{
            nodeIntegration:true
        }
    });

    loginWindow = new BrowserWindow({
        title: "Log In",
        webPreferences:{
            nodeIntegration:true
        },
        parent: mainWindow
    });

    loginWindow.loadURL(url.format({
        pathname: path.join(__dirname, 'loginwindow.html'),
        protocol: 'file:',
        slashes: true
    }));

    mainWindow.maximize();
    mainWindow.loadURL(url.format({
        pathname: path.join(__dirname, 'mainWindow.html'),
        protocol: 'file:',
        slashes: true
    }));

    mainWindow.on('closed', ()=>{
        app.quit();
    });

    loginWindow.on('closed', ()=>{
        loginWindow = null;
    });

});

async function createShowMessageWindow(){
    //Create new window
    showMessageWindow = new BrowserWindow({
        width: 400,
        height: 200,
        title: 'Mail',
        webPreferences:{
            nodeIntegration:true
        }
    });

    //showMessageWindow.removeMenu();
    //Load html file into window
    showMessageWindow.loadURL(url.format({
        pathname: path.join(__dirname, 'showmessage.html'),
        protocol: 'file:',
        slashes: true
    }));
    showMessageWindow.maximize();
    //Garbage collection
    showMessageWindow.on('closed', ()=>{
        showMessageWindow = null;
    });
};

async function createSendMessageWindow(){
    //Create new window
    sendMessageWindow = new BrowserWindow({
        width: 400,
        height: 200,
        title: 'New Mail',
        webPreferences:{
            nodeIntegration:true
        }
    });

    //showMessageWindow.removeMenu();
    //Load html file into window
    sendMessageWindow.loadURL(url.format({
        pathname: path.join(__dirname, 'sendmessage.html'),
        protocol: 'file:',
        slashes: true
    }));
    sendMessageWindow.maximize();
    //Garbage collection
    sendMessageWindow.on('closed', ()=>{
        sendMessageWindow = null;
    });
};

ipcMain.on("get_messages", (e, data)=>{
    let arr = mail_client.getMessages();
    //console.log(arr);
    mainWindow.webContents.send("get_messages", arr);
});

ipcMain.on("get_message_by_id", async (e, data)=>{
    if(showMessageWindow == null) {createShowMessageWindow();}
    else {return;}
    let msg = mail_client.getMessageById(data);
    showMessageWindow.webContents.on("did-finish-load", ()=>{
        showMessageWindow.webContents.send("get_message_by_id", msg);
    });
});

/*
{
    sender: "sender@example.com",
    recipient: "recipient@example.com",
    from: "Someone aka sender@example.com",
    to: "Recipient aka recipient@example.com",
    subject: "Subject",
    content_type: "text/plain",
    data: "Data"
}
*/

ipcMain.on("send_email", (e, data)=>{
    data.sender = mail_client.getUserEmail();
    data.content_type = "text/plain";
    data.from = data.sender;
    let result = mail_client.sendMessage(data);
    sendMessageWindow.close();
});

ipcMain.on("new_email", (e, data)=>{
    if(sendMessageWindow == null) {createSendMessageWindow();}
});

ipcMain.on("delete_email", (e, data)=>{
    let result = mail_client.deleteMessage(data);
    console.log(result);
});

ipcMain.on("authorize", (e, data)=>{
    let result = mail_client.authorize(data);
    console.log(result);
    loginWindow.close();
});

const menuWindowTemplate = [
    {
        label: 'File',
        submenu : [
            {
                label : 'Send mail',
                click(){
                    
                }
            },
            {
                label : 'Quit',
                accelerator : (process.platform == 'darwin') ? 'Command+Q' : 'Ctrl+Q',
                click(){
                    app.quit();
                }
            }
        ]
    }
];

if(process.platform == 'darwin'){
    menuWindowTemplate.unshift({});
}
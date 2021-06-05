const { ipcRenderer } = require('electron');

const body = document.querySelector("body");

ipcRenderer.on("get_message_by_id", (e, data)=>{
    body.innerHTML = data.data;
});
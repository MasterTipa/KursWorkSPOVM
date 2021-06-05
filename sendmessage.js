const { ipcRenderer } = require('electron');

const submit_button = document.getElementById("submit_button");
const to_filed = document.getElementById("to_field");
const subject_filed = document.getElementById("subject_field");
const text_filed = document.getElementById("text_field");

submit_button.addEventListener('click', (e)=>{
    e.preventDefault();
    let object = new Object;
    object.to = to_filed.value;
    object.recipient = to_filed.value;
    object.subject = subject_filed.value;
    object.data = text_filed.value;
    ipcRenderer.send("send_email", object);
});
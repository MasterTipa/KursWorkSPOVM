const { ipcRenderer } = require('electron');

const submit_button = document.getElementById("submit_button");
const email_filed = document.getElementById("email_field");
const password_filed = document.getElementById("password_field");

submit_button.addEventListener('click', (e)=>{
    e.preventDefault();
    let object = new Object;
    object.name = email_filed.value;
    object.password = password_filed.value;
    ipcRenderer.send("authorize", object);
});
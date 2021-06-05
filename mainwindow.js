const { ipcRenderer } = require('electron');

const threshold_subject = 44;
const threshold_name = 10;
const main = document.getElementsByClassName("main")[0];
const items = document.getElementsByClassName("item");

document.getElementById("reload_btn").addEventListener('click', (e) => {
    e.preventDefault();
    ipcRenderer.send("get_messages");
});

document.getElementById("add_btn").addEventListener('click', (e)=>{
    e.preventDefault();
    ipcRenderer.send("new_email");
});

document.getElementById("delete_btn").addEventListener('click', (e)=>{
    Array.from(items).forEach(item =>{
        if(item.childNodes[0].childNodes[0].checked == true) {
            main.removeChild(item);
            ipcRenderer.send("delete_email", item.letter_id);
        }
    });
});

ipcRenderer.on("get_messages", (event, data) => {
    main.innerHTML = "";
    data.forEach(email => {
        createItem(email)
    })
});


const createItem = (object) => {
    let item = document.createElement('div');
    item.className = "item";
    let checkbox_container = document.createElement("div");
    checkbox_container.className = "checkbox-container";
    let checkbox = document.createElement("input");
    checkbox.type = "checkbox";
    checkbox.className = "checkbox";
    let title = document.createElement("div");
    title.className = "title";
    title.textContent = (object.subject.length >= threshold_subject) ? object.subject.substring(0, threshold_subject - 3) + "..." : object.subject;
    let author = document.createElement("div");
    author.className = "author";
    author.textContent = object.sender;
    let date = document.createElement("div");
    date.className = "time";
    date.textContent = "12:00 PM";
    main.appendChild(item);
    checkbox_container.appendChild(checkbox);
    item.appendChild(checkbox_container);
    item.appendChild(title);
    item.appendChild(author);
    item.appendChild(date);
    item.letter_id = object.id;

    title.addEventListener("click", (e)=>{
        e.preventDefault();
        ipcRenderer.send("get_message_by_id", item.letter_id);
    });
}

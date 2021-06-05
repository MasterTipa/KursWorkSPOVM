{
    "targets":[{
        "target_name" : "client",
        "sources" :["node_wrap.cpp", "client_pop3.cpp", "client_smtp.cpp", "message.cpp", "ssl_socket.cpp", "tcp_socket.cpp", "base64.cpp"],
        "cflags!": [ '-fno-exceptions' ],
        "cflags_cc!": [ '-fno-exceptions' ],
        "cflags_cc": ["-std=c++17"],
        "link_settings" : {
            "libraries" : ["-lssl -lcrypto -lpthread"]
        },
        "include_dirs" : ["./"],
    }]
}
#include <node.h>
#include <v8.h>
#include <algorithm>

#include "client_pop3.hpp"
#include "client_smtp.hpp"
#include "message.h"
#include "base64.h"

static ClientPOP3 pop3_client;
static ClientSMTP smtp_client;

void authorize(const v8::FunctionCallbackInfo<v8::Value>& args){
    v8::Isolate *isolate = args.GetIsolate();

    if(!args[0]->IsObject()){
        args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
        return;
    }
    v8::Local<v8::Object> object = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    
    std::string name_from_obj = *v8::String::Utf8Value(isolate, object->Get(isolate->GetCurrentContext(),
        v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(isolate, std::string("name").c_str())).ToLocalChecked()).ToLocalChecked());
    std::string password_from_obj = *v8::String::Utf8Value(isolate, object->Get(isolate->GetCurrentContext(),
        v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(isolate, std::string("password").c_str())).ToLocalChecked()).ToLocalChecked());

    pop3_client.setUserInfo(name_from_obj, password_from_obj);
    smtp_client.setUserInfo(base64_encode(name_from_obj), base64_encode(password_from_obj));

    bool result = pop3_client.Autorithe() && smtp_client.Autorithe();
    pop3_client.closeSession();
    smtp_client.closeSession();

    args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
}

void sendMessage(const v8::FunctionCallbackInfo<v8::Value>& args){
    v8::Isolate *isolate = args.GetIsolate();

    if(!args[0]->IsObject()){
        args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
        return;
    }
    v8::Local<v8::Object> object = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

    std::string sender_from_obj = *v8::String::Utf8Value(isolate, object->Get(isolate->GetCurrentContext(),
        v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(isolate, std::string("sender").c_str())).ToLocalChecked()).ToLocalChecked());
    std::string recipient_from_obj = *v8::String::Utf8Value(isolate, object->Get(isolate->GetCurrentContext(),
        v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(isolate, std::string("recipient").c_str())).ToLocalChecked()).ToLocalChecked());
    std::string from_from_obj = *v8::String::Utf8Value(isolate, object->Get(isolate->GetCurrentContext(),
        v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(isolate, std::string("from").c_str())).ToLocalChecked()).ToLocalChecked());
    std::string to_from_obj = *v8::String::Utf8Value(isolate, object->Get(isolate->GetCurrentContext(),
        v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(isolate, std::string("to").c_str())).ToLocalChecked()).ToLocalChecked());
    std::string content_type_from_obj = *v8::String::Utf8Value(isolate, object->Get(isolate->GetCurrentContext(),
        v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(isolate, std::string("content_type").c_str())).ToLocalChecked()).ToLocalChecked());
    std::string data_from_obj = *v8::String::Utf8Value(isolate, object->Get(isolate->GetCurrentContext(),
        v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(isolate, std::string("data").c_str())).ToLocalChecked()).ToLocalChecked());
    std::string subject_from_obj = *v8::String::Utf8Value(isolate, object->Get(isolate->GetCurrentContext(),
        v8::MaybeLocal<v8::String>(v8::String::NewFromUtf8(isolate, std::string("subject").c_str())).ToLocalChecked()).ToLocalChecked());

    bool result = smtp_client.Autorithe();
    if(result){
        result = smtp_client.sendMessage(Message(sender_from_obj, recipient_from_obj, subject_from_obj,
            content_type_from_obj, data_from_obj));
    }

    pop3_client.closeSession();
    smtp_client.closeSession();

    args.GetReturnValue().Set(v8::Boolean::New(isolate, result));

}


void getMessages(const v8::FunctionCallbackInfo<v8::Value>& args){
    v8::Isolate *isolate = args.GetIsolate();
    if(!pop3_client.Autorithe()){
        args.GetReturnValue().Set(v8::Array::New(isolate));
        return;
    }
    v8::Local<v8::Array> arr = v8::Array::New(isolate);
    std::map<size_t, std::string> messages_map = pop3_client.getMessages();
    size_t counter = 0;
    std::for_each(messages_map.begin(), messages_map.end(), [&](auto & message){
        v8::Local<v8::Object> obj = v8::Object::New(isolate);
        Message msg;
        msg.Parse(message.second);
        obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("id").c_str()).ToLocalChecked(),
            v8::Integer::New(isolate, message.first));
        obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("sender").c_str()).ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, msg.getSenderContext().c_str()).ToLocalChecked());
        obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("recipient").c_str()).ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, msg.getRecipientContext().c_str()).ToLocalChecked());
        obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("from").c_str()).ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, msg.getFromFieldContext().c_str()).ToLocalChecked());
        obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("to").c_str()).ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, msg.getRecipientContext().c_str()).ToLocalChecked());
        obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("subject").c_str()).ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, msg.getSubjectContext().c_str()).ToLocalChecked());
        obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("content_type").c_str()).ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, msg.getContentTypeContext().c_str()).ToLocalChecked());
        obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("data").c_str()).ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, msg.getDataContext().c_str()).ToLocalChecked());
        arr->Set(isolate->GetCurrentContext(), counter, obj);
        ++counter;
    });
    pop3_client.closeSession();
    args.GetReturnValue().Set(arr);
}

void getMessageById(const v8::FunctionCallbackInfo<v8::Value>& args){
    v8::Isolate *isolate = args.GetIsolate();
    if(!pop3_client.Autorithe()){
        args.GetReturnValue().Set(v8::Object::New(isolate));
        return;
    }
    size_t id = args[0]->IntegerValue(isolate->GetCurrentContext()).FromJust();
    std::pair<size_t, std::string> message = pop3_client.getMessageById(id);
    v8::Local<v8::Object> obj = v8::Object::New(isolate);
    Message msg;
    msg.Parse(message.second);
    obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("id").c_str()).ToLocalChecked(),
        v8::Integer::New(isolate, message.first));
    obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("sender").c_str()).ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, msg.getSenderContext().c_str()).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("recipient").c_str()).ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, msg.getRecipientContext().c_str()).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("from").c_str()).ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, msg.getFromFieldContext().c_str()).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("to").c_str()).ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, msg.getRecipientContext().c_str()).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("subject").c_str()).ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, msg.getSubjectContext().c_str()).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("content_type").c_str()).ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, msg.getContentTypeContext().c_str()).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, std::string("data").c_str()).ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, msg.getDataContext().c_str()).ToLocalChecked());

    args.GetReturnValue().Set(obj);
}

void deleteMessage(const v8::FunctionCallbackInfo<v8::Value>& args){
    v8::Isolate *isolate = args.GetIsolate();
    if(!pop3_client.Autorithe()){
        args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
        return;
    }
    bool result = pop3_client.deleteMessage(args[0]->IntegerValue(isolate->GetCurrentContext()).FromJust());
    pop3_client.closeSession();
    args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
}

void getUserEmail(const v8::FunctionCallbackInfo<v8::Value>& args){
    args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), pop3_client.getUserEmail().c_str()).ToLocalChecked());
}


void Init(v8::Local<v8::Object> exports){
    NODE_SET_METHOD(exports, "authorize", authorize);
    NODE_SET_METHOD(exports, "sendMessage", sendMessage);
    NODE_SET_METHOD(exports, "getMessages", getMessages);
    NODE_SET_METHOD(exports, "getMessageById", getMessageById);
    NODE_SET_METHOD(exports, "deleteMessage", deleteMessage);
    NODE_SET_METHOD(exports, "getUserEmail", getUserEmail);
}

NODE_MODULE(addon, Init);
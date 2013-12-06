#ifndef CONNECTION_H
#define CONNECTION_H

#include <node.h>
#include <v8.h>
#include <node_buffer.h>
#include <libpq-fe.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define LOG(msg) printf("%s\n",msg);
#define THROW(msg) return ThrowException(Exception::Error(String::New(msg)));

using namespace v8;
using namespace node;

class Connection : public node::ObjectWrap
{

public:
	
	static void Init(Handle<Object> target);
	bool Connect(const char* conninfo);
	void Disconnect();
	
private:
	
	PGconn *connection_;
	
	Connection();
	~Connection();
	
	static Handle<Value> New(const v8::Arguments& args);
	static Handle<Value> Connect(const Arguments& args);
	static Handle<Value> Disconnect(const Arguments& args);
	
	static Handle<Value> ExecCommand(const Arguments& args);
	static Handle<Value> ExecQuery(const Arguments& args);
	static Handle<Value> ExecQueryWithParams(const Arguments& args);
	static Handle<Value> ExecPrepare(const Arguments& args);
	static Handle<Value> ExecQueryPrepared(const Arguments& args);
	
	static Handle<Value> DispatchParameterizedQuery(const Arguments& args, bool isPrepared);
	
	static Handle<Array> HandleTuplesResult(const PGresult* result);
	
	static char** ArgToCStringArray(Local<Array> params);
	static void ReleaseCStringArray(char **strArray, int len);
	static char* MallocCString(Handle<Value> v8String);
	
};

#endif



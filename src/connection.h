#ifndef CONNECTION_H
#define CONNECTION_H

#include <nan.h>
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
	
protected:
	
	PGconn *connection_;
	
	Connection();
	~Connection();
	
	static NAN_METHOD(New);
	static NAN_METHOD(Connect);
	static NAN_METHOD(Disconnect);
	
	static NAN_METHOD(EscapeLiteral);
	static NAN_METHOD(EscapeIdentifier);
	static NAN_METHOD(EscapeStringConn);
	
	static NAN_METHOD(ExecCommand);
	static NAN_METHOD(ExecQuery);
	static NAN_METHOD(ExecQueryWithParams);
	static NAN_METHOD(ExecPrepare);
	static NAN_METHOD(ExecQueryPrepared);
	
	static Handle<Array> DispatchParameterizedQuery(_NAN_METHOD_ARGS, bool isPrepared);
	static Handle<Array> HandleTuplesResult(const PGresult* result);
	
	static char** ArgToCStringArray(Local<Array> params);
	static void ReleaseCStringArray(char **strArray, int len);
	static char* MallocCString(Handle<Value> v8String);
	
};

#endif



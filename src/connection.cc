
#include <libpq-fe.h>
#include <node.h>
#include <node_buffer.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "connection.h"

using namespace v8;
using namespace node;

Connection::Connection() {};
Connection::~Connection() {};

//v8 object initializer
void Connection::Init(Handle<Object> target)
{
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	
	tpl->SetClassName(String::NewSymbol("Connection"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("Connection"),
		FunctionTemplate::New(New)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("connect"),
		FunctionTemplate::New(Connect)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("execCommand"),
		FunctionTemplate::New(ExecCommand)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("execQuery"),
		FunctionTemplate::New(ExecQuery)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("execQueryWithParams"),
		FunctionTemplate::New(ExecQueryWithParams)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("execPrepare"),
		FunctionTemplate::New(ExecPrepare)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("execQueryPrepared"),
		FunctionTemplate::New(ExecQueryPrepared)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		String::NewSymbol("disconnect"),
		FunctionTemplate::New(Disconnect)->GetFunction()
	);
	
	Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
	target->Set(String::NewSymbol("Connection"), constructor);
}

//v8 entry point to constructor
Handle<Value> Connection::New(const Arguments& args)
{
	HandleScope scope;
	
	Connection *connection = new Connection();
	connection->Wrap(args.This());
	
	return args.This();
}

//v8 entry point into Connection#connect
Handle<Value> Connection::Connect(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if(args.Length() == 0 || !args[0]->IsString()) {
		THROW("Must include connection string as only argument to connect");
	}
	
	String::Utf8Value conninfo(args[0]->ToString());
	bool success = self->Connect(*conninfo);
	if(!success) {
		THROW("Can not make connection");
		self->Disconnect();
	}
	
	return Undefined();
}

//v8 entry point into Connection#disconnect
Handle<Value> Connection::Disconnect(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	self->Disconnect();
	
	return Undefined();
}


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 


//v8 entry point into Connection#execCommand
Handle<Value> Connection::ExecCommand(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		THROW("First parameter must be a string query");
	}
	
	char* queryText = MallocCString(args[0]);
	
	// call libpq
	PGresult *result = PQexec(self->connection_, queryText);
	
	// simply returns result status
	Local<Object> response = Object::New();
	char* status = PQresStatus(PQresultStatus(result));
	response->Set(NODE_PSYMBOL("status"), String::New(status));
	
	PQclear(result);
	free(queryText);
	
	return scope.Close(response);
}

//v8 entry point into Connection#execQuery
Handle<Value> Connection::ExecQuery(const Arguments& args)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		THROW("First parameter must be a string query");
	}
	
	char* queryText = MallocCString(args[0]);
	
	// call libpq
	PGresult *result = PQexec(self->connection_, queryText);
	
	// check result
	if( (PQresultStatus(result) != PGRES_COMMAND_OK) && (PQresultStatus(result) != PGRES_TUPLES_OK) ){
		LOG(PQresStatus(PQresultStatus(result)));
		THROW("Can not execute query");
	}
	
	// PGresult to JavaScript object
	Handle<Array> response = HandleTuplesResult(result);
	
	PQclear(result);
	free(queryText);
	
	return scope.Close(response);
}

//v8 entry point into Connection#execQueryWithParams
Handle<Value> Connection::ExecQueryWithParams(const Arguments& args)
{
	HandleScope scope;
	//dispatch non-prepared parameterized query
	return DispatchParameterizedQuery(args, false);
}

//v8 entry point into Connection#execPrepare(string queryName, string queryText, int nParams)
// 
//usage:
//  var res = con.execPrepare("s","select * from product where id=$1",1);
//  
Handle<Value> Connection::ExecPrepare(const Arguments& args)
{
	HandleScope scope;
	
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	char* queryName = MallocCString(args[0]);
	char* queryText = MallocCString(args[1]);
	
	int length = args[2]->Int32Value();
	
	// call libpq
	PGresult *result = PQprepare(self->connection_, queryName, queryText, length, NULL);
	
	// check result
	if( PQresultStatus(result) != PGRES_COMMAND_OK ){
		LOG(PQresStatus(PQresultStatus(result)));
		THROW("Can not execute prepare");
	}
	
	free(queryName);
	free(queryText);
	
	return Undefined();
}

//v8 entry point into Connection#execQueryPrepared(string queryName, string[] paramValues)
// 
//usage: parameter should be string
//  var res = con.execQueryPrepared("s",["1"]);
//  
Handle<Value> Connection::ExecQueryPrepared(const Arguments& args)
{
	HandleScope scope;
	//dispatch prepared parameterized query
	return DispatchParameterizedQuery(args, true);
}

Handle<Value> Connection::DispatchParameterizedQuery(const Arguments& args, bool isPrepared)
{
	HandleScope scope;
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	String::Utf8Value queryName(args[0]);
	
	if(!args[0]->IsString()) {
		THROW("First parameter must be a string");
	}
	
	if(!args[1]->IsArray()) {
		THROW("Values must be an array");
	}
	
	Local<Array> jsParams = Local<Array>::Cast(args[1]);
	int len = jsParams->Length();
	
	char** paramValues = ArgToCStringArray(jsParams);
	if(!paramValues) {
		THROW("Unable to allocate char **paramValues from Local<Array> of v8 params");
	}
	
	char* queryText = MallocCString(args[0]);
	
	// call libpq
	PGresult *result;
	if(isPrepared) {
		result = PQexecPrepared(self->connection_, queryText, len, paramValues, NULL, NULL, 0);
	} else {
		result = PQexecParams(self->connection_, queryText, len, NULL, paramValues, NULL, NULL, 0);
	}
	
	// check result
	if( (PQresultStatus(result) != PGRES_COMMAND_OK) && (PQresultStatus(result) != PGRES_TUPLES_OK) ){
		LOG(PQresStatus(PQresultStatus(result)));
		THROW("Can not execute parametarized query");
	}
	
	// PGresult to JavaScript object
	Handle<Array> response = HandleTuplesResult(result);
	
	PQclear(result);
	free(queryText);
	
	return scope.Close(response);
}


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 


//initializes initial sync connection to postgres via libpq
bool Connection::Connect(const char* conninfo)
{
	connection_ = PQconnectdb(conninfo);
	
	if ( PQstatus(connection_) != CONNECTION_OK ) {
		LOG("Unable to set connection");
		PQfinish(connection_);
		return false;
	}
	
	return true;
}

//safely destroys the connection at most 1 time
void Connection::Disconnect()
{
	if(connection_ != NULL) {
		PQfinish(connection_);
		connection_ = NULL;
	}
}


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 


//maps the postgres tuple results to v8 objects
Handle<Array> Connection::HandleTuplesResult(const PGresult* result)
{
	HandleScope scope;
	int rowCount = PQntuples(result);
	Local<Array> rows = Array::New();
	for(int rowNumber = 0; rowNumber < rowCount; rowNumber++) {
		//create result object for this row
		Local<Array> row = Array::New();
		int fieldCount = PQnfields(result);
		for(int fieldNumber = 0; fieldNumber < fieldCount; fieldNumber++) {
			Local<Object> field = Object::New();
			//name of field
			char* fieldName = PQfname(result, fieldNumber);
			field->Set(NODE_PSYMBOL("name"), String::New(fieldName));
			
			//oid of type of field
			int fieldType = PQftype(result, fieldNumber);
			field->Set(NODE_PSYMBOL("type"), Integer::New(fieldType));
			
			//value of field
			if(PQgetisnull(result, rowNumber, fieldNumber)) {
				field->Set(NODE_PSYMBOL("value"), Null());
			} else {
				char* fieldValue = PQgetvalue(result, rowNumber, fieldNumber);
				field->Set(NODE_PSYMBOL("value"), String::New(fieldValue));
			}
			row->Set(Integer::New(fieldNumber), field);
		}
		rows->Set(Integer::New(rowNumber),row);
	}
	return scope.Close(rows);
}

//Converts a v8 array to an array of cstrings
//the result char** array must be free() when it is no longer needed
//if for any reason the array cannot be created, returns 0
char** Connection::ArgToCStringArray(Local<Array> params)
{
	int len = params->Length();
	char** paramValues = new char*[len];
	for(int i = 0; i < len; i++) {
		Handle<Value> val = params->Get(i);
		if(val->IsString()) {
			char* cString = MallocCString(val);
			//will be 0 if could not malloc
			if(!cString) {
				LOG("ArgToCStringArray: OUT OF MEMORY OR SOMETHING BAD!");
				ReleaseCStringArray(paramValues, i-1);
				return 0;
			}
			paramValues[i] = cString;
		} else if(val->IsNull()) {
			paramValues[i] = NULL;
		} else {
			//a paramter was not a string
			LOG("Parameter not a string");
			ReleaseCStringArray(paramValues, i-1);
			return 0;
		}
	}
	return paramValues;
}

//helper function to release cString arrays
void Connection::ReleaseCStringArray(char **strArray, int len)
{
	for(int i = 0; i < len; i++) {
		free(strArray[i]);
	}
	delete [] strArray;
}

//helper function to malloc new string from v8string
char* Connection::MallocCString(v8::Handle<Value> v8String)
{
	String::Utf8Value utf8String(v8String->ToString());
	char *cString = (char *) malloc(strlen(*utf8String) + 1);
	if(!cString) {
		return cString;
	}
	strcpy(cString, *utf8String);
	return cString;
}





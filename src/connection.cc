#include <nan.h>
#include <libpq-fe.h>
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
	NanScope();
	
	Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
	
	tpl->SetClassName(NanNew<String>("Connection"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("Connection"),
		NanNew<FunctionTemplate>(New)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("connect"),
		NanNew<FunctionTemplate>(Connect)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("escapeLiteral"),
		NanNew<FunctionTemplate>(EscapeLiteral)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("escapeIdentifier"),
		NanNew<FunctionTemplate>(EscapeIdentifier)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("escapeStringConn"),
		NanNew<FunctionTemplate>(EscapeStringConn)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("execCommand"),
		NanNew<FunctionTemplate>(ExecCommand)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("execQuery"),
		NanNew<FunctionTemplate>(ExecQuery)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("execQueryWithParams"),
		NanNew<FunctionTemplate>(ExecQueryWithParams)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("execPrepare"),
		NanNew<FunctionTemplate>(ExecPrepare)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("execQueryPrepared"),
		NanNew<FunctionTemplate>(ExecQueryPrepared)->GetFunction()
	);
	
	tpl->PrototypeTemplate()->Set(
		NanNew<String>("disconnect"),
		NanNew<FunctionTemplate>(Disconnect)->GetFunction()
	);
	
	target->Set(NanNew<String>("Connection"), tpl->GetFunction());
}

//v8 entry point to constructor
NAN_METHOD(Connection::New)
{
	NanScope();
	
	Connection *connection = new Connection();
	connection->Wrap(args.This());
	
	NanReturnValue(args.This());
}

//v8 entry point into Connection#connect
NAN_METHOD(Connection::Connect)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if(args.Length() == 0 || !args[0]->IsString()) {
		NanThrowError("Must include connection string as only argument to connect");
	}
	
	String::Utf8Value conninfo(args[0]->ToString());
	bool success = self->Connect(*conninfo);
	if(!success) {
		NanThrowError("Can not make connection");
		self->Disconnect();
	}
	
	NanReturnUndefined();
}

//v8 entry point into Connection#disconnect
NAN_METHOD(Connection::Disconnect)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	self->Disconnect();
	
	NanReturnUndefined();
}

//v8 entry point into Connection#escapeLiteral
NAN_METHOD(Connection::EscapeLiteral)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanReturnUndefined();
	}
	
	char* unsafeStr = MallocCString(args[0]);
	
	// call libpq
	char *safeStr = PQescapeLiteral(self->connection_, unsafeStr, strlen(unsafeStr));
	
	// simply returns result status
	Handle<String> response =  NanNew<String>(safeStr);
	
	free(safeStr);
	free(unsafeStr);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#escapeIdentifier
NAN_METHOD(Connection::EscapeIdentifier)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanReturnUndefined();
	}
	
	char* unsafeStr = MallocCString(args[0]);
	
	// call libpq
	char *safeStr = PQescapeIdentifier(self->connection_, unsafeStr, strlen(unsafeStr));
	
	Handle<String> response = NanNew<String>(safeStr);
	
	free(safeStr);
	free(unsafeStr);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#escapeStringConn
NAN_METHOD(Connection::EscapeStringConn)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanReturnUndefined();
	}
	
	char* unsafeStr = MallocCString(args[0]);
	char* safeStr = (char *) malloc(strlen(unsafeStr)*2+1);
	int error;
	
	// call libpq
	size_t safeStr_length = PQescapeStringConn(self->connection_, safeStr, unsafeStr, strlen(unsafeStr), &error);
	
	Handle<String> response = NanNew<String>(safeStr,safeStr_length);
	
	free(safeStr);
	free(unsafeStr);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#execCommand
NAN_METHOD(Connection::ExecCommand)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("First parameter must be a string query");
	}
	
	char* queryText = MallocCString(args[0]);
	
	// call libpq
	PGresult *result = PQexec(self->connection_, queryText);
	
	// simply returns result status
	Local<Object> response = NanNew<Object>();
	char* status = PQresStatus(PQresultStatus(result));
	response->Set(NanNew<String>("status"), NanNew<String>(status));
    if( (PQresultStatus(result) != PGRES_COMMAND_OK) && (PQresultStatus(result) != PGRES_TUPLES_OK) ){
        char* errorMessage = PQresultErrorMessage(result);
        response->Set(NanNew<String>("errorMessage"), NanNew<String>(errorMessage));
    }
	
	PQclear(result);
	free(queryText);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#execQuery
NAN_METHOD(Connection::ExecQuery)
{
	NanScope();
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	if( !args[0]->IsString() ){
		NanThrowError("First parameter must be a string query");
	}
	
	char* queryText = MallocCString(args[0]);
	
	// call libpq
	PGresult *result = PQexec(self->connection_, queryText);
	
	// check result
	if( (PQresultStatus(result) != PGRES_COMMAND_OK) && (PQresultStatus(result) != PGRES_TUPLES_OK) ){
		LOG(PQresStatus(PQresultStatus(result)));
        char* errorMessage = PQresultErrorMessage(result);
		NanThrowError(errorMessage);
	}
	
	// PGresult to JavaScript object
	Handle<Array> response = HandleTuplesResult(result);
	
	PQclear(result);
	free(queryText);
	
	NanReturnValue(response);
}

//v8 entry point into Connection#execQueryWithParams
NAN_METHOD(Connection::ExecQueryWithParams)
{
	NanScope();
	//dispatch non-prepared parameterized query
	NanReturnValue(DispatchParameterizedQuery(args,false));
}

//v8 entry point into Connection#execPrepare(string queryName, string queryText, int nParams)
// 
//usage:
//  var res = con.execPrepare("s","select * from product where id=$1",1);
//  
NAN_METHOD(Connection::ExecPrepare)
{
	NanScope();
	
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	char* queryName = MallocCString(args[0]);
	char* queryText = MallocCString(args[1]);
	
	int length = args[2]->Int32Value();
	
	// call libpq
	PGresult *result = PQprepare(self->connection_, queryName, queryText, length, NULL);
	
	// check result
	if( PQresultStatus(result) != PGRES_COMMAND_OK ){
		LOG(PQresStatus(PQresultStatus(result)));
		NanThrowError("Can not execute prepare");
	}
	
	free(queryName);
	free(queryText);
	
	NanReturnUndefined();
}

//v8 entry point into Connection#execQueryPrepared(string queryName, string[] paramValues)
// 
//usage: parameter should be string
//  var res = con.execQueryPrepared("s",["1"]);
//  
NAN_METHOD(Connection::ExecQueryPrepared)
{
	NanScope();
	//dispatch prepared parameterized query
	NanReturnValue(DispatchParameterizedQuery(args,true));
}

Handle<Array> Connection::DispatchParameterizedQuery(_NAN_METHOD_ARGS, bool isPrepared)
{
	Connection *self = ObjectWrap::Unwrap<Connection>(args.This());
	
	String::Utf8Value queryName(args[0]);
	
	if(!args[0]->IsString()) {
		NanThrowError("First parameter must be a string");
	}
	
	if(!args[1]->IsArray()) {
		NanThrowError("Values must be an array");
	}
	
	Local<Array> jsParams = Local<Array>::Cast(args[1]);
	int len = jsParams->Length();
	
	char** paramValues = ArgToCStringArray(jsParams);
	if(!paramValues) {
		NanThrowError("Unable to allocate char **paramValues from Local<Array> of v8 params");
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
		NanThrowError("Can not execute parametarized query");
	}
	
	// PGresult to JavaScript object
	Handle<Array> response = HandleTuplesResult(result);
	
	PQclear(result);
	free(queryText);
	
	return response;
}


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 


//initializes initial sync connection to postgres via libpq
bool Connection::Connect(const char* conninfo)
{
	connection_ = PQconnectdb(conninfo);
	
	if ( PQstatus(connection_) != CONNECTION_OK ) {
		LOG("Unable to set connection");
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
	int rowCount = PQntuples(result);
	Local<Array> rows = NanNew<Array>();
	for(int rowNumber = 0; rowNumber < rowCount; rowNumber++) {
		//create result object for this row
		Local<Array> row = NanNew<Array>();
		int fieldCount = PQnfields(result);
		for(int fieldNumber = 0; fieldNumber < fieldCount; fieldNumber++) {
			Local<Object> field = NanNew<Object>();
			//name of field
			char* fieldName = PQfname(result, fieldNumber);
			field->Set(NanNew<String>("name"), NanNew<String>(fieldName));
			
			//oid of type of field
			int fieldType = PQftype(result, fieldNumber);
			field->Set(NanNew<String>("type"), NanNew<Integer>(fieldType));
			
			//value of field
			if(PQgetisnull(result, rowNumber, fieldNumber)) {
				field->Set(NanNew<String>("value"), NanNull());
			} else {
				char* fieldValue = PQgetvalue(result, rowNumber, fieldNumber);
				field->Set(NanNew<String>("value"), NanNew<String>(fieldValue));
			}
			row->Set(NanNew<Integer>(fieldNumber), field);
		}
		rows->Set(NanNew<Integer>(rowNumber),row);
	}
	return rows;
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





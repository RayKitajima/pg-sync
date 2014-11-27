
// usege:
//     
//     var connString = "host=127.0.0.1 port=5432 dbname=mydb";
//     
//     var pgsync = require('pg-sync');
//     var client = new pgsync.Client();
//     
//     client.connect(connString);
//     
//     client.begin();
//     client.setIsolationLevelSerializable();
//     client.isolationLevel(client.ISOLATION_LEVEL.SERIALIZABLE);
//     
//     var result = client.query(queryText);
//     var result = client.query(queryText,[params]);
//     
//     var result = client.exec(command); // returns result code only, even if the command was normal query
//     
//     var statement = client.prepare(queryText);
//     var result = statement.execute([params]);
//     
//     var queryName = "a_repeated_query";
//     var queryText = "select * from table where id=$1";
//     var statement = client.prepare(queryName,queryText);
//     [1,2,3,4].map( function(i){ statement.execute([i]); } );
//     
//     client.commit();
//     client.rollback();
//     
//     client.disconnect();
//     
//

var StatementModule = require(__dirname + '/statement');
var parser = require(__dirname + '/parser').Parser();
var utils = require(__dirname + '/utils');

var binding = require(__dirname + '/../build/Release/pgsync.node');

var ISOLATIONLEVEL_SERIALIZABLE    = 'serializable';
var ISOLATIONLEVEL_REPEATABLE_READ = 'repeatable read';
var ISOLATIONLEVEL_READ_COMMITED   = 'read commited';


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// constructor

function ClientConstructor(){
	// instance variable
	this.connection = new binding.Connection();
};

module.exports = {
	Client : ClientConstructor
};

var Client = ClientConstructor.prototype;

// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// static variables

Client.ISOLATION_LEVEL = {
	SERIALIZABLE    : ISOLATIONLEVEL_SERIALIZABLE,
	REPEATABLE_READ : ISOLATIONLEVEL_REPEATABLE_READ,
	READ_COMMITED   : ISOLATIONLEVEL_READ_COMMITED
};

// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// private, static method (not a instance/prototype method)
// be careful, you should provide your connection

var setIsolationLevel = function(connection,level){
	var queryText = 'set transaction isolation level ' + level;
	var result = connection.execCommand(queryText);
	return result; // result status
};

var dispatchParametalizedQuery = function(connection,queryName,queryText,params){
	for( var i=0, len=params.length; i<len; i++ ){
		params[i] = utils.prepareValue(params[i]);
	}
	var statement = new StatementModule(connection,queryName,queryText);
	var result = statement.execute(params);
	return parser.parse(result); // json
};


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// public APIs

Client.connect = function(connString){
	//this.connection = new binding.Connection();
	this.connection.connect(connString);
};

Client.begin = function(){
	var queryText = 'begin transaction';
	var result = this.connection.execQuery(queryText);
	return result;
};

Client.setAutoCommit = function(val){
	if( !val ){ val = 'ON' }
	if( val.toUpperCase() == 'ON' ){
		val = 'ON';
	}else{
		val = 'OFF';
	}
	var queryText = 'set autocommit='+val;
	var result = this.connection.execCommand(queryText);
	return result;
};

Client.isolationLevel = function(level){
	if( level != ISOLATIONLEVEL_SERIALIZABLE || level != ISOLATIONLEVEL_REPEATABLE_READ || level != ISOLATIONLEVEL_READ_COMMITED ){
		throw("invalid isolation level");
	}
	return setIsolationLevel(this.connection,level);
};

Client.setIsolationLevelSerializable = function(){
	return setIsolationLevel(this.connection,ISOLATIONLEVEL_SERIALIZABLE);
};

Client.setIsolationLevelRepeatableRead = function(){
	return setIsolationLevel(this.connection,ISOLATIONLEVEL_REPEATABLE_READ);
};

Client.setIsolationLevelReadCommited = function(){
	return setIsolationLevel(this.connection,ISOLATIONLEVEL_READ_COMMITED);
};

Client.exec = function(command){
	var result = this.connection.execCommand(command);
	return result;
};

Client.escapeLiteral = function(str){
	var result = this.connection.escapeLiteral(str);
	return result;
};

Client.escapeIdentifier = function(str){
	var result = this.connection.escapeIdentifier(str);
	return result;
};

Client.escapeStringConn = function(str){
	var result = this.connection.escapeStringConn(str);
	return result;
};

Client.query = function(){
	var queryName;
	var queryText;
	var params;
	
	if( arguments.length == 2 )
	{
		queryName = "";
		queryText = arguments[0];
		params    = arguments[1];
		
		return dispatchParametalizedQuery(this.connection,queryName,queryText,params);
	}
	else
	{
		queryText = arguments[0];
		
		var result = this.connection.execQuery(queryText);
		return parser.parse(result);
	}
};

Client.prepare = function(){
	var queryName;
	var queryText;
	
	if( arguments.length == 2 )
	{
		queryName = arguments[0];
		queryText = arguments[1];
	}
	else
	{
		queryName = "";
		queryText = arguments[0];
	}
	
	var statement = new StatementModule(this.connection,queryName,queryText);
	return statement;
};

Client.commit = function(){
	var queryText = 'commit';
	var result = this.connection.execCommand(queryText);
	return result; // result status
};

Client.rollback = function(){
	var queryText = 'rollback';
	var result = this.connection.execCommand(queryText);
	return result; // result status
};

Client.abort = function(){
	return this.rollback();
};

Client.disconnect = function(){
	this.connection.disconnect();
};


// backward compatibility for old misspelled methods
Client.setIsolationLebelSerializable   = Client.setIsolationLevelSerializable;
Client.setIsolationLebelRepeatableRead = Client.setIsolationLevelRepeatableRead;
Client.setIsolationLebelReadCommited   = Client.setIsolationLevelReadCommited;


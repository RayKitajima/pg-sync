
// usage:
//     
//     var Statement = require('statement');
//     var queryName_statement = new Statement(connection,queryName,queryText);
//     
// usage for enduser:
//     
//     var queryText = 'select * from table where field1=$1 and field2=$2 and field3=$3';
//     
//     var statement = client.prepare(queryText);
//     var statement = client.prepare(queryName,queryText);
//     
//     var prepared_statements = {
//         name1 : statement1,
//         name2 : statement2
//     };
//     
//     var result = prepared_statements.name1.execute([1,2,3]);
//     

var utils = require(__dirname + '/utils');


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// constructor

function StatementConstructor(){
	// instance variables
	this.connection = arguments[0];
	this.queryName  = arguments[1];
	this.queryText  = arguments[2];
	
	var nParams = this.queryText.match(/\$/g);
	
	this.connection.execPrepare(this.queryName,this.queryText,nParams);
};

module.exports = StatementConstructor;

var Statement = StatementConstructor.prototype;


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// public api

Statement.execute = function(params){
	if( !params ) params = [];
	for( var i=0, len=params.length; i<len; i++ ){
		params[i] = utils.prepareValue(params[i]);
	}
	var result = this.connection.execQueryPrepared(this.queryName,params);
	return result;
};



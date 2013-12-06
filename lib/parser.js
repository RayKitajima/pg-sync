
// usage:
//     
//     var parser = require('parser').Parser();
//     
//     var result = client.query(queryText);
//     var json = parser(result);
//     

var types = require(__dirname + '/types');


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// this is a static service module

var Parser = {}; // name space, will be set in the prototype

var parserConstructor = function(){
	return Parser;
};

module.exports = {
	Parser : parserConstructor
};


// *   *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    *    * 

// public api

Parser.parse = function(native_rows){
	var rows = [];
	
	for( var i=0; i<native_rows.length; i++ ){
		var native_row = native_rows[i];
		
		var row = {};
		for( var j=0; j<native_row.length; j++ ){
			var native_field = native_row[j];
			
			if( native_field.value != null )
			{
				var field_parser = types.getTypeParser(native_field.type,'text');
				var parsed_field = field_parser(native_field.value);
				row[native_field.name] = parsed_field;;
			}
			else
			{
				row[native_field.name] = null;
			}
		}
		rows.push(row);
	}
	
	return rows;
};



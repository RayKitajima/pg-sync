
/*

*/

var conninfo = "host=127.0.0.1 port=5432 dbname=myshop";

var pgsync = require('pg-sync');
var client = new pgsync.Client();
client.connect(conninfo);

var unsafeStr = "foo; delete from product; --";
var safeStr = client.escapeLiteral(unsafeStr);

console.log("unsafeStr:"+unsafeStr);
console.log("safeStr:"+safeStr);

var unsafeStr = "might 'malicious' data";
var safeStr = client.escapeStringConn(unsafeStr);

console.log("unsafeStr:"+unsafeStr);
console.log("safeStr:"+safeStr);

var unsafeName = "order";
var safeName = client.escapeIdentifier(unsafeName);

console.log("unsafeName:"+unsafeName);
console.log("safeName:"+safeName);

// also escaped
var normalName = "product";
var processedName = client.escapeIdentifier(normalName);
console.log("normalName:"+normalName);
console.log("processedName:"+processedName);


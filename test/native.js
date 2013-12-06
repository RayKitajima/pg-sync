
// 
// make sure you are running postmaster with this configuration
// 
var conninfo = "host=127.0.0.1 port=5432 dbname=myshop";

var pgsync = require('../build/Release/pgsync');
console.log("pgsync:"+pgsync);

var con = new pgsync.Connection();
console.log("connection:"+con);

console.log("---");
console.log("making connection");
con.connect(conninfo);
console.log("connected");

console.log("---");
console.log("simple select");
var res1 = con.execQuery("select * from product");
console.log(res1);

console.log("---");
console.log("prepare");
var res2 = con.execPrepare("","select * from product where id=$1",1);
console.log(res2);

console.log("---");
console.log("exec with prepared");
var res3 = con.execQueryPrepared("",["1"]);
console.log(res3);

console.log("---");
con.disconnect();
console.log("then disconnected");


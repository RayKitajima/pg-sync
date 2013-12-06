
/*

make sure you are running postmaster with following configuration:

host   : 127.0.0.1
port   : 5432
dbname : myshop

create table product ( id int, name text, price int );
create sequence productSeq start 10000;
insert into product values ( 1, 'Product_1', 100 );
insert into product values ( 2, 'Product_2', 110 );
insert into product values ( 3, 'Product_3', 200 );
insert into product values ( 4, 'Product_4', 150 );
insert into product values ( 5, 'Product_5', 900 );

create table "order" ( oid int, customer text );
create sequence orderSeq start 10000;
insert into "order" values ( 200, 'Customer_1' );

create table orderedProduct ( opid int, oid int, pid int );
create sequence orderedProductSeq start 10000;
insert into orderedProduct values ( 301, 200, 1 );
insert into orderedProduct values ( 302, 200, 2 );

create table orderOrderedProduct ( oid int, opid int );
insert into orderOrderedProduct values ( 200, 301 );
insert into orderOrderedProduct values ( 200, 302 );

*/

var conninfo = "host=127.0.0.1 port=5432 dbname=myshop";

var pgsync = require('pg-sync');
var client = new pgsync.Client();
client.connect(conninfo);

var res;

console.log("---");
console.log("select collection");
res = client.query("select * from orderOrderedProduct");
console.log(res);
console.log("---");

console.log("---");
console.log("select collection (2)");
res = client.query("select * from orderOrderedProduct where oid=200");
console.log(res);
console.log("---");

console.log("---");
console.log("select collection (3)");
res = client.query("select opid from orderOrderedProduct where oid=200");
console.log(res);
console.log("---");

client.disconnect();



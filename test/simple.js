
/*

make sure you are running postmaster with following configuration:

host   : 127.0.0.1
port   : 5432
dbname : myshop

//create table product ( productID int4, productName text, price int4, UNIQUE(productID) );
create table product ( id int, name text, price int );
create sequence productSeq start 10000;

insert into product values ( 1, 'Product_1', 100 );
insert into product values ( 2, 'Product_2', 110 );
insert into product values ( 3, 'Product_3', 200 );
insert into product values ( 4, 'Product_4', 150 );
insert into product values ( 5, 'Product_5', 900 );

*/

var conninfo = "host=127.0.0.1 port=5432 dbname=myshop";

var pgsync = require('pg-sync');
var client = new pgsync.Client();
client.connect(conninfo);

var res;

console.log("---");
console.log("select test");
res = client.query("select * from product");
console.log(res);

console.log("---");
console.log("auto commit mode:");
client.setAutoCommit('on');

console.log("inserting new row:");
res = client.query("insert into product values ( 6, 'Product_6', 530 )");
console.log(res);

console.log("check the row:");
res = client.query("select * from product where id=6");
console.log(res);

console.log("delete the row:");
res = client.query("delete from product where id=6");
console.log(res);

console.log("check the row:");
res = client.query("select * from product where id=6");
console.log(res);

console.log("new id");
res = client.query("select nextval('productSeq')");
console.log(res);

var newid = res[0].nextval;
console.log("inserting new row with prepared statement:");
var stm = client.prepare("insert into product values ( $1, $2, $3 )");
res = stm.execute([newid,'Product_new',330]);
console.log(res);

console.log("check the row:");
res = client.query("select * from product where id=$1",[newid]);
console.log(res);

console.log("---");
console.log("transaction:");
client.begin();
client.setIsolationLevelSerializable();

console.log("inserting volatile row:");
res = client.query("insert into product values ( 10, 'Product_10', 800 )");
console.log(res);

console.log("check the row:");
res = client.query("select * from product where id=10");
console.log(res);

console.log("roleback:");
res = client.rollback();
console.log(res);

console.log("check the row:");
res = client.query("select * from product where id=10");
console.log(res);

console.log("---");
console.log("done. disconnecting");
client.disconnect();




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

create table "order" ( id int, customer text, date date );
insert into "order" values ( 1, 'Steve', '2013-02-21' );
insert into "order" values ( 2, 'Tim', '2013-02-22' );
insert into "order" values ( 3, 'Bill', '2013-02-23' );

*/

var conninfo = "host=127.0.0.1 port=5432 dbname=myshop";

var pgsync = require('pg-sync');
var client = new pgsync.Client();
client.connect(conninfo);

var res;

// number
console.log("---");
console.log("selectby price:");
res = client.query("select * from product where price>$1 and price<$2",[100,300]);
console.log(res);

// text
console.log("---");
console.log("selectby name:");
res = client.query("select * from product where name like $1",['%_3%']);
console.log(res);

// date
console.log("---");
console.log("selectby date:");
res = client.query("select * from \"order\" where date > $1",['2013-02-22']);
console.log(res);

console.log("---");
console.log("done. disconnecting");
client.disconnect();



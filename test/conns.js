
/*

make sure you are running postmaster with following configuration:

host   : 127.0.0.1
port   : 5432
dbname : myshop
dbname : myshop2

// myshop

create table product ( id int, name text, price int );
create sequence productSeq start 10000;

insert into product values ( 1, 'Product_1', 100 );
insert into product values ( 2, 'Product_2', 110 );
insert into product values ( 3, 'Product_3', 200 );
insert into product values ( 4, 'Product_4', 150 );
insert into product values ( 5, 'Product_5', 900 );

// myshop2

create table shop ( id int, name text, city text );
create sequence shopSeq start 10000;

insert into shop values ( 1, 'SHOP_1', 'Ginza'    );
insert into shop values ( 2, 'SHOP_2', 'Shibuya'  );
insert into shop values ( 3, 'SHOP_3', 'Shinjuku' );

*/

var p_conninfo = "host=127.0.0.1 port=5432 dbname=myshop";
var s_conninfo = "host=127.0.0.1 port=5432 dbname=myshop2";

var pgsync = require('pg-sync');

var p_client = new pgsync.Client();
p_client.connect(p_conninfo);

var s_client = new pgsync.Client();
s_client.connect(s_conninfo);

var res;

console.log("---");
console.log("select products");
res = p_client.query("select * from product");
console.log(res);

console.log("---");
console.log("select shops");
res = s_client.query("select * from shop");
console.log(res);



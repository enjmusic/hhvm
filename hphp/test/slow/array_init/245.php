<?hh

class MyClass {
 function __toString() {
 return 'foo';
 }
 }

<<__EntryPoint>>
function main_245() {
$obj = new MyClass();
$arr = array($obj => 1, new MyClass() => 2, false => 3, true => 4, count(array(1,2,3)) => 5);
var_dump($arr);
}

<?hh
function func1(){
        $string = 'what the word and the other word the';
  preg_match_all_with_matches('/(?P<word>the)/', $string, &$matches);
  return $matches['word'];
}
$words = func1();
var_dump($words);

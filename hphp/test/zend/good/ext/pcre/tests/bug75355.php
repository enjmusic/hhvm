<?hh
<<__EntryPoint>> function main(): void {
var_dump(preg_quote('#'));

  var_dump(preg_match_with_matches(
    '~^('.preg_quote('hello#world', '~').')\z~x',
    'hello#world',
    &$m,
  ));

  var_dump($m[1]);
}

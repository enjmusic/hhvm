<?hh // strict
// Copyright 2004-present Facebook. All Rights Reserved.

final class Child extends Base<int> {

  use ATrait;
}

trait ATrait {

  require extends Base<mixed>;
}

class Base<+T> {
  /* HH_FIXME[4110] */
  public function get(): T {
  }
}

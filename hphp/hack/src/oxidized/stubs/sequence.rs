// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the "hack" directory of this source tree.

#[derive(Debug)]
pub struct Sequence<T>(T);

impl<T: Into<ocamlrep::Value>> Into<ocamlrep::Value> for Sequence<T> {
    fn into(self) -> ocamlrep::Value {
        ().into()
    }
}

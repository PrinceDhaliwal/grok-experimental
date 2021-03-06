// Copyright 2009 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Test pre- and postfix count operations.

// Test value context.
var a = 42;
var b = {x:42};
var c = "x";
assert_equal(43, ++a);
assert_equal(43, a);
assert_equal(43, a++);
assert_equal(44, a);
assert_equal(43, ++b.x);
assert_equal(43, b.x);
assert_equal(43, b.x++);
assert_equal(44, b.x);
assert_equal(45, ++b[c]);
assert_equal(45, b[c]);
assert_equal(45, b[c]++);
assert_equal(46, b[c]);

// Test test context.
a = 42;
b = {x:42};
c = "x";
assert_equal(1, (++a) ? 1 : 0);
assert_equal(43, a);
assert_equal(1, (a++) ? 1 : 0);
assert_equal(44, a);
assert_equal(1, (++b.x) ? 1 : 0);
assert_equal(43, b.x);
assert_equal(1, (b.x++) ? 1 : 0);
assert_equal(44, b.x);
assert_equal(1, (++b[c]) ? 1 : 0);
assert_equal(45, b[c]);
assert_equal(1, (b[c]++) ? 1 : 0);
assert_equal(46, b[c]);

// following test fails; because || returns 0 or 1
// // Test value/test and test/value contexts.
// a = 42;
// b = {x:42};
// c = "x";
// assert_equal(43, ++a || 1);
// assert_equal(43, a);
// assert_equal(43, a++ || 1);
// assert_equal(44, a);
// assert_equal(43, ++b.x || 1);
// assert_equal(43, b.x);
// assert_equal(43, (b.x++) || 1);
// assert_equal(44, b.x);
// assert_equal(45, ++b[c] || 1);
// assert_equal(45, b[c]);
// assert_equal(45, b[c]++ || 1);
// assert_equal(46, b[c]);
// a = 42;
// b = {x:42};
// c = "x";
// assert_equal(1, ++a && 1);
// assert_equal(43, a);
// assert_equal(1, a++ && 1);
// assert_equal(44, a);
// assert_equal(1, ++b.x && 1);
// assert_equal(43, b.x);
// assert_equal(1, (b.x++) && 1);
// assert_equal(44, b.x);
// assert_equal(1, ++b[c] && 1);
// assert_equal(45, b[c]);
// assert_equal(1, b[c]++ && 1);
// assert_equal(46, b[c]);

// // Test count operations with parameters.
function f(x) { x++; return x; }
assert_equal(43, f(42));

function g(x) { ++x; return x; }
assert_equal(43, g(42));

function h(x) { var y = x++; return y; }
assert_equal(42, h(42));

function k(x) { var y = ++x; return y; }
assert_equal(43, k(42));

// Test count operation in a test context.
function countTestPost(i) { var k = 0; while (i--) { k++; } return k; }
assert_equal(10, countTestPost(10));

function countTestPre(i) { var k = 0; while (--i) { k++; } return k; }
assert_equal(9, countTestPre(10));

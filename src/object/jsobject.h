///////////////////////////////////////////////////////////////////////////////
//
//    +-----------------------------------------------------------+
//    |          Grok : A Naive JavaScript Interpreter            |
//    +-----------------------------------------------------------+
//
// Copyright 2015 Pushpinder Singh
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
//  \file:  jsobject.h
//  \description:  definition of JSBasicObject and other related classes
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef JS_OBJECT_H_
#define JS_OBJECT_H_

#include "object/object.h"
#include "common/exceptions.h"

#include <memory>
#include <string>

extern std::string __type[7];

#define DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(op, type) \
    { throw Exception(  \
            std::string("operator '" #op "' is not defined for '" \
                #type "'")); }

#define DEFINE_OPERATOR(op, type)       \
    virtual Object operator op(Object ) \
        DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(op, type)

class JSArray;
class JSObject;
class JSString;
// JSBasicObject: super class for every javascript object
// and javascript functions
// JSBasicObject
//    JSNumber
//    JSString
//    JSObject
//    JSArray
//    JSFunction
class JSBasicObject {
public:
  // type of javascript objects
  enum class ObjectType {
    _null,
    _undefined,
    _number,
    _string,
    _object,
    _array,
    _function
  };

  // default constructor
  JSBasicObject() = default;

  JSBasicObject(const JSBasicObject &obj) = default;
  JSBasicObject &operator=(const JSBasicObject &rhs) = default;
  virtual ~JSBasicObject() = default;

  virtual std::string ToString() const { return "undefined"; }

  virtual void print(std::ostream &os) const {
    os << "Type: " << __type[static_cast<int>(GetType())];
  }

  virtual long long &GetNumber() {
    return k;
  }

  virtual ObjectType
  GetType() const { // returns the type of the javascript object
    return ObjectType::_undefined;
  }

  template <ObjectType T> bool is() { return GetType() == T; }

  virtual void reset(std::shared_ptr<JSBasicObject> ptr) { return; }

  static long long k;
};


class JSString : public JSBasicObject {
public:
  JSString(const std::string &str) : js_string_(str) {}

  JSString() : js_string_("") {}

  ~JSString() {}

  inline std::string &GetString() { return js_string_; }
  inline std::string GetString() const { return js_string_; }
  ObjectType GetType() const override { return ObjectType::_string; }
  std::string ToString() const override { return js_string_; }

  std::shared_ptr<JSBasicObject> At(int i) {
    return std::dynamic_pointer_cast<JSBasicObject, JSString>(
        std::make_shared<JSString>(std::string() + (js_string_[i])));
  }

private:
  std::string js_string_;
};

// JSNumber class holding a javascript number
// currently it supports only int64_t not double's
class JSNumber : public JSBasicObject {
public:
  // derive some constructors
  using JSBasicObject::JSBasicObject;

  JSNumber(long long num) : number_(num) {}

  JSNumber(const std::string &str) { number_ = std::stoll(str); }

  JSNumber(const JSNumber &number) : number_(number.number_) {}

  JSNumber &operator=(const JSNumber &rhs) {
    number_ = (rhs.number_);
    return *this;
  }

  JSNumber() : number_(0) {}

  long long &GetNumber() override { return number_; }

  ~JSNumber() {}

  inline long long GetValue() const { return number_; }
  inline long long &GetValue() { return number_; }
  ObjectType GetType() const override { return ObjectType::_number; }
  std::string ToString() const override { return std::to_string(number_); }

private:
  long long number_;
};

extern Object operator+(std::shared_ptr<JSNumber> l, Object r);

#define DECLARE_OPERATOR(op)  \
extern Object operator op(std::shared_ptr<JSNumber> l, Object r)
DECLARE_OPERATOR(-);
DECLARE_OPERATOR(*);
DECLARE_OPERATOR(/);
DECLARE_OPERATOR(%);
#undef DECLARE_OPERATOR

extern Object operator +(std::shared_ptr<JSString> l, Object r);
extern Object operator+(Object l, Object r);

#define DECLARE_OTHER_OPERATOR(op) \
extern Object operator op (Object l, Object r)

DECLARE_OTHER_OPERATOR(*);
DECLARE_OTHER_OPERATOR(-);
DECLARE_OTHER_OPERATOR(/);
DECLARE_OTHER_OPERATOR(%);
#undef DECLARE_OTHER_OPERATOR

#endif


#include "object/prototype.h"

namespace grok {
namespace obj {

void AddPropertiesFromPrototype(JSObject *ctor, JSObject *obj)
{
    std::shared_ptr<Object> proto_wrapped = ctor->GetProperty("prototype");

    // if proto is undefined then we are done
    if (IsUndefined(proto_wrapped)) {
        return; // nothing to be done
    }

    if (ctor->GetType() == ObjectType::_array
        || ctor->GetType() == ObjectType::_bool
        || ctor->GetType() == ObjectType::_number)
        return;

    std::shared_ptr<JSObject> proto = proto_wrapped->as<JSObject>();

    for (auto &property : *proto) {
        obj->AddProperty(property.first, CreateCopy(property.second));
    }
}

}
}

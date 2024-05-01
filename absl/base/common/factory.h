#pragma once

#include <map>
#include <memory>
#include <functional>

#include "common/log.h"
#include "common/singleton.h"
#include "common/pointer_related.h" //reinterpret_pointer_cast

namespace mlf {

template<class T>
class Factory : public Singleton<Factory<T>> {
public:
    typedef std::function<std::shared_ptr<T>()> producer_t;

    template<class TT>
    void add(const std::string& name) {
        add(name, []()->std::shared_ptr<TT> {
            return std::make_shared<TT>();
        });
    }

    void add(const std::string& name, producer_t producer) {
        MCHECK(_name2producer.insert({name, producer}).second) << "Factory item[" <<
            name << "] already exists";
    }

    template<class TT = T>
    std::shared_ptr<TT> produce(const std::string& name) {
        auto it = _name2producer.find(name);
        MCHECK(it != _name2producer.end()) << "Factory item[" << name << "] not found";
        std::shared_ptr<T> obj = it->second();
        MCHECK(obj) << "Factory item is not found, name: " << name;
        std::shared_ptr<TT> x = std::dynamic_pointer_cast<TT>(obj); 
        //std::shared_ptr<TT> x = reinterpret_pointer_cast<TT>(obj);
        MCHECK(x) << "Factory item[" << name << "] can not cast from " 
                 << typeid(*obj).name() << " to " << typeid(TT).name();
        return x;
    }

    std::vector<std::string> get_all_names() {
        std::vector<std::string> names;
        for (auto it = _name2producer.begin(); it != _name2producer.end(); it++) {
            names.push_back(it->first);
        }
        return names;
    }

    bool exist(const std::string & name) {
        return (_name2producer.find(name) != _name2producer.end());
    }

private:
    std::map<std::string, producer_t> _name2producer;
};

#define DECLARE_CLASS_FACTORY(BaseClass) \
    typedef Factory<BaseClass> BaseClass##Factory; \
    template<class T> \
    class BaseClass##Register { \
        public: \
            BaseClass##Register(const std::string & class_name) { \
                if (!BaseClass##Factory::instance().exist(class_name)) { \
                    BaseClass##Factory::instance().add<T>(class_name); \
                }\
            } \
    };

#define REGISTER_CLASS_TO_FACTORY(BaseClass, DerivedClass, class_name) \
    static BaseClass##Register<DerivedClass> _##DerivedClass##_register(class_name);

#define PRODUCE_INSTANCE_FROM_FACTORY(BaseClass, class_name) \
    BaseClass##Factory::instance().produce(class_name);
} //namespace mlf

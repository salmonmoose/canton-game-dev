#ifndef FACTORY_H
#define FACTORY_H

#include <map>
#include <functional>
#include <memory>

template <typename T>

T* newT() { 
    return new T(); 
}

template <typename Key, typename Abstraction>

struct generic_factory
{
    typedef std::function<Abstraction*()> creator_t;

    std::shared_ptr<Abstraction> build_shared_ptr(Key const& key)
    {
    	auto fit = creators.find(key);

    	if(fit == creators.end())
    	{
    		printf("Didn't find: %s\n", key.c_str());
    		return 0;
    	}

    	return std::shared_ptr<Abstraction>(fit->second());
    }

    Abstraction* build_object(Key const& key) 
    {
        auto fit = creators.find(key);
        
        if (fit == creators.end()) 
        {
            printf("Didn't find: %s\n", key.c_str());
            return 0;
        }

        return fit->second();
    }

    bool register_creator(Key const& key, creator_t const& creator) {
        creators[key] = creator;
        return true;
    }

    template <typename T>
    bool register_type(Key const& key) {
    	printf("Registering %s\n", key.c_str());
        register_creator(key, newT<T>);
        return true;
    }

private:
    std::map<Key, creator_t> creators;
};
#endif
#ifndef GENERICFACTORY_H
#define GENERICFACTORY_H

#include <map>
#include <functional>
#include <memory>

template <typename GenericType>

GenericType* newGenericType() { 
    return new GenericType(); 
}

template <typename Key, typename Abstraction>

struct Factory
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

    template <typename GenericType>
    bool register_type(Key const& key) {
    	printf("Registering %s\n", key.c_str());
        register_creator(key, newGenericType<GenericType>);
        return true;
    }

private:
    std::map<Key, creator_t> creators;
};
#endif
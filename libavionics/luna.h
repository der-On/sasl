#ifndef __LUNA_H__
#define __LUNA_H__


#include <string>


extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


namespace xa {


class Avionics;


/// Simple LUA wrapper
class Luna
{
    private:
        /// Lua virtual machine
        lua_State *lua;
        #if USE_EXTERNAL_ALLOCATOR
            void *ud;
        #endif
        
    public:
        /// Create LUA instance
        Luna();

        /// Destroy LUA virtual machine
        ~Luna();

    public:
        /// Execute LUA script.
        /// Returns true on errors.
        /// \param fileName path to script
        bool runScript(const std::string &fileName);

        /// Returns reference to LUA virtual machine
        lua_State* getLua() { return lua; }

        /// Returns value of table field as float
        /// \param tableIdx index of table
        /// \param field name of field
        float getFieldf(int tableIdx, const std::string &field, 
                float dflt=0.0f);

        /// Returns value of table field as double
        /// \param tableIdx index of table
        /// \param field name of field
        double getFieldd(int tableIdx, const std::string &field,
                double dflt=0.0);

        /// Returns value of table field as string
        /// \param tableIdx index of table
        /// \param field name of field
        std::string getFields(int tableIdx, const std::string &field,
                const std::string &dflt=std::string());

        /// Returns value of table field as booleab
        /// \param tableIdx index of table
        /// \param field name of field
        bool getFieldb(int tableIdx, const std::string &field,
                bool dflt=false);

        /// Returns value of table field as integer
        /// \param tableIdx index of table
        /// \param field name of field
        int getFieldi(int tableIdx, const std::string &field, int dflt=0);

        /// Create reference to object at top of stack.  
        /// Pops value from stack.
        /// Returns reference number
        int addRef();

        /// Push referenced object to stack
        /// \param ref reference number
        void getRef(int ref);

        /// Remove reference to object
        /// \param ref reference number
        void unRef(int ref);

        /// Store data in registry table
        void storeAvionics(Avionics *avionics);
};


Avionics* getAvionics(lua_State *lua);

};

#endif

